#include "Terminal.hpp"

using namespace godot;

void Terminal::_register_methods() {
	godot::register_method("read", &Terminal::read);
	godot::register_method("fork", &Terminal::fork);
	godot::register_method("write", &Terminal::write);
	godot::register_method("get_envs", &Terminal::get_envs);
}

Terminal::Terminal() {
}

Terminal::~Terminal() {
	// add your cleanup here
}

int Terminal::get_buffer_bytes(int fd) {
	int p;
	if (ioctl(fd, FIONREAD, &p) == -1) {
		return 0;
	} else {
		return p;
	}

	return 0;
}

String Terminal::read() {

	if (fd == 0) {
		return String();
	}

	int l = get_buffer_bytes(fd);
	char str[l + 1];
	String result, esc_seq = "";
	bool is_inside_esc_seq = false;

	for (int i = 0; i < l; i++) {
		char c = fgetc(fd_file);
		// printf("%c", c);

		if (c == '\e') {
			is_inside_esc_seq = true;
			continue;
		}

		if (is_inside_esc_seq) {
			if (c == 'm') {
				is_inside_esc_seq = false;
			}
		} else {
			result += String(c);
		}
	}

	return result;
}

String Terminal::parse_es(String es) {
	return String();
}

void Terminal::fork(String exec, Array args, Array env, String cwd, int cols, int rows) {

	struct winsize size;
	size.ws_col = cols;
	size.ws_row = rows;
	size.ws_xpixel = 0;
	size.ws_ypixel = 0;

	struct termios t = termios();
	struct termios *term = &t;
	term->c_iflag = ICRNL | IXON | IXANY | IMAXBEL | BRKINT | IUTF8;

	term->c_oflag = OPOST | ONLCR;
	term->c_cflag = CREAD | CS8 | HUPCL;
	term->c_lflag =
			ICANON | ISIG | IEXTEN | ECHO | ECHOE | ECHOK | ECHOKE | ECHOCTL;

	term->c_cc[VEOF] = 4;
	term->c_cc[VEOL] = -1;
	term->c_cc[VEOL2] = -1;
	term->c_cc[VERASE] = 0x7f;
	term->c_cc[VWERASE] = 23;
	term->c_cc[VKILL] = 21;
	term->c_cc[VREPRINT] = 18;
	term->c_cc[VINTR] = 3;
	term->c_cc[VQUIT] = 0x1c;
	term->c_cc[VSUSP] = 26;
	term->c_cc[VSTART] = 17;
	term->c_cc[VSTOP] = 19;
	term->c_cc[VLNEXT] = 22;
	term->c_cc[VDISCARD] = 15;
	term->c_cc[VMIN] = 1;
	term->c_cc[VTIME] = 0;

	char name[40];
	pid_t pid = forkpty(&fd, name, term, &size);

	switch (pid) {
		case -1: {
			Godot::print_error("forkpty(3) faild.", __FUNCTION__, __FILE__, __LINE__);
			return;
		}
		case 0: {
			// child process
			if (cwd.length() > 0) {
				if (chdir(cwd.alloc_c_string()) == -1) {
					Godot::print_error("chdir(2) faild.", __FUNCTION__, __FILE__, __LINE__);
					return;
				}
			}

			char const *args_ptr[args.size() + 2];
			args_ptr[0] = exec.alloc_c_string();
			for (int i = 0; i < args.size(); i++) {
				String arg_i = args[i];
				args_ptr[i + 1] = arg_i.alloc_c_string();
			}
			args_ptr[args.size() + 1] = nullptr;

			char const *env_ptr[env.size() + 1];
			for (int i = 0; i < env.size(); i++) {
				String env_i = env[i];
				env_ptr[i] = env_i.alloc_c_string();
			}
			env_ptr[env.size()] = nullptr;

			execvpe(args_ptr[0], (char *const *)args_ptr, (char *const *)env_ptr);
			Godot::print("execvp faild");
		} break;
		default:
			Godot::print("pid: " + String::num_int64(pid));
			Godot::print("fd: " + String::num_int64(fd));
			Godot::print("name: " + String(name));
			fd_file = fdopen(fd, "wr+");
			if (!fd_file) {
				Godot::print("Error fdopen faild");
			}
			break;
	}
}

void Terminal::resize(int cols, int rows) {
	winsize size;
	size.ws_col = cols;
	size.ws_row = rows;
	size.ws_xpixel = 0;
	size.ws_ypixel = 0;

	if (ioctl(fd, TIOCSWINSZ, &size) == -1) {
		//error bruh
	}
}

Array Terminal::get_envs() {
	Array envs;
	int i = 1;
	for (char **env = environ; *env; ++env, i++) {
		envs.push_back(String(*env));
	}

	return envs;
}

void Terminal::write(String data) {
	fputs(data.alloc_c_string(), fd_file);
}

void Terminal::_init() {
	// initialize any variables here
	Godot::print("Init Ready");
}
