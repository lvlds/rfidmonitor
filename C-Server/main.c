#include <pthread.h>
#include <stdio.h>

// Kenner
// Augusto

/* for POSIX threads */

void * ThreadMain (void *arg);

/* Main program of a thread */
struct ThreadArgs
{
    /* Structure of arguments to pass to client thread */
    int clntSock;
    /* socket descriptor for client */
};

int main(int argc, char *argv[])
{
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    unsigned short echoServPort; /* Server port */
    pthread_t threadID; /* Thread ID from pthread_create()*/
    struct ThreadArgs *threadArgs; /* Pointer to argument structure for thread */
    if (argc != 2) { /* Test for correct number of arguments */
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }
    echoServPort = atoi(argv[1]); /* First arg: local port */
    servSock = CreateTCPServerSocket(echoServPort);
    for (;;) { /* Run forever */
        clntSock = AcceptTCPConnection(servSock);
        /* Create separate memory for client argument */
        if ((threadArgs = ( struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
            DieWithError("…");
        threadArgs-> clntSock = clntSock;
        /* Create client thread */
        if (pthread_create (&threadID, NULL, ThreadMain, (void *)threadArgs) != 0)
            DieWithError("…");
    }
    /* NOT REACHED */
}

void * ThreadMain(void *threadArgs)
{
    int clntSock; /* Socket descriptor for client connection */
    pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */
    /* Extract socket file descriptor from argument */
    clntSock = ((struct ThreadArgs *)threadArgs) -> clntSock;
    free(threadArgs); /* Deallocate memory for argument */
    HandleTCPClient(clntSock);
    return (NULL);
}
