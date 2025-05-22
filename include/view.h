// include/view.h
#ifndef VIEW_H
#define VIEW_H

#include <gtk/gtk.h>

// GTK ve pencere/widget'ları başlatır
int view_init(int *argc, char ***argv);

// GTK main döngüsünü çalıştırır
void view_main_loop(void);

// Temizlik: controller_cleanup() çağrılır
void view_cleanup(void);

#endif // VIEW_H
