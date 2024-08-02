#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "../utils.h"

int main() {
    int count = 0;
    logger("INFO", "before fork, in parent process");

    pid_t pid = fork();

    logger("INFO", "after fork, in both parent and child process");

    if (pid < 0) {
        logger("Error", "in fork: ", errno);
    } else if (pid == 0) {
        logger("INFO", "from child process");
        count += 1;
    } else {
        logger("INFO", "from parent process");
        count += 1;
    }

    logger("INFO", "final count: %d", count);

    return 0;
}
