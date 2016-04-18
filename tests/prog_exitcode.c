#include <unistd.h>
#include <signal.h>

int main() {
    raise(SIGQUIT);
    return 42;
}
