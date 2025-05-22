// include/controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stddef.h>

// Model katmanını başlatır
int controller_init(void);

// Kullanıcı girdisini işler:
// - "@msg <mesaj>" formatındaysa model_send_message()
// - Aksi halde model_execute_command()
// output_buf: çıktının yazılacağı buffer
// bufsize: buffer boyutu
// Döner: çıkış kodu veya -1
int controller_handle_input(const char *input, char *output_buf, size_t bufsize);

// Paylaşılan bellekten mesajları okur
int controller_poll_messages(char *out_buf, size_t bufsize);

// Temizlik: model_cleanup() çağrılır
void controller_cleanup(void);

#endif // CONTROLLER_H
