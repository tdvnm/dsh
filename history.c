// history.c - Command history with arrow key support via termios
#include "history.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define MAX_HISTORY 1024
#define MAX_CMD_LEN 2048

static char *history[MAX_HISTORY];
static int history_count = 0;
static int history_pos = 0;

void
history_init()
{
    // Load from ~/.kreash_history
    char *home = getenv("HOME");
    if (!home) home = "/root";

    char histfile[1024];
    snprintf(histfile, sizeof(histfile), "%s/.kreash_history", home);

    FILE *f = fopen(histfile, "r");
    if (f) {
        char line[MAX_CMD_LEN];
        while (fgets(line, sizeof(line), f) && history_count < MAX_HISTORY) {
            // Remove trailing newline
            size_t len = strlen(line);
            if (len > 0 && line[len-1] == '\n')
                line[--len] = '\0';
            if (len > 0)
                history[history_count++] = strdup(line);
        }
        fclose(f);
    }
    history_pos = history_count;
}

void
history_add(const char *cmd)
{
    if (!cmd || strlen(cmd) == 0)
        return;
    if (history_count >= MAX_HISTORY) {
        free(history[0]);
        for (int i = 0; i < history_count - 1; i++)
            history[i] = history[i + 1];
        history_count--;
    }
    history[history_count++] = strdup(cmd);
    history_pos = history_count;
}

void
history_save()
{
    char *home = getenv("HOME");
    if (!home) home = "/root";

    char histfile[1024];
    snprintf(histfile, sizeof(histfile), "%s/.kreash_history", home);

    FILE *f = fopen(histfile, "w");
    if (f) {
        for (int i = 0; i < history_count; i++)
            fprintf(f, "%s\n", history[i]);
        fclose(f);
    }
}

char *
readline_with_history(const char *prompt)
{
    struct termios orig, raw;
    tcgetattr(STDIN_FILENO, &orig);
    raw = orig;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    char buffer[MAX_CMD_LEN] = {0};
    int pos = 0;

    printf("%s", prompt);
    fflush(stdout);

    while (1) {
        unsigned char c;
        if (read(STDIN_FILENO, &c, 1) <= 0)
            break;

        // Handle arrow keys (ESC [ A/B for up/down)
        if (c == 27) { // ESC
            unsigned char seq[2];
            if (read(STDIN_FILENO, seq, 2) == 2) {
                if (seq[0] == '[') {
                    if (seq[1] == 'A') { // Up arrow
                        if (history_pos > 0)
                            history_pos--;
                        // Clear line and print history entry
                        printf("\r\033[K%s%s", prompt, history[history_pos] ? history[history_pos] : "");
                        strcpy(buffer, history[history_pos] ? history[history_pos] : "");
                        pos = strlen(buffer);
                    } else if (seq[1] == 'B') { // Down arrow
                        if (history_pos < history_count - 1)
                            history_pos++;
                        else
                            history_pos = history_count;
                        printf("\r\033[K%s%s", prompt, history_pos < history_count ? history[history_pos] : "");
                        strcpy(buffer, history_pos < history_count ? history[history_pos] : "");
                        pos = strlen(buffer);
                    }
                    fflush(stdout);
                }
            }
        }
        // Backspace/Delete
        else if (c == 127 || c == 8) {
            if (pos > 0) {
                pos--;
                buffer[pos] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        }
        // Enter
        else if (c == '\n') {
            printf("\n");
            fflush(stdout);
            break;
        }
        // Regular character
        else if (pos < MAX_CMD_LEN - 1) {
            buffer[pos++] = c;
            buffer[pos] = '\0';
            printf("%c", c);
            fflush(stdout);
        }
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
    return strdup(buffer);
}
