/*
 * TU: simulates a "telephone unit", which interfaces a client with the PBX.
 */
#include <stdlib.h>

#include "pbx.h"
#include "debug.h"
#include "structures.h"

/*
 * Initialize a TU
 *
 * @param fd  The file descriptor of the underlying network connection.
 * @return  The TU, newly initialized and in the TU_ON_HOOK state, if initialization
 * was successful, otherwise NULL.
 */
#if 1
TU *tu_init(int fd)
{
    TU *_tu = malloc(sizeof(TU));
    if (_tu == NULL)
    {
        debug("tu_init() failed to allocate memory for new_tu");
        return NULL;
    }
    _tu->file_descriptor = fd;
    _tu->client_fd = fd;
    _tu->peer = NULL;
    _tu->state = TU_ON_HOOK;
    _tu->ref_count = 0;
    // _tu->signaling_lock = (pthread_mutex_t)PTHREAD_COND_INITIALIZER;
    // _tu->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    tu_ref(_tu, "tu_init");
    return _tu;
    // TO BE IMPLEMENTED
    // abort();
}
#endif

/*
 * Increment the reference count on a TU.
 *
 * @param tu  The TU whose reference count is to be incremented
 * @param reason  A string describing the reason why the count is being incremented
 * (for debugging purposes).
 */
#if 1
void tu_ref(TU *tu, char *reason)
{
    if (tu == NULL)
    {
        debug("tu_ref() called with NULL tu");
        return;
    }
    debug("tu_ref(%s)", reason);
    tu->ref_count++;
    debug("ref of tu is now %d", tu->ref_count);
}
#endif

/*
 * Decrement the reference count on a TU, freeing it if the count becomes 0.
 *
 * @param tu  The TU whose reference count is to be decremented
 * @param reason  A string describing the reason why the count is being decremented
 * (for debugging purposes).
 */
#if 1
void tu_unref(TU *tu, char *reason)
{
    if (tu == NULL)
    {
        debug("tu_unref(%s) called with NULL tu", reason);
        return;
    }
    
    debug("tu_unref(%s)", reason);
    tu->ref_count--;
    debug("ref of tu is now %d", tu->ref_count);
    if (tu->ref_count == 0)
    {
        free(tu);
    }
}
#endif

/*
 * Get the file descriptor for the network connection underlying a TU.
 * This file descriptor should only be used by a server to read input from
 * the connection.  Output to the connection must only be performed within
 * the PBX functions.
 *
 * @param tu
 * @return the underlying file descriptor, if any, otherwise -1.
 */
#if 1
int tu_fileno(TU *tu)
{
    if (tu == NULL)
    {
        debug("tu_fileno() called with NULL tu");
        return -1;
    }
    return tu->file_descriptor;
}
#endif

/*
 * Get the extension number for a TU.
 * This extension number is assigned by the PBX when a TU is registered
 * and it is used to identify a particular TU in calls to tu_dial().
 * The value returned might be the same as the value returned by tu_fileno(),
 * but is not necessarily so.
 *
 * @param tu
 * @return the extension number, if any, otherwise -1.
 */
#if 1
int tu_extension(TU *tu)
{
    if (tu == NULL)
    {
        return -1;
    }
    return tu->extension;
}
#endif

/*
 * Set the extension number for a TU.
 * A notification is set to the client of the TU.
 * This function should be called at most once one any particular TU.
 *
 * @param tu  The TU whose extension is being set.
 */
#if 1
int tu_set_extension(TU *tu, int ext)
{
    tu->extension = ext;
    return 0;
}
#endif

