// src/controller.c
#include "controller.h"
#include "model.h"
#include <string.h>
#include <stdio.h>

int controller_init(void) {
    return model_init();
}

int controller_handle_input(const char *input, char *output_buf, size_t bufsize) {
    if (!input || !output_buf) return -1;

    while (*input == ' ' || *input == '\t') input++;
    if (*input == '\0') return -1;

    const char *prefix = "@msg ";
    if (strncmp(input, prefix, strlen(prefix)) == 0) {
        const char *msg = input + strlen(prefix);
        if (model_send_message(msg) == 0) {
            snprintf(output_buf, bufsize, "Mesaj gönderildi: %s", msg);
            return 0;
        } else {
            snprintf(output_buf, bufsize, "Mesaj gönderilemedi");
            return -1;
        }
    }

    return model_execute_command(input, output_buf, bufsize);
}

int controller_poll_messages(char *out_buf, size_t bufsize) {
    return model_read_messages(out_buf, bufsize);
}

void controller_cleanup(void) {
    model_cleanup();
}
