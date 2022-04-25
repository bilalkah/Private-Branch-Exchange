#include <stdlib.h>
#include <unistd.h>

#include "pbx.h"
#include "server.h"
#include "debug.h"
#include "structures.h"

#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

static void terminate(int status);



/*
 * "PBX" telephone exchange simulation.
 *
 * Usage: pbx <port>
 */
int main(int argc, char *argv[])
{
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    int port;
    if (argc < 2)
    {
        error("Usage: pbx <port>.");
        exit(1);
    }
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            if (i + 1 < argc)
            {
                port = atoi(argv[i + 1]);
            }
            else
            {
                error("Option -p requires an argument.");
                exit(EXIT_FAILURE);
            }
        }
    }
    success("PBX starting on port %d.", port);
    
    // install SIGHUB handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = terminate;
    struct sigaction old_sa;
    sigaction(SIGINT, &sa, &old_sa);
    sigaction(SIGHUP, &sa, &old_sa);
    success("SIGHUP handler installed.");


    // Perform required initialization of the PBX module.
    debug("Initializing PBX...");

    // Define PBX structure
    pbx = pbx_init();
    if (pbx == NULL)
    {
        error("pbx_init failed to allocate memory for _pbx.");
        terminate(EXIT_FAILURE);
    }

    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function pbx_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.

    // Setup server socket
    pbx->serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (pbx->serverfd < 0)
    {
        debug("pbx_init() failed to create server socket.");
        terminate(EXIT_FAILURE);
    }
    else
    {
        success("pbx_init() created server socket.");
    }
    memset(&pbx->serv_addr, 0, sizeof(pbx->serv_addr));
    pbx->serv_addr.sin_family = AF_INET;
    pbx->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    pbx->serv_addr.sin_port = htons(port);

    if (bind(pbx->serverfd, (struct sockaddr *)&(pbx->serv_addr), sizeof(pbx->serv_addr)) < 0)
    {
        error("pbx_init() failed to bind server socket.");
        terminate(EXIT_FAILURE);
    }
    else
    {
        success("pbx_init() bound server socket.");
    }

    // Listen for connections on the server socket and accept them as needed in a loop
    while (1)
    {
        if (listen(pbx->serverfd, 5) < 0)
        {
            error("listen() failed.");
            terminate(EXIT_FAILURE);
        }
        else
        {
            success("listen() successful.");
            int client_fd;
            socklen_t client_len;
            struct sockaddr_in client_addr;
            client_len = sizeof(client_addr);
            client_fd = accept(pbx->serverfd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0)
            {
                perror("accept() failed.");
                terminate(EXIT_FAILURE);
            }
            else
            {
                success("accept() successful.");
                // Create a new thread to handle the connection
                int *client_fd_ptr = malloc(sizeof(int));
                assert(client_fd_ptr != NULL);
                *client_fd_ptr = client_fd;
                pthread_t thread;
                pthread_attr_t attr;
                int err;
                err = pthread_attr_init(&attr);
                assert(err == 0);
                err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                assert(err == 0);
                err = pthread_create(&thread, &attr, pbx_client_service, (void *)client_fd_ptr);
                assert(err == 0);
            }
        }
    }


    fprintf(stderr, "You have to finish implementing main() "
                    "before the PBX server will function.\n");

    terminate(EXIT_FAILURE);
}

/*
 * Function called to cleanly shut down the server.
 */
static void terminate(int status)
{
    debug("Shutting down PBX...");
    pbx_shutdown(pbx);
    debug("PBX server terminating");
    exit(status);
}