/*
 * Initiate a call from a specified originating TU to a specified target TU.
 *   If the originating TU is not in the TU_DIAL_TONE state, then there is no effect.
 *   If the target TU is the same as the originating TU, then the TU transitions
 *     to the TU_BUSY_SIGNAL state.
 *   If the target TU already has a peer, or the target TU is not in the TU_ON_HOOK
 *     state, then the originating TU transitions to the TU_BUSY_SIGNAL state.
 *   Otherwise, the originating TU and the target TU are recorded as peers of each other
 *     (this causes the reference count of each of them to be incremented),
 *     the target TU transitions to the TU_RINGING state, and the originating TU
 *     transitions to the TU_RING_BACK state.
 *
 * In all cases, a notification of the resulting state of the originating TU is sent to
 * to the associated network client.  If the target TU has changed state, then its client
 * is also notified of its new state.
 *
 * If the caller of this function was unable to determine a target TU to be called,
 * it will pass NULL as the target TU.  In this case, the originating TU will transition
 * to the TU_ERROR state if it was in the TU_DIAL_TONE state, and there will be no
 * effect otherwise.  This situation is handled here, rather than in the caller,
 * because here we have knowledge of the current TU state and we do not want to introduce
 * the possibility of transitions to a TU_ERROR state from arbitrary other states,
 * especially in states where there could be a peer TU that would have to be dealt with.
 *
 * @param tu  The originating TU.
 * @param target  The target TU, or NULL if the caller of this function was unable to
 * identify a TU to be dialed.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
#if 1
int tu_dial(TU *tu, TU *target)
{
    pthread_mutex_lock(&target->signaling_lock);
    debug("%s",tu_state_names[tu->state]);
    char buf[256];
    if (tu->state != TU_DIAL_TONE)
    {
        debug("tu_dial() called on TU in state %s", tu_state_names[tu->state]);
        sprintf(buf, "%s%s",tu_state_names[tu->state],EOL);
        send(tu->file_descriptor, buf, strlen(buf), 0);
        pthread_mutex_unlock(&target->signaling_lock);
        return -1;
    }
    if (target == NULL)
    {
        debug("tu_dial() called with NULL target");
        tu->state = TU_ERROR;
        sprintf(buf, "%s%s",tu_state_names[tu->state],EOL);
        send(tu->file_descriptor, buf, strlen(buf), 0);
        pthread_mutex_unlock(&target->signaling_lock);
        return -1;
    }
    if (target->state != TU_ON_HOOK || target->peer != NULL)
    {
        debug("tu_dial() called with target TU in state %d", target->state);
        tu->state = TU_BUSY_SIGNAL;
        sprintf(buf, "%s%s",tu_state_names[tu->state],EOL);
        send(tu->file_descriptor, buf, strlen(buf), 0);
        pthread_mutex_unlock(&target->signaling_lock);
        return -1;
    }

    tu->state = TU_RING_BACK;
    target->state = TU_RINGING;
    sprintf(buf, "%s\n", tu_state_names[target->state]);
    send(target->client_fd, buf, strlen(buf), 0);
    sprintf(buf, "%s\n", tu_state_names[tu->state]);
    send(tu->client_fd, buf, strlen(buf), 0);
    
    debug("tu_dial() called, tu->state = %s, target->state = %s",
          tu_state_names[tu->state], tu_state_names[target->state]);

    pthread_cond_wait(&target->cond, &target->signaling_lock);
    
    tu->peer = target;
    target->peer = tu;
    tu_ref(target, "tu_dial");
    tu_ref(tu, "tu_dial");
    tu->state = TU_CONNECTED;
    sprintf(buf, "%s\n", tu_state_names[tu->state]);
    send(tu->client_fd, buf, strlen(buf), 0);
    success("tu_dial() called, tu->state = %s, target->state = %s",
          tu_state_names[tu->state], tu_state_names[target->state]);

    pthread_mutex_unlock(&target->signaling_lock);
    return 0;
}
#endif

/*
 * Take a TU receiver off-hook (i.e. pick up the handset).
 *   If the TU is in neither the TU_ON_HOOK state nor the TU_RINGING state,
 *     then there is no effect.
 *   If the TU is in the TU_ON_HOOK state, it goes to the TU_DIAL_TONE state.
 *   If the TU was in the TU_RINGING state, it goes to the TU_CONNECTED state,
 *     reflecting an answered call.  In this case, the calling TU simultaneously
 *     also transitions to the TU_CONNECTED state.
 *
 * In all cases, a notification of the resulting state of the specified TU is sent to
 * to the associated network client.  If a peer TU has changed state, then its client
 * is also notified of its new state.
 *
 * @param tu  The TU that is to be picked up.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
#if 1
int tu_pickup(TU *tu)
{
    if (!tu || tu->state == TU_CONNECTED)
    {
        return -1;
    }
    char buf[256];
    switch (tu->state)
    {
    case TU_ON_HOOK:
        tu->state = TU_DIAL_TONE;
        debug("tu_pickup: TU %d is now in state %s", tu->extension, tu_state_names[tu->state]);
        break;
    case TU_RINGING:
        tu->state = TU_CONNECTED;
        pthread_cond_signal(&tu->cond);
        break;
    default:
        break;
    }
    sprintf(buf, "%s%s", tu_state_names[tu->state],EOL);
    send(tu->client_fd, buf, strlen(buf), 0);
    return 0;
}
#endif

/*
 * Hang up a TU (i.e. replace the handset on the switchhook).
 *
 *   If the TU is in the TU_CONNECTED or TU_RINGING state, then it goes to the
 *     TU_ON_HOOK state.  In addition, in this case the peer TU (the one to which
 *     the call is currently connected) simultaneously transitions to the TU_DIAL_TONE
 *     state.
 *   If the TU was in the TU_RING_BACK state, then it goes to the TU_ON_HOOK state.
 *     In addition, in this case the calling TU (which is in the TU_RINGING state)
 *     simultaneously transitions to the TU_ON_HOOK state.
 *   If the TU was in the TU_DIAL_TONE, TU_BUSY_SIGNAL, or TU_ERROR state,
 *     then it goes to the TU_ON_HOOK state.
 *
 * In all cases, a notification of the resulting state of the specified TU is sent to
 * to the associated network client.  If a peer TU has changed state, then its client
 * is also notified of its new state.
 *
 * @param tu  The tu that is to be hung up.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
#if 1
int tu_hangup(TU *tu)
{
    if (!tu)
    {
        return -1;
    }
    char buf[256];
    switch (tu->state)
    {
    case TU_CONNECTED:
        tu->state = TU_ON_HOOK;
        tu->peer->state = TU_DIAL_TONE;
        sprintf(buf, "%s\n", tu_state_names[tu->peer->state]);
        int tu_fd = tu->peer->client_fd;
        tu_unref(tu->peer, "tu_hangup");
        tu->peer = NULL;
        tu_unref(tu, "tu_hangup");
        tu->peer = NULL;
        send(tu_fd, buf, strlen(buf), 0);
        break;
    case TU_RINGING:
        tu->state = TU_ON_HOOK;
        break;
    case TU_RING_BACK:
        tu->state = TU_ON_HOOK;
        break;
    case TU_DIAL_TONE:
    case TU_BUSY_SIGNAL:
    case TU_ERROR:
        tu->state = TU_ON_HOOK;
        break;
    default:
        break;
    }
    sprintf(buf, "%s\n", tu_state_names[tu->state]);
    send(tu->client_fd, buf, strlen(buf), 0);
    return 0;
}
#endif

/*
 * "Chat" over a connection.
 *
 * If the state of the TU is not TU_CONNECTED, then nothing is sent and -1 is returned.
 * Otherwise, the specified message is sent via the network connection to the peer TU.
 * In all cases, the states of the TUs are left unchanged and a notification containing
 * the current state is sent to the TU sending the chat.
 *
 * @param tu  The tu sending the chat.
 * @param msg  The message to be sent.
 * @return 0  If the chat was successfully sent, -1 if there is no call in progress
 * or some other error occurs.
 */
#if 1
int tu_chat(TU *tu, char *msg)
{
    if (!tu || !msg)
    {
        debug("tu_chat() called with NULL tu or msg");
        return -1;
    }
    if (tu->state != TU_CONNECTED)
    {
        debug("tu_chat() called with tu->state = %s", tu_state_names[tu->state]);
        return -1;
    }
    if (tu->peer->state != TU_CONNECTED)
    {
        debug("tu_chat() called with tu->peer->state = %s", tu_state_names[tu->peer->state]);
        return -1;
    }
    // add EOL to message
    strcat(msg, EOL);
    if (send(tu->peer->file_descriptor, msg, strlen(msg), 0) < 0)
    {
        debug("tu_chat: send() failed");
        return -1;
    }
    debug("tu_chat: sent message %s", msg);

    return 0;
}
#endif
