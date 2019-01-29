/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* pending_sig_expt.c

   Write a program, pending_sig_expt.c, that:

   * Establishes a handler for SIGINT that does nothing but return
   * Blocks all signals except SIGINT (sigprocmask())
   * Calls pause() to wait for a signal
   * After pause() returns, retrieves the list of pending signals for
     the process (sigpending()) and prints them (strsignal())

   Run the program and send it various signals (other than SIGINT)
   using the kill command. (Send signals whose default disposition is
   not "ignore".) Then type Control-C to generate SIGINT and inspect
   the list of pending signals.

   Extend the program created in the preceding exercise so that:
   * Just after installing the handler for SIGINT, it installs an
     additional handler for the SIGQUIT signal (generated when the
     Control-\ key is pressed). The handler should print a message
     "SIGQUIT received", and return.
   * After displaying the list of pending signals, unblock SIGQUIT
     and call pause() once more.

   While the program is blocking signals (i.e., before typing
   Control-C) try typing Control-\ multiple times. After Control-C
   is typed, how many times does the SIGQUIT handler display its
   message? Why?
*/
#define _GNU_SOURCE
#include <signal.h>
#include "tlpi_hdr.h"

/* FIXME: Implement handlers for SIGINT and SIGQUIT; the latter handler
   should display a message that SIGQUIT has been caught */

static void block_every_sig_except_int()
{
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

static void unblock_sigquit()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

static void handle_int(int _)
{
    return;
}

static void handle_quit(int _)
{
    printf("SIGQUIT received\n"); /* UNSAFE?? */
}

static void print_all_pending_sigs()
{
    sigset_t mask;
    int ret = sigpending(&mask);
    if (ret != 0)
        errExit("sigpending");
    for (int i = 1; i < NSIG; ++i)
    {
        int yolo = sigismember(&mask, i);
        switch (yolo)
        {
            case 0:
                continue;
            case 1:
                break;
            default:
                errExit("sigismember returns %d for signal %d", yolo, i);
        }
        printf("pending signal: %s\n", strsignal(i));
    }
}

static void setup_handler_for_int_and_quit()
{
    struct sigaction s = {
            .sa_flags = 0,
            .sa_handler = handle_int,
    };
    sigemptyset(&s.sa_mask);

    sigaction(SIGINT, &s, NULL);

    s.sa_handler = handle_quit;
    sigaction(SIGQUIT, &s, NULL);
}

int
main(int argc, char *argv[])
{

    /* FIXME: Add variable declarations as required */

    block_every_sig_except_int();

    /* FIXME: Set up handlers for SIGINT and SIGQUIT */
    setup_handler_for_int_and_quit();

    /* Block until SIGINT handler is invoked */

    printf("Pausing... send me some signals (PID=%ld)\n", (long) getpid());
    pause();
    print_all_pending_sigs();
    unblock_sigquit();

    pause();

    exit(EXIT_SUCCESS);
}
