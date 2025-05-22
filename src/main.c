// src/main.c
#include "controller.h"
#include "view.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (controller_init() != 0) {
        fprintf(stderr, "Model başlatılamadı\n");
        return 1;
    }

    if (view_init(&argc, &argv) != 0) {
        fprintf(stderr, "Arayüz başlatılamadı\n");
        return 1;
    }

    view_main_loop();
    view_cleanup();
    return 0;
}
