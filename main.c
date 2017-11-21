// fprintf
#include <stdio.h>
// open
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
// flock
#include <sys/file.h>
// lseek, fsync
#include <unistd.h>
// strerror
#include <string.h>
// errno
#include <errno.h>
// read_last_int, write_incremented_int
#include "intmanip.h"

// количество чисел, которые запишет программа в файл
#define MAX_CYCLES 1000

static void my_perror(const char *msg) {
  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}

void release_lock(int fd) {
  // отпускаем lock
  if (flock (fd, LOCK_UN) < 0) {
    my_perror("unlock не удался");
  }
}

int main () {
  // имя открываемого файла
  const char *fn = "file.txt";
  // дескриптор открытого файла
  int fd;
  // если открытие файла не удалось
  if ((fd = open (fn, O_RDWR|O_APPEND|O_CREAT, 0777)) < 0) {
    // выходим из программы
    my_perror("open не удался");
    return SUCCESS;
  }
  // файл успешно открылся
  // в цикле от 0 до MAX_CYCLES
  for (int i = 0; i < MAX_CYCLES; i++) {
    // если блокировка файла не удалась
    if (flock (fd,LOCK_EX) < 0) {
      my_perror("flock не удался");
      break;
    }
    // файл заблокирован для остальных процессов
    // читаем последнее число из файла
    int last_int = read_last_int(fd);
    if (last_int == ERROR) {
      my_perror ("read_last_int не удался");
      release_lock (fd);
      break;
    }
    // пишем увеличенное на 1 число в файл
    if (write_incremented_int(fd, last_int+1) == ERROR) {
      my_perror ("write_incremented_int не удался");
      release_lock (fd);
      break;
    }
    // заливаем данные на диск
    if (fsync (fd) < 0) {
      my_perror ("fsync не удался");
      release_lock (fd);
      break;
    }
    // если не вышло отпустить lock
    if (flock (fd, LOCK_UN) < 0) {
      my_perror("unlock не удался");
      // NOTE: Linux отпускает lock при выходе из программы
      break;
    }
  }
  // закрываем файл и выходим из программы
  if (close (fd) < 0) {
    my_perror ("close не удался");
  }
  return SUCCESS;
}
