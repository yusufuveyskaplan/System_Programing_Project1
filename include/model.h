// include/model.h
#ifndef MODEL_H
#define MODEL_H

#include <stddef.h>

// Paylaşılan belleği ve semaforu başlatır
int model_init(void);

// Shell komutunu çalıştırır, çıktıyı output_buf'a yazar
int model_execute_command(const char *cmdline, char *output_buf, size_t bufsize);

// Paylaşılan buffer'a mesaj gönderir
int model_send_message(const char *msg);

// Paylaşılan buffer'dan mesajları okur
int model_read_messages(char *out_buf, size_t bufsize);

// Semafor, bellek ve shared memory'i temizler
void model_cleanup(void);

#endif // MODEL_H
