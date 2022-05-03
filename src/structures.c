#include "structures.h"

tu_clients *create_node(TU *tu_client)
{
    tu_clients *new_node = (tu_clients *)malloc(sizeof(tu_clients));
    new_node->tu_client = tu_client;
    new_node->next = NULL;
    return new_node;
}

tu_clients *insert_node(tu_clients *head, TU *tu_client)
{
    tu_clients *new_node = create_node(tu_client);
    if (head == NULL)
    {
        return new_node;
    }
    tu_clients *temp = head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = new_node;
    return head;
}

tu_clients *delete_node(tu_clients *head, TU *tu_client)
{
    if (head == NULL)
    {
        return NULL;
    }
    tu_clients *temp = head;
    if (temp->tu_client == tu_client)
    {
        head = temp->next;
        free(temp);
        return head;
    }
    while (temp->next != NULL)
    {
        if (temp->next->tu_client == tu_client)
        {
            tu_clients *temp2 = temp->next;
            temp->next = temp->next->next;
            temp2->tu_client->is_connected = false;
            tu_unref(temp2->tu_client, "delete_node");
            free(temp2);
            return head;
        }
        temp = temp->next;
    }
    return head;
}

TU *find_tu(tu_clients *head, int extension)
{
    if (head == NULL)
    {
        return NULL;
    }
    tu_clients *temp = head;
    while (temp != NULL)
    {
        if (temp->tu_client->extension == extension)
        {
            return temp->tu_client;
        }
        temp = temp->next;
    }
    return NULL;
}

void delete_all_nodes(tu_clients *head)
{
    if (head == NULL)
    {
        return;
    }
    tu_clients *temp = head;
    while (temp != NULL)
    {
        tu_clients *temp2 = temp;
        temp = temp->next;
        temp2->tu_client->is_connected = false;
        tu_unref(temp2->tu_client, NULL);
        free(temp2);
    }
}

tu_clients *reverse_list(tu_clients *head)
{
    if (head == NULL)
    {
        return NULL;
    }
    tu_clients *prev = NULL;
    tu_clients *current = head;
    tu_clients *next = NULL;
    while (current != NULL)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    return prev;
}
