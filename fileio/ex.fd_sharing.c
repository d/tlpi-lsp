/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* fd_sharing.c

   Demonstrate that duplicate file descriptors share the file
   offset and file status flags by writing a program that:

   * Opens a file and duplicates the resulting file descriptor, to
     create a second file descriptor.
   * Displays the file offset and the state of the O_APPEND file
     status flag via the first file descriptor.
   * Changes the file offset and setting of the O_APPEND file status
     flag via the first file descriptor.
   * Displays the file offset and the state of the O_APPEND file
     status flag via the second file descriptor.
*/
#include <fcntl.h>
#include "tlpi_hdr.h"
#include <sys/syscall.h>
#include <linux/kcmp.h>

static int kcmp(pid_t pid1, pid_t pid2, int type, unsigned long idx1, unsigned long idx2)
{
    return syscall(SYS_kcmp, pid1, pid2, type, idx1, idx2);
}

static void
printFileDescriptionInfo(int fd)
{

    /* FIXME: Display file offset and O_APPEND setting for 'fd' */

}

int
main(int argc, char *argv[])
{
    int fd1, fd2, flags;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    fd1 = open(argv[1], O_RDWR);
    if (fd1 == -1)
        errExit("opening file %s", argv[1]);

    fd2 = dup(fd1);
    if (fd2 == -1)
        errExit("duping fd1 into fd2");

    flags = fcntl(fd1, F_GETFL);
    off_t offset = lseek(fd1, 0, SEEK_CUR);
    if (offset == -1)
        errExit("lseek");

    printf("fd1: offset: %zd, append bit: %x\n", (ssize_t) offset, (flags & O_APPEND));

    flags |= O_APPEND;
    fcntl(fd2, F_SETFL, flags);
    offset = lseek(fd2, 1, SEEK_CUR);
    if (offset == -1) {
        errExit("lseek");
    }
    printf("fd2: offset: %zd, append bit: %x\n", (ssize_t) offset, (flags & O_APPEND));
    offset = lseek(fd1, 0, SEEK_CUR);

    printf("fd1: offset: %zd\n", (ssize_t) offset);

    pid_t pid = getpid();
    if (kcmp(pid, pid, KCMP_FILE, fd1, fd2) == 0)
    {
        printf("fd1 refers to the same OFD as fd2\n");
    }
    else
    {
        printf("fd1 does not refer to the same OFD as fd2\n");
    }

    /* FIXME: Complete as described in comments above */

    exit(EXIT_SUCCESS);
}
