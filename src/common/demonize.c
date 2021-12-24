#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* demonization */
int demonize(void)
{
    int r;
    pid_t pid, sid;
    int fd;

    /* fork process */
    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "ERROR: 'fork' failed with code %d\n", pid);
        return EXIT_FAILURE;
    };

    /* exiting from parent */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* changing file mode mask */
    umask(0);

    /* new sid */
    if ((sid = setsid()) < 0)
    {
        fprintf(stderr, "ERROR: 'setid' failed with code %d\n", sid);
        return EXIT_FAILURE;
    };

    /* changing working directory */
    if ((r = chdir("/")) < 0)
    {
        fprintf(stderr, "ERROR: Unable to change root directory. "
            "'chdir' failed with code %d\n", r);
        return EXIT_FAILURE;
    };

    /* change std(in/out/err) descriptors */
    fd = open("/dev/null", O_RDWR);
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);

    return 0;
};
