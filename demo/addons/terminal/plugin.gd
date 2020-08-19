tool
extends EditorPlugin

const UI: PackedScene = preload("res://addons/terminal/Control.tscn")
const pty: NativeScript = preload("res://addons/terminal/native/Terminal.gdns")
const TITLE: String = "Terminal"
var dock : Control
var term : Terminal
var screen : RichTextLabel
var from_pty := "" 
var from_user := ""
var has_change := false


func _enter_tree():
	dock = UI.instance()
	add_control_to_bottom_panel(dock, TITLE)
	
	var editor_viewport = get_editor_interface().get_editor_viewport()
	yield(get_tree(), 'idle_frame')
	dock.rect_min_size.y = editor_viewport.rect_size.y * 0.49


func _exit_tree():
	remove_control_from_bottom_panel(dock)
	dock.free()


func handles(object : Object) -> bool:
	return object is Term


func make_visible(visible : bool) -> void:
	dock.set_active(visible)
	update_overlays()


func forward_canvas_gui_input(event : InputEvent) -> bool:
	if event is InputEventKey:
		print("KeyEvent")
	if not dock or not dock.is_inside_tree():
		return false
	return dock.forward_input(event) 


func forward_canvas_draw_over_viewport(overlay):
	pass
	
	

#
#func setup_pty():
#	var envs : Array = term.get_envs()
#	envs.push_back("TERM=xterm")
#	term.fork("/bin/bash", [], envs, ProjectSettings.globalize_path("res://"), 80, 28);
#	term.open()
