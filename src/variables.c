#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variables.h"


/* Create and return a new Node with the supplied values. */
Node *create_node(char* variable, char* value, Node *next) {
    Node *new_node = malloc(sizeof(Node));
    char new_variable[64], new_value[64];
    strcpy(new_variable, variable);
    strcpy(new_value, value);
    new_variable[strlen(variable)] = '\0';
    new_value[strlen(value)] = '\0';
    // printf("new: %s %s", new_variable, new_value);
    strcpy(new_node->value, new_value);
    strcpy(new_node->variable, new_variable);
    new_node->next = next;
    return new_node;
}

/* Insert a new node with the value num into this position of list front. */
int insert(char* variable, char* value, Node **front_ptr, int position) {
    Node *curr = *front_ptr;

    if (position == 0) {
        *front_ptr = create_node(variable, value, *front_ptr);
        return 0;
    }

    for (int i = 0; i < position - 1 && curr != NULL; i++) {
        curr = curr->next;
    }
    if (curr == NULL) {
        return -1;
    }
    Node *new_node = create_node(variable, value, curr->next);
    curr->next = new_node;

    return 0;
}

// return the value of the variable, if not found return NULL
char* find_node(char* variable, Node** front_ptr) {
    Node *curr = *front_ptr;
    while (curr != NULL) {
        if (strcmp(variable, curr->variable) == 0 ) {
            // printf("found the value %s : %s\n", variable, curr->value);
            return curr->value;
        }
        curr = curr->next;
    }

    return NULL;
}
