// src/model.c
#include "model.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE 4096
#define SHARED_FILE_PATH "/mymsgbuf"

#define errExit(msg)        \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

// POSIX paylaşılan bellek tampon yapısı
typedef struct shmbuf {
    sem_t sem;     // Senkronizasyon için semafor
    size_t cnt;    // msgbuf içinde kullanılan bayt sayısı
    int fd;        // paylaşılan bellek dosya tanıtıcısı
    char msgbuf[]; // mesajlar için esnek dizi
} ShmBuf;

// Paylaşılan bellek adresi
static ShmBuf *shmp = NULL;

// Paylaşılan belleği ve semaforu başlatır
int model_init(void) {
    int fd = shm_open(SHARED_FILE_PATH, O_CREAT | O_RDWR, 0600);
    if (fd < 0) errExit("shm_open hatası");

    if (ftruncate(fd, BUF_SIZE) == -1) errExit("ftruncate hatası");

    void *addr = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) errExit("mmap hatası");

    shmp = (ShmBuf *)addr;
    shmp->fd = fd;
    shmp->cnt = 0;
    if (sem_init(&shmp->sem, 1, 1) == -1) errExit("sem_init hatası");

    return 0;
}

// Bir shell komutunu çalıştırır ve çıktısını output_buf içine yazar
int model_execute_command(const char *cmdline, char *output_buf, size_t bufsize) {
    if (!cmdline || !output_buf) return -1;

    char *cmd = strdup(cmdline);
    if (!cmd) return -1;

    char *argv[64];
    int argc = 0;
    char *tok = strtok(cmd, " ");
    while (tok && argc < 63) {
        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }
    argv[argc] = NULL;
    if (argc == 0) {
        free(cmd);
        return -1;
    }

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe hatası");
        free(cmd);
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork hatası");
        free(cmd);
        return -1;
    } else if (pid == 0) {
        // Çocuk: stdout ve stderr'i pipe'a yönlendir
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        execvp(argv[0], argv);
        perror("execvp hatası");
        exit(127);
    } else {
        // Ebeveyn: pipe'dan oku ve child'ı bekle
        close(pipefd[1]);
        ssize_t n = read(pipefd[0], output_buf, bufsize - 1);
        if (n < 0) perror("read hatası");
        else output_buf[n] = '\0';
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);
        free(cmd);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}

// Paylaşılan buffer'a mesaj gönderir
int model_send_message(const char *msg) {
    if (!shmp || !msg) return -1;
    size_t len = strlen(msg);
    size_t max_msg = BUF_SIZE - sizeof(ShmBuf);
    if (len + 1 > max_msg) return -1;

    sem_wait(&shmp->sem);
    // Yer kalmazsa başa sar
    if (shmp->cnt + len + 1 > max_msg) {
        shmp->cnt = 0;
    }
    memcpy(&shmp->msgbuf[shmp->cnt], msg, len);
    shmp->msgbuf[shmp->cnt + len] = '\n';
    shmp->cnt += len + 1;
    sem_post(&shmp->sem);

    return 0;
}

// Paylaşılan buffer'dan tüm mesajları okur
int model_read_messages(char *out_buf, size_t bufsize) {
    if (!shmp || !out_buf) return -1;

    sem_wait(&shmp->sem);
    size_t to_read = shmp->cnt;
    if (to_read > bufsize - 1) to_read = bufsize - 1;
    memcpy(out_buf, shmp->msgbuf, to_read);
    out_buf[to_read] = '\0';
    sem_post(&shmp->sem);

    return to_read;
}

// Temizlik: semafor, bellek ve shared memory silme
void model_cleanup(void) {
    if (shmp) {
        sem_destroy(&shmp->sem);
        munmap(shmp, BUF_SIZE);
        shm_unlink(SHARED_FILE_PATH);
        shmp = NULL;
    }
}
