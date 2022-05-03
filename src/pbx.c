/*
 * PBX: simulates a Private Branch Exchange.
 */
#include <stdlib.h>

#include "pbx.h"
#include "debug.h"
#include "structures.h"

#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <stdbool.h>



/*
 * Initialize a new PBX.
 *
 * @return the newly initialized PBX, or NULL if initialization fails.
 */
#if 1
PBX *pbx_init()
{
    // TO BE IMPLEMENTED
    debug("pbx_init()");

    // create a new PBX
    PBX *_pbx = malloc(sizeof(PBX));
    if (_pbx == NULL)
    {
        debug("pbx_init() failed to allocate memory for new_pbx");
        return NULL;
    }

    // initialize the pbx and server socket
    _pbx->num_tus = 0;
    _pbx->head = NULL;

    return _pbx;
}
#endif

/*
 * Shut down a pbx, shutting down all network connections, waiting for all server
 * threads to terminate, and freeing all associated resources.
 * If there are any registered extensions, the associated network connections are
 * shut down, which will cause the server threads to terminate.
 * Once all the server threads have terminated, any remaining resources associated
 * with the PBX are freed.  The PBX object itself is freed, and should not be used again.
 *
 * @param pbx  The PBX to be shut down.
 */
#if 1
void pbx_shutdown(PBX *pbx)
{
    // TO BE IMPLEMENTED
    // abort();
    debug("pbx_shutdown()");
    delete_all_nodes(pbx->head);
    free(pbx);
}
#endif

/*
 * Register a telephone unit with a PBX at a specified extension number.
 * This amounts to "plugging a telephone unit into the PBX".
 * The TU is initialized to the TU_ON_HOOK state.
 * The reference count of the TU is increased and the PBX retains this reference
 *for as long as the TU remains registered.
 * A notification of the assigned extension number is sent to the underlying network
 * client.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU to be registered.
 * @param ext  The extension number on which the TU is to be registered.
 * @return 0 if registration succeeds, otherwise -1.
 */
#if 1
int pbx_register(PBX *pbx, TU *tu, int ext)
{
    // TO BE IMPLEMENTED
    // abort();
    tu_set_extension(tu, ext);
    tu_ref(tu, "pbx_register");
    pbx->head = insert_node(pbx->head, tu);
    return 0;
}
#endif

/*
 * Unregister a TU from a PBX.
 * This amounts to "unplugging a telephone unit from the PBX".
 * The TU is disassociated from its extension number.
 * Then a hangup operation is performed on the TU to cancel any
 * call that might be in progress.
 * Finally, the reference held by the PBX to the TU is released.
 *
 * @param pbx  The PBX.
 * @param tu  The TU to be unregistered.
 * @return 0 if unregistration succeeds, otherwise -1.
 */
#if 1
int pbx_unregister(PBX *pbx, TU *tu)
{
    // TO BE IMPLEMENTED
    // abort();
    pbx->head = delete_node(pbx->head, tu);
    tu_unref(tu, "pbx_unregister");
    return 0;
}
#endif

/*
 * Use the PBX to initiate a call from a specified TU to a specified extension.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU that is initiating the call.
 * @param ext  The extension number to be called.
 * @return 0 if dialing succeeds, otherwise -1.
 */
#if 1
int pbx_dial(PBX *pbx, TU *tu, int ext)
{
    // TO BE IMPLEMENTED
    // abort();
    TU *_tu = find_tu(pbx->head, ext);
    if (_tu == NULL)
    {
        debug("pbx_dial() failed to find extension");
        return -1;
    }
    if(_tu->state != TU_ON_HOOK)
    {
        debug("tu is not on hook");
        return -1;
    }
    if (tu_dial(_tu, tu) == -1)
    {
        debug("tu_dial() failed");
        return -1;
    }
}
#endif
