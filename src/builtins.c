#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include "builtins.h"
#include "io_helpers.h"


// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;

    if (tokens[index] != NULL) {
        // TODO:
        // Implement the echo command
        display_message(tokens[index]);
        index++;
    }
    while (tokens[index] != NULL) {
        // TODO:
        // Implement the echo command
        display_message(" ");
        display_message(tokens[index]);
        index += 1;
    }
    display_message("\n");

    return 0;
}
int directory_size(char* dir) {
    int n=0;
    DIR *pDIR;
    struct dirent *di;
    // display_error("dir: ", dir);
    pDIR = opendir(dir);
    if (pDIR == NULL) {
        display_error("ERROR: Invalid path", "");
        display_error("ERROR: Builtin failed: ls", "");
        return -1;
    }
    //Determine the number of files
    while((di = readdir(pDIR))) {
        struct stat myFile;
        stat(di->d_name, &myFile);
        if (S_ISDIR(myFile.st_mode) && strcmp(di->d_name, ".") != 0 && strcmp(di->d_name, "..") != 0) {
            n++;
        } 
    }
    closedir(pDIR);
    return n;
}

int load_dir_content(char* dir, int level, char* keyword) {
    int size = directory_size(dir);
    if (size == -1) return 0;
    char num[20];
    sprintf(num, "%d", size);
    // display_error("size: ", num);
    char file[size][256]; // array holds filenames, each file has max length 256 bytes
    DIR* pDIR = opendir(dir);
    struct dirent *d;
    int i = 0;
    while ((d = readdir(pDIR))) {
        struct stat myFile;

        char absolute_path[256];
        memset(absolute_path, 0, sizeof(absolute_path));
        strncpy(absolute_path, dir, strlen(dir));
        if (absolute_path[strlen(dir) -1] != '/') {
            strcat(absolute_path, "/");
        }
        strncat(absolute_path, d->d_name, strlen(d->d_name));

        stat(absolute_path, &myFile);
        // display_error("path: ", absolute_path);
        if (S_ISDIR(myFile.st_mode)) {
            // a directory that is not . or ..
            if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
                memset(file[i], 0, strlen(absolute_path));
                strncpy(file[i], absolute_path, strlen(absolute_path));
                // display_error("abs path", absolute_path);
                // strcat(file[i], "/");
                // display_error("file[i]", file[i]);
                i++;
            }
        }
        if (strstr(d->d_name, keyword)) {
            display_message(d->d_name);
            display_message("\n");
        }
        
    }
    // for (int i = 0; i < size; i++) {
    //     display_error("file: ", file[i]);
    // }
    // char num[10];
    // sprintf(num, "num:%d\n", i);
    // display_message(num);

    // printf("%s", file[0]);
    if (level <= 1) {
        closedir(pDIR);
        return 0;
    }
    for (int j = 0; j < i; j++) {
        // display_error("bruh:", file[j]);
        load_dir_content(file[j], level-1, keyword);
    }
    closedir(pDIR);
    return 0;
}
void print_error(char** tokens, int start, int end) {
    display_message("Error: bad arguments:");
    while (start < end) {
        display_message(" ");
        display_message(tokens[start]);
        start++;
    }
    display_message("\n");
}


