#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    pid_t pid;
    int do_kill = 0;
    int status;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [--kill] prog arg ...\n", argv[0]);
        exit(2);
    }
    if (strcmp(argv[1], "--kill")==0)
        do_kill = 1;

    switch (pid=fork()) {
        case 0:
            setsid();
            execv(argv[1+do_kill], argv+1+do_kill);
            perror("execve");
            exit(1);
        case -1:
            perror("fork");
            exit(1);
        default:
            if (do_kill) {
                sleep(1);
                kill(pid, SIGQUIT);
            }
            waitpid(pid, &status, 0);
            if (!WIFSIGNALED(status) || !WCOREDUMP(status)) {
                fprintf(stderr, "Unexpected exit status - core not produced\n");
                exit(1);
            }
            printf("core.%d\n", pid);
            return 0;
    }
}
