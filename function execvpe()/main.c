#include <unistd.h>
#include <stdlib.h>

int execvpe (char *file, char *argv[], char *envp[]) {
	extern char **environ;
	environ = envp;
	execvp(file, argv);
	return -1;
}

int main (int argc, char *argv[], char *envp[]) {
	execvpe(argv[1], &argv[1], envp);
	exit (1);
}