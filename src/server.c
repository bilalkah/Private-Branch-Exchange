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
    debug("pbx_client_service()");
    int client_fd = *(int *)arg;
    free(arg);

    debug("client_fd = %d", client_fd);

    TU *tu = tu_init(client_fd);
    assert(tu != NULL);
    tu_set_extension(tu, client_fd);
    int status = pbx_register(pbx, tu, client_fd);
    // if (status == 0)
    // {
    //     tu->state = TU_ON_HOOK;
    // }

    // Enter a service loop to handle interactions with the client TU.
    // This loop will terminate when the client disconnects.
    // Listen for input from the client TU and process it.

    while (tu->is_connected)
    {
        // listen for input from the client TU
        char buf[1024];
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
            // check if there any spaces in the input
            char *p = strchr(buf, ' ');
            if (p != NULL)
            {
                // there is a space in the input
                // terminate the string at the space
                debug("Found space in input.");
                // get string up to the first space
                char *cmd = strtok(buf, " ");
                if (cmd == NULL)
                {
                    debug("strtok() failed.");
                }
                else
                {
                    debug("cmd = %s", cmd);
                }

                // get the rest of the string until newline
                char *args = strtok(NULL, "\n");
                if (args == NULL)
                {
                    debug("strtok() failed.");
                }
                else
                {
                    debug("args = %s", args);
                }

                // compare command to known commands
                // tu_command_names[]
                if (strncmp(cmd, tu_command_names[TU_DIAL_CMD], strlen(tu_command_names[TU_DIAL_CMD])) == 0)
                {
                    debug("TU_DIAL_CMD");
                }
                else if (strncmp(cmd, tu_command_names[TU_CHAT_CMD], strlen(tu_command_names[TU_CHAT_CMD])) == 0)
                {
                    debug("TU_CHAT_CMD");
                }
                else
                {
                    debug("Unknown command");
                }
            }
            else
            {
                // there is no space in the input
                // terminate the string at the end of the input
                debug("No space in input.");

                // compare command to known commands
                // tu_command_names[]
                // debug("command %s.",tu_command_names[TU_PICKUP_CMD]);

                if (strncmp(buf, tu_command_names[TU_PICKUP_CMD], strlen(tu_command_names[TU_PICKUP_CMD])) == 0)
                {
                    success("TU_PICKUP_CMD");
                    // status = pbx_pickup(pbx, tu);
                    assert(status == 0);
                }
                else if (strncmp(buf, tu_command_names[TU_HANGUP_CMD], strlen(tu_command_names[TU_HANGUP_CMD])) == 0)
                {
                    success("TU_HANGUP_CMD");
                    // status = pbx_hangup(pbx, tu);
                    assert(status == 0);
                }
                else
                {
                    // unknown command
                    debug("Unknown command.");
                }

            }
        }
    }
    tu_unref(tu,"pbx_client_service end");

    return NULL;
}
#endif
