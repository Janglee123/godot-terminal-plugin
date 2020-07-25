#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <Godot.hpp>
#include <Resource.hpp>
#include <RegEx.hpp>
#include <RegExMatch.hpp>

#include <pty.h>
#include <termios.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#ifndef NSIG
#define NSIG 32
#endif

extern char **environ;

namespace godot {

class Terminal : public Resource {
    GODOT_CLASS(Terminal, Reference)

private:
	int fd;
	FILE *fd_file;
	static pid_t _fork_pty(int *p_master, char *name, const termios *termp, const winsize *size);
	int _non_block(int pid);
public:
    static void _register_methods();

    Terminal();
    ~Terminal();
    String read();
    void fork(String file, Array args, Array env, String cwd, int cols, int rows);
    void resize(int cols, int rows);
    void get_proc();
	Array get_envs();
	int get_buffer_bytes(int fd);
	void write(String data);
    void _init(); // our initializer called by Godot
	String parse_es(String es);
};

}

#endif