ssize_t bn_ls(char** tokens) {
    int arguments = 1;
    // int token_count = 0;
    while (tokens[arguments] != NULL) {
        arguments++;
    }
    // printf("%d", arguments);
    char dir[300] = "./";
    if (arguments == 1) { // ls
        load_dir_content(dir, 1, "");
    } else if (arguments == 2) {
        load_dir_content(tokens[1], 1, "");
    } else if (arguments == 3 && strcmp(tokens[1], "--f") == 0) {
        load_dir_content(dir, 1, tokens[2]);
    } else if (arguments == 4 && strcmp(tokens[2], "--f") == 0) {
        load_dir_content(tokens[1], 1, tokens[3]);
    } else if (arguments == 5) {
        if (strcmp(tokens[1], "--rec") == 0 && strcmp(tokens[3], "--d") == 0) {
            int value = atoi(tokens[4]);
            if (value > 0) {
                load_dir_content(tokens[2], value, "");
            }
        } else if (strcmp(tokens[3], "--rec") == 0 && strcmp(tokens[1], "--d") == 0) {
            int value = atoi(tokens[2]);
            if (value > 0) {
                load_dir_content(tokens[4], value, "");
            }
        } else {
            print_error(tokens, 1, arguments);
        }
    } else if (arguments == 7) {
        char path[256];
        char keyword[256];
        int value = -1;
        if (strcmp(tokens[1], "--rec") == 0) {
            strcpy(path, tokens[2]);
            if (strcmp(tokens[3], "--f") == 0) {
                strcpy(keyword, tokens[4]);
                value = atoi(tokens[6]);
            } else if (strcmp(tokens[5], "--f") == 0) {
                strcpy(keyword, tokens[6]);
                value = atoi(tokens[4]);
            } else print_error(tokens, 1, arguments);
        } else if (strcmp(tokens[3], "--rec") == 0) {
            strcpy(path, tokens[4]);
            if (strcmp(tokens[1], "--f") == 0) {
                strcpy(keyword, tokens[2]);
                value = atoi(tokens[6]);
            } else if (strcmp(tokens[5], "--f") == 0) {
                strcpy(keyword, tokens[6]);
                value = atoi(tokens[2]);
            } else print_error(tokens, 1, arguments);
        } else if (strcmp(tokens[5], "--rec") == 0){
            strcpy(path, tokens[6]);
            if (strcmp(tokens[3], "--f") == 0) {
                strcpy(keyword, tokens[4]);
                value = atoi(tokens[2]);
            } else if (strcmp(tokens[1], "--f") == 0) {
                strcpy(keyword, tokens[2]);
                value = atoi(tokens[4]);
            }
        } else print_error(tokens, 1, arguments);
        if (value >= 0) {
            load_dir_content(path, value, keyword);
        }
    }
    return 0;

}

ssize_t bn_cd(char** tokens) {
    if (tokens[2] != NULL || tokens[1] == NULL) return -1;
    char dir[100] = "..";
    if (strcmp(tokens[1], "...") == 0) {
        for (int i = 0; i< 3; i++) {
            strcat(dir, "/..");
        }
    } else if (strcmp(tokens[1], "....") == 0) {
        for (int j = 0; j < 4; j++) {
            strcat(dir, "/..");
        }
    } else {
        memset(dir, 0 , strlen(dir));
        strncpy(dir, tokens[1], strlen(tokens[1]));
    }
    printf("%s", dir);
    if (chdir(dir) == -1) {
        display_error("ERROR: Invalid path", "");
        display_error("ERROR: Builtin failed: cd", "");
    }
    return 0;
}

ssize_t bn_cat(char** tokens) {
    if (tokens[1] == NULL) {
        display_error("ERROR: No input source provided", "");
        display_error("ERROR: Builtin failed: cat", "");
        return -1;
    } 
    FILE* fp;
    if ((fp = fopen(tokens[1], "r")) == NULL) {
        display_error("ERROR: Cannot open file", "");
        display_error("ERROR: Builtin failed: cat", "");
        return -1;
    } else {
        char content[64];
        while (fgets(content, 64, fp) != NULL) {
            display_message(content);
        }
    }
    display_message("\n");
    fclose(fp);
    return 0;
}

ssize_t bn_wc(char** tokens) {
    if (tokens[1] == NULL) {
        display_error("ERROR: No input source provided", "");
        display_error("ERROR: Builtin failed: wc", "");
        return -1;
    } 
    FILE* fp;
    if ((fp = fopen(tokens[1], "r")) == NULL) {
        display_error("ERROR: Cannot open file", "");
        display_error("ERROR: Builtin failed: wc", "");
        return -1;
    } else {
        char ch;
        int word_count = 0;
        int char_count = 0;
        int new_line = 0;
        while (ch != EOF) {
            ch = fgetc(fp);
            if (ch == '\n') {
                new_line++;
            }
            if (isalnum(ch) && ((ch = fgetc(fp)) != ' ' || ch != '\t' || ch != '\r' || ch != '\n')) {
                word_count++;
            }
            char_count++;
        }
        char res[50];
        sprintf(res, "word count %d\ncharacter count %d\nnewline count %d\n", word_count, char_count, new_line);
        display_message(res);
    }
    display_message("\n");
    fclose(fp);
    return 0;
}