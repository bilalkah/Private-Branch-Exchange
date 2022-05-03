#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "pbx.h"
#include "tu.h"

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <stdbool.h>


typedef struct tu_clients
{
    TU *tu_client;
    struct tu_clients *next;
}tu_clients;

struct pbx
{
    int num_tus;
    int serverfd;
    struct sockaddr_in serv_addr;
    tu_clients *head;
};


struct tu
{
    int client_fd;
    int file_descriptor;
    int state;
    int extension;
    int ref_count;
    bool is_connected;
    TU *peer;
};

tu_clients *create_node(TU *tu_client);
tu_clients *insert_node(tu_clients *head, TU *tu_client);
tu_clients *delete_node(tu_clients *head, TU *tu_client);
tu_clients *reverse_list(tu_clients *head);
TU *find_tu(tu_clients *head, int extension);
void delete_all_nodes(tu_clients *head);

#endif