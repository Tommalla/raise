#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

int main() {
    char *buf1, *buf2;
    FILE *maps = fopen("/proc/self/maps", "r");
    char name[128];
    int found = 0;
    void *stack_start, *stack_end, *last_addr;
    void *alloc_end = (void*)0xc0000000;
    int eof = 0;
    if (maps == NULL) {
        perror("fopen");
        exit(1);
    }
    // bfbdc000-bfbfd000 rw-p 00000000 00:00 0          [stack]
    while (!eof && !found) {
        switch (fscanf(maps, "%p-%p %*s %*s %*s %*d%*[ ]%[^ \n]\n", &stack_start, &stack_end, name)) {
            case 3:
                if (strcmp(name, "[stack]") == 0) {
                    found = 1;
                    break;
                }
                /* fall through */
            case 2:
                last_addr = stack_end;
                break;
            default:
                eof = 1;
                break;
        }
    }
    if (!found) {
        fprintf(stderr, "Stack not found\n");
        exit(1);
    }
    fclose(maps);
    /* on x86_64, userspace is not limited to 0xc0000000 */
    if (stack_end < alloc_end) {
        buf1 = mmap(stack_end, alloc_end - stack_end, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
        if (buf1 == MAP_FAILED) {
            perror("mmap buf1");
            exit(1);
        }
    }
    buf2 = mmap(last_addr, stack_start - last_addr, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
    if (buf2 == MAP_FAILED) {
        perror("mmap buf2");
        exit(1);
    }
    raise(SIGQUIT);
    buf1[0] = 42;
    buf1[alloc_end - stack_end - 1] = 42;
    buf2[0] = 42;
    buf2[stack_start - last_addr - 1] = 42;
    return 0;
}
