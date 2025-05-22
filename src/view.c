// src/view.c
#include "view.h"
#include "controller.h"
#include <string.h>

// GTK widget'ları
static GtkWidget *window;
static GtkWidget *cmd_entry;
static GtkTextBuffer *term_buffer;
static GtkWidget *msg_view;
static GtkTextBuffer *msg_buffer;

// Komut girişine basıldığında
static void on_entry_activate(GtkEntry *entry, gpointer user_data) {
    const char *input = gtk_entry_get_text(entry);
    char output[4096];
    int code = controller_handle_input(input, output, sizeof(output));

    gtk_text_buffer_insert_at_cursor(term_buffer, output, -1);
    gtk_text_buffer_insert_at_cursor(term_buffer, "\n", -1);

    gtk_entry_set_text(entry, "");
}

// Periyodik mesaj sorgulama (2 saniye)
static gboolean on_timeout(gpointer user_data) {
    char msgs[4096];
    int n = controller_poll_messages(msgs, sizeof(msgs));
    if (n > 0) {
        gtk_text_buffer_set_text(msg_buffer, msgs, n);
    }
    return TRUE;
}

int view_init(int *argc, char ***argv) {
    gtk_init(argc, argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *term_view = gtk_text_view_new();
    term_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(term_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(term_view), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), term_view, TRUE, TRUE, 0);

    cmd_entry = gtk_entry_new();
    g_signal_connect(cmd_entry, "activate", G_CALLBACK(on_entry_activate), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), cmd_entry, FALSE, FALSE, 0);

    msg_view = gtk_text_view_new();
    msg_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(msg_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(msg_view), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), msg_view, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    g_timeout_add(2000, on_timeout, NULL);

    return 0;
}

void view_main_loop(void) {
    gtk_main();
}

void view_cleanup(void) {
    controller_cleanup();
}
