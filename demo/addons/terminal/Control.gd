extends Control
class_name Term
tool

const pty : NativeScript = preload("res://addons/terminal/bin/Terminal.gdns")

onready var screen := $Panel/TextEdit as RichTextLabel
onready var term := pty.new() as Terminal

var from_pty := "" 
var from_user := ""


func _ready():
	screen.text = ""
	setup_term()


func _process(delta):
	
	if not is_active():
		return
	
	var t : String = term.read()
	if t.length() > 0: 
		from_pty += t
	
	screen.text = from_pty + from_user


func _input(event):
	
	if not event is InputEventKey:
		return
	
	if event.is_echo() or event.is_pressed():
		return
	
	if event.scancode == KEY_BACKSPACE:
		from_user.erase(from_user.length()-1, 1)
		return
	
	if event.scancode == KEY_ENTER:
		term.write(from_user + "\n")
		from_user = ""
		return
	
	var character = char(event.unicode)
	from_user += character


func forward_input(event : InputEvent) -> bool:
	return event is InputEventKey and is_active()


func set_active(value : bool) -> void:
	if value:
		screen.grab_focus()


func is_active() -> bool:
	return screen.has_focus()


func setup_term() -> void:
	var envs : Array = term.get_envs()
	envs.push_back("TERM=xterm")
	term.fork("/bin/bash", [], envs, ProjectSettings.globalize_path("res://"), 80, 28);
