#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main() {
    char buf[512];
    int ret;

    raise(SIGQUIT);
    ret = read(0, buf, sizeof(buf));
    if (ret <= 0)
        return 1;
    write(1, buf, ret);
}
