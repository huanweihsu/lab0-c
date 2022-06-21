#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    element_t *q = malloc(sizeof(element_t));
    if (q != NULL) {
        q->value = NULL;
        q->list.next = &q->list;
        q->list.prev = &q->list;

        return &q->list;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l || l == NULL) {
        return;
    }
    struct list_head *n = l->next;
    while (n != l) {
        element_t *node = container_of(n, element_t, list);
        n = n->next;
        q_release_element(node);
    }
    element_t *node = container_of(l, element_t, list);
    free(node);
    // struct list_head **ptr = &l;
    // while ((*ptr)->next != *ptr)
    // {
    //     element_t *node = container_of(*ptr, element_t, list);
    //     ptr = &(*ptr)->next;
    //     q_release_element(node);
    // }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *q = malloc(sizeof(element_t));
    // Return NULL if malloc fail or empty queue
    if (!q) {
        return false;
    }
    q->value = strdup(s);
    if (!q->value) {
        free(q);
        return false;
    }
    list_add(&q->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *q = malloc(sizeof(element_t));
    if (!q) {
        return false;
    }
    q->value = strdup(s);
    // strdup = malloc + strcpy
    // Check malloc of q->value
    if (!q->value) {
        free(q);
        return false;
    }
    list_add_tail(&q->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node = list_first_entry(head, element_t, list);
    if (sp != NULL) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node = container_of(head->prev, element_t, list);
    if (sp != NULL) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || head->next == head) {
        return false;
    }
    int del_idx = (q_size(head) + 1) / 2;
    int count = 0;
    struct list_head *n = head;
    while (count != del_idx) {
        n = n->next;
        count++;
    }
    list_del(n);
    q_release_element(container_of(n, element_t, list));
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || head->next == head) {
        return false;
    }
    struct list_head *node = head->next;
    while (node->next != head) {
        if (strcmp(list_entry(node, element_t, list)->value,
                   list_entry(node->next, element_t, list)->value) == 0) {
            element_t *n = list_entry(node->next, element_t, list);
            list_del(node->next);
            q_release_element(n);
            if (node->next != head) {
                if (strcmp(list_entry(node, element_t, list)->value,
                           list_entry(node->next, element_t, list)->value) !=
                    0) {
                    n = list_entry(node, element_t, list);
                    struct list_head *n_next = node->next;
                    list_del(node);
                    q_release_element(n);
                    node = n_next;
                }
            } else {
                n = list_entry(node, element_t, list);
                struct list_head *n_prev = node->prev;
                list_del(node);
                q_release_element(n);
                node = n_prev;
            }
        } else {
            node = node->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (list_empty(head)) {
        return;
    }
    struct list_head *node = head->next;
    struct list_head *n_next, *n_prev;
    while (node != head && node->next != head) {
        n_next = node->next;
        n_prev = node->prev;

        // list_del(n_next);
        // list_add(n_next, n_prev);

        node->next = n_next->next;
        node->prev = n_next;
        node->next->prev = node;

        n_next->prev = n_prev;
        n_next->next = node;

        n_prev->next = n_next;

        node = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    // Reverse list
    struct list_head *n = head->prev;
    struct list_head *n_prev;
    while (n != head) {
        n_prev = n->prev;
        n->prev = n->next;
        n->next = n_prev;
        n = n->next;
    }
    // Reverse head
    n_prev = head->prev;
    head->prev = head->next;
    head->next = n_prev;
}

struct list_head *mergesort(struct list_head *lnode, struct list_head *rnode)
{
    if (lnode == rnode) {
        return lnode;
    }
    struct list_head *slow = lnode;
    for (struct list_head *fast = slow; fast->next && fast->next->next;
         fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = mergesort(lnode, slow);
    struct list_head *right = mergesort(mid, rnode);

    // merge 2 sorted list
    struct list_head *ms_head = lnode, **ptr = &ms_head;

    while (left && right) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) < 0) {
            *ptr = left;
            left = left->next;
        } else {
            *ptr = right;
            right = right->next;
        }
        ptr = &(*ptr)->next;
    }
    *ptr = left ? left : right;

    return ms_head;

}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    if (head->next == head->prev) {
        return;
    }
    struct list_head *start_node = head->next;
    struct list_head *end_node = head->prev;
    end_node->next = NULL;

    head->next = mergesort(start_node, end_node);

    for (start_node = head; start_node->next != NULL;
         start_node = start_node->next) {
        start_node->next->prev = start_node;
    }
    start_node->next = head;
    head->prev = start_node;
}

// /* Swap two nodes */
// void swap_node(struct list_head *n1, struct list_head *n2)
// {
//     struct list_head *n1_prev = n1->prev;
//     if (n1->next != n2) {
//         list_del(n1);
//         list_add(n1, n2);
//         list_del(n2);
//         list_add(n2, n1_prev);
//     } else {
//         list_del(n1);
//         list_add(n1, n2);
//     }
// }

void swap_element_value(element_t *a, element_t *b)
{
    char *tmp = a->value;
    a->value = b->value;
    b->value = tmp;
}

void quicksort(struct list_head *node_l, struct list_head *node_r)
{
    if ((node_l != node_r) && (node_r->next != node_l)) {
        char *pivot = list_entry(node_r, element_t, list)->value;
        struct list_head *idx = node_l->prev;
        for (struct list_head *node = node_l; node != node_r;
             node = node->next) {
            if (strcmp(list_entry(node, element_t, list)->value, pivot) <= 0) {
                idx = idx->next;
                // swap_node(idx, node);
                swap_element_value(list_entry(idx, element_t, list),
                                   list_entry(node, element_t, list));
            }
        }
        idx = idx->next;
        // swap_node(idx, node_r);
        swap_element_value(list_entry(idx, element_t, list),
                           list_entry(node_r, element_t, list));

        quicksort(node_l, idx->prev);
        quicksort(idx->next, node_r);
    }
}