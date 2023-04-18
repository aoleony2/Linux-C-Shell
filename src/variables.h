#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#define EQUAL "= "
struct Node;
/* Prereq: pre_str, str are NULL terminated string
 */
typedef struct node {
    char variable[64];
    char value[64];
    struct node *next;
} Node;
Node* create_node(char* variable, char* value, Node *next);
int insert(char* variable, char* value, Node **front, int position);
char* find_node (char* variable, Node** front_ptr);
#endif