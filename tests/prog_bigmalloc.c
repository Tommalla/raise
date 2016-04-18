#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main() {
    char *buf;
    raise(SIGQUIT);
    buf = malloc(0x2000000);
    if (buf == NULL)
        return 1;
    return 0;
}
