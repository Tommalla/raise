#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main() {
    char buf[512];
    int ret;

    while(1) {
        ret = read(0, buf, sizeof(buf));
        // 512 ERESTARTSYS
        if (ret == -1 && (errno == EINTR || errno == 512))
            continue;
        if (ret <= 0)
            break;
        write(1, buf, ret);
    }
    return ret;
}
