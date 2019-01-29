/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* setuid_expt.c

   Enable set-user-ID permission on the executable version of this
   program using the command:

       chmod u+s file
*/
#define _GNU_SOURCE
#include <unistd.h>
#include "tlpi_hdr.h"

static void
displayIds(void)
{
    uid_t ruid, euid, suid;

    if (getresuid(&ruid, &euid, &suid) == -1)
        errExit("getresuid");

    printf("rUID = %ld, eUID = %ld, sUID = %ld\n",
            (long) ruid,  (long) euid,  (long) suid);
}

static void temporarily_drop_privs()
{
    uid_t uid = getuid();
    int ret = seteuid(uid);
    if (ret != 0)
        errExit("seteuid");
}

static void regain_priv()
{
    uid_t ruid, euid, suid;
    int r = getresuid(&ruid, &euid, &suid);

    if (r != 0)
        errExit("getresuid");
    r = seteuid(suid);
    if (r != 0)
        errExit("seteuid");
}

static void permanently_drop_privs()
{
    uid_t uid = getuid();

    int r = setresuid(-1, uid, uid);
    if (r != 0)
        errExit("setresuid");
}

int
main(int argc, char *argv[])
{
    uid_t euid;

    displayIds();

    euid = geteuid();           /* Obtain copy of privileged UID */

    temporarily_drop_privs();

    displayIds();

    regain_priv();
    displayIds();

    permanently_drop_privs();
    displayIds();

    int r = seteuid(0);
    if (r == 0)
    {
        printf("seteuid succeeded\n");
    }
    else
    {
        printf("seteuid failed\n");
    }
    displayIds();
    exit(EXIT_SUCCESS);
}
