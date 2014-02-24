#include <stdio.h>

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void DieWithError(char *errorMessage);
void InterruptSignalHandler(int signalType);

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    struct sigaction handler; /* Signal handler specification structure */
    handler.sa_handler = &InterruptSignalHandler;
    /* Set handler function */
    if (sigfillset(&handler.sa_mask) < 0)
        /* Create mask that masks all signals */
        DieWithError ("sigfillset() failed");
    handler.sa_flags = 0;
    if (sigaction(SIGINT, &handler, 0) < 0)
        /* Set signal handling for interrupt signals */
        DieWithError ("sigaction() failed");
    for(;;) pause();
    /* Suspend program until signal received */
    exit(0);
}

void DieWithError(char *errorMessage)
{
    printf("ERROR -> %s", errorMessage);
}

void InterruptSignalHandler(int signalType)
{
    printf ("Interrupt received (%d). Exiting program.\n", signalType);
    exit(1);
}
