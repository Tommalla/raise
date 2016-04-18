#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

char *buf1;

static void segv_handler(int sig, siginfo_t *si, void *unused)
{
    if (si->si_addr == buf1)
        exit(0);
}

int main() {
    struct sigaction sa;
    buf1 = mmap(NULL, 0x2000, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buf1 == MAP_FAILED) {
        perror("mmap buf1");
        exit(1);
    }
    raise(SIGQUIT);

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segv_handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    /* this should trigger SEGV */
    buf1[0] = 42;
    exit(1);
}
