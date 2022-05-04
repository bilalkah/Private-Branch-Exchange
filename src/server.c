/*
 * "PBX" server module.
 * Manages interaction with a client telephone unit (TU).
 */
#include <stdlib.h>

#include "debug.h"
#include "pbx.h"
#include "server.h"
#include "structures.h"

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

/*
 * Thread function for the thread that handles interaction with a client TU.
 * This is called after a network connection has been made via the main server
 * thread and a new thread has been created to handle the connection.
 */
#if 1
void *pbx_client_service(void *arg)
{
    // TO BE IMPLEMENTED
    int status;
    char buf[1024];

    debug("pbx_client_service()");
    int client_fd = *(int *)arg;
    free(arg);

    debug("client_fd = %d", client_fd);

    TU *tu = tu_init(client_fd);
    assert(tu != NULL);
    tu_set_extension(tu, client_fd);
    status = pbx_register(pbx, tu, client_fd);
    sprintf(buf, "%s %d\n", tu_state_names[tu->state], tu->extension);
    send(client_fd, buf, strlen(buf), 0);
    while (tu->is_connected)
    {
        // listen for input from the client TU
        int n = read(client_fd, buf, sizeof(buf));
        if (n < 0)
        {
            error("read() failed.");
        }
        else if (n == 0)
        {
            debug("read() returned 0, client disconnected.");
            pbx->head = delete_node(pbx->head, tu);
            break;
        }
        else
        {
            char *p = strchr(buf, ' ');
            if (p != NULL)
            {
                debug("Found space in input.");
                char *cmd = strtok(buf, " ");
                if (cmd == NULL)
                {
                    debug("strtok() failed.");
                }
                else
                {
                    debug("cmd = %s", cmd);
                }
                char *args = strtok(NULL, "\n");
                if (args == NULL)
                {
                    debug("strtok() failed.");
                }
                else
                {
                    debug("args = %s", args);
                }

                if (strncmp(cmd, tu_command_names[TU_DIAL_CMD], strlen(tu_command_names[TU_DIAL_CMD])) == 0)
                {
                    debug("TU_DIAL_CMD");
                    int extension = atoi(args);
                    debug("extension = %d", extension);
                    status=pbx_dial(pbx, tu, extension);
                    sprintf(buf, "%s\n", tu_state_names[tu->state]);
                    send(client_fd, buf, strlen(buf), 0);
                }
                else if (strncmp(cmd, tu_command_names[TU_CHAT_CMD], strlen(tu_command_names[TU_CHAT_CMD])) == 0)
                {
                    debug("TU_CHAT_CMD");
                    tu_chat(tu, args);
                    debug("chat sended");
                }
                
            }
            else
            {
                debug("No space in input.");
                if (strncmp(buf, tu_command_names[TU_PICKUP_CMD], strlen(tu_command_names[TU_PICKUP_CMD])) == 0)
                {
                    success("TU_PICKUP_CMD");
                    status = tu_pickup(tu);
                    if (status == 0)
                    {
                        sprintf(buf, "%s\n", tu_state_names[tu->state]);
                        send(client_fd, buf, strlen(buf), 0);
                    }
                }
                else if (strncmp(buf, tu_command_names[TU_HANGUP_CMD], strlen(tu_command_names[TU_HANGUP_CMD])) == 0)
                {
                    success("TU_HANGUP_CMD");
                    status = tu_hangup(tu);
                    if (status == 0)
                    {
                        sprintf(buf, "%s\n", tu_state_names[tu->state]);
                        send(client_fd, buf, strlen(buf), 0);
                    }
                }
                else
                {
                    // unknown command
                    debug("Unknown command.");
                }
            }
        }
    }
    pbx_unregister(pbx, tu);

    return NULL;
}
#endif
