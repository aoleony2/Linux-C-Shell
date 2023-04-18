#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"



int main(int argc, char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};
    Node *front = NULL;

    while (1) {
        // Prompt and input tokenization

        // TODO Step 2:
        // Display the prompt via the display_message function.
        display_message(prompt);
        int ret = get_input(input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr, DELIMITERS);
        // Clean exit

        if (ret != -1 && (strncmp("\n", input_buf, 1) != 0) && (token_count == 0 || (strncmp("exit", token_arr[0], 4) == 0))) {
            break;
        }
        // expand variable
        // display_error("token received", token_arr[0]);
        for (int i = 0; i < token_count; i++) {
            if (token_arr[i][0] == '$') { 
                int var_length = strlen(token_arr[i]) - 1;
                if (var_length == 0) continue; 
                char variable[var_length+1];
                strncpy(variable, &token_arr[i][1], var_length);
                variable[var_length] = '\0';
                // display_error("token received", variable);
                char* result = find_node(variable, &front);
                if (result != NULL) {
                    token_arr[i] = result;
                }
            }
        }

        // Command execution
        if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);
            if (builtin_fn != NULL) {
                ssize_t err = builtin_fn(token_arr);
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", token_arr[0]);
                }
            } else if (token_count == 1 && strchr(token_arr[0], '=') != NULL){
                // get the variable name and its value
                char* temp = strchr(token_arr[0], '=');
                int index = (int)(temp - token_arr[0]); // get index of =
                char variable[index+1];
                int length_of_value = strlen(token_arr[0]) - index;
                char value[length_of_value+1];

                strncpy(variable, token_arr[0], index);
                strncpy(value, &token_arr[0][index+1], length_of_value);
                variable[index] = '\0';
                value[length_of_value-1] = '\0';

                // display_error("variable: ", variable);
                // display_error("value: ", value);

                insert(variable, value, &front, 0);
                find_node(variable, &front);

            } else {
                display_error("ERROR: Unrecognized command: ", token_arr[0]);
            }
        }
        // Node* curr = front;
        // while (curr != NULL) {
        //     display_error(curr->variable, curr->value);
        //     curr = curr->next;
        // }
    }
    Node* curr = front;
    while (curr != NULL) {
        Node* temp = curr->next;
        free(curr);
        curr = temp;
    }
    return 0;
}
