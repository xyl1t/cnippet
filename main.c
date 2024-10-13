#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <unistd.h>

#ifdef DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

#define MAX_STRING_SIZE 4096

typedef enum {
    BEGIN,
    BEGIN_NAME,
    // CONTENT,
    END,
    END_NAME,
} Keyword;

void get_parameters(int argc, char** argv, char** snippets_path, char** snippet) {
    static struct option long_options[] = {
        {"snippets-path", required_argument, 0, 's'},
        {0 ,0 ,0 ,0}
    };

    const char *home = getenv("HOME");
    strcpy(*snippets_path, home);
    strcat(*snippets_path, "/.config/helix/snippets.txt");

    int opt;

    while ((opt = getopt_long(argc, argv, "s:", long_options, NULL)) != -1) {
        switch (opt) {
            case 's':
                *snippets_path = optarg;
                break;
            default:
                fprintf(stderr, "Usage: cnippet [--snippets-path <path>] [command]\n");
                break;
        }
    }

    if (optind < argc) {
        strncpy(*snippet, argv[optind], MAX_STRING_SIZE);
    } else if (!isatty(STDIN_FILENO)) {
        read(STDIN_FILENO, *snippet, MAX_STRING_SIZE);
    }

    if ((*snippet)[0] == '\0') {
        fprintf(stderr, "Error: Command is required. Provide it as an argument or through pipe.\n");
        fprintf(stderr, "Usage: %s [--config <config_path>] [command]\n", argv[0]);
        fprintf(stderr, "   or: echo \"command\" | %s [--config <config_path>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    char* snippets_path = malloc(MAX_STRING_SIZE);
    char* search = malloc(MAX_STRING_SIZE);

    get_parameters(argc, argv, &snippets_path, &search);

    dprintf("snippets_path: %s\n", snippets_path);
    dprintf("search: %s\n", search);

    FILE* f = fopen(snippets_path, "rb");

    if (!f) {
        printf("File '%s' not found\n", snippets_path);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);

    long fsize = ftell(f);

    fseek(f, 0, SEEK_SET);

    char* file_data = malloc(fsize + 1); // NOTE: +1 acts as null terminator to have a c string 
    fread(file_data, fsize, 1, f);
    file_data[fsize] = 0;
    fclose(f);

    dprintf("File content:\n%s\n", file_data);
    dprintf("------------\n\n");

    dprintf("Parser:\n\n");

    char* begin_keyword = "#BEGIN_SNIPPET";
    char* end_keyword = "#END_SNIPPET";

    char* buffer = malloc(MAX_STRING_SIZE * sizeof(char));
    char* snippet_name = malloc(MAX_STRING_SIZE * sizeof(char));
    bool read = false;
    bool read_snippet_name = false;

    Keyword current_keyword = BEGIN;

    int idx = 0;
    int content_idx = 0;
    for (int file_pos = 0; file_pos < fsize; file_pos++) {
        switch (current_keyword) {
            case BEGIN: {
                // dprintf("%d, %d, %c\n", file_pos, idx, file_data[file_pos]);
                if (file_data[file_pos] == begin_keyword[idx]) {
                    idx++;
                    // dprintf("true?\n");
                } else {
                    idx = 0;
                }
                if (idx == strlen(begin_keyword)) {
                    current_keyword = BEGIN_NAME;
                    idx = 0;
                    memset(buffer, 0, MAX_STRING_SIZE);
                    memset(snippet_name, 0, MAX_STRING_SIZE);
                    file_pos++; // NOTE: skip whitepsace
                }
            } break;

            case BEGIN_NAME: {
                if (file_data[file_pos] == '\n') {
                    // snippet_name[idx] = 0;
                    idx = 0;
                    current_keyword = END;
                    dprintf("BEG SNIPPET: %s\n", snippet_name);
                    if (strcmp(search, snippet_name) == 0) {
                        dprintf("========== FOUND %s ==========\n", snippet_name);
                    }
                } else {
                    snippet_name[idx] = file_data[file_pos];
                    idx++;
                }
            } break;

            case END: {
                if (file_data[file_pos] == end_keyword[idx]) {
                    idx++;
                } else {
                    idx = 0;
                }

                buffer[content_idx] = file_data[file_pos];
                content_idx++;

                if (idx == strlen(end_keyword)) {
                    buffer[content_idx - strlen(end_keyword)] = 0;
                    if (strcmp(search, snippet_name) == 0) {
                        printf("%s", buffer);
                        dprintf("========== FOUND %s ==========\n", snippet_name);
                    }
                    current_keyword = END_NAME;
                    idx = 0;
                    content_idx = 0;
                    memset(buffer, 0, MAX_STRING_SIZE);
                    file_pos++; // NOTE: skip whitepsace
                }
            } break;

            case END_NAME: {
                if (file_data[file_pos] == '\n') {
                    idx = 0;
                    current_keyword = BEGIN;
                    dprintf("END SNIPPET: %s\n\n", buffer);
                } else {
                    buffer[idx] = file_data[file_pos];
                    idx++;
                }
            } break;
        }
    }

    return 0;
}
