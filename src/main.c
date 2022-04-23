#include <stdlib.h>
#include <unistd.h>

#include "pbx.h"
#include "server.h"
#include "debug.h"

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
                fprintf(stderr, "Option -p requires an argument.\n");
                exit(1);
            }
        }
    }
    printf("PBX starting on port %d\n", port);

    // install SIGHUB handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = terminate;
    struct sigaction old_sa;
    sigaction(SIGINT, &sa, &old_sa);
    sigaction(SIGHUP, &sa, &old_sa);
    printf("SIGHUP handler installed\n");

    // Perform required initialization of the PBX module.
    debug("Initializing PBX...");
    pbx = pbx_init();

    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function pbx_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.

    // Setup server socket
    struct sockaddr_in server_addr, client_addr;
    int server_fd, client_fd;
    socklen_t client_len;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        terminate(1);
    }
    else
    {
        fprintf(stderr, "socket() successful\n");
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        terminate(1);
    }
    else
    {
        fprintf(stderr, "bind() successful\n");
    }

    // Listen for connections on the server socket and accept them as needed in a loop
    while (1)
    {
        if (listen(server_fd, 5) < 0)
        {
            perror("listen");
            terminate(1);
        }
        else
        {
            fprintf(stderr, "listen() successful\n");
            client_len = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd < 0)
            {
                perror("accept");
                terminate(1);
            }
            else
            {
                fprintf(stderr, "accept() successful\n");
                // Create a new thread to handle the connection
                pthread_t thread;
                pthread_attr_t attr;
                int err;
                err = pthread_attr_init(&attr);
                assert(err == 0);
                err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                assert(err == 0);
                err = pthread_create(&thread, &attr, pbx_client_service, (void *)&client_fd);
                assert(err == 0);
            }
        }
    }

    // while (1)
    // {
    //     int client_fd = server_accept(port);
    //     if (client_fd < 0)
    //     {
    //         fprintf(stderr, "Error accepting client connection\n");
    //         exit(1);
    //     }
    //     pthread_t thread;
    //     pthread_create(&thread, NULL, pbx_client_service, (void *)client_fd);
    // }

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
