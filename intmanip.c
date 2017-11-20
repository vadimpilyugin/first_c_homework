#include "intmanip.h"
// lseek, read
#include <unistd.h>
// fprintf, snprintf
#include <stdio.h>
// atoi
#include <stdlib.h>

#define EOL '\n'
#define ZERO_BYTE '\0'
// 2**31 - 1 = 2147483647 (10 цифр)
#define MAX_NUMBER_LEN 10

int find_second_last (const char *str, const size_t len, const char char_to_find) {
	// считаем, что последний символ равен char_to_find
	int i;
	for (i = len-2; i > -1 && str[i] != char_to_find; i--) {}
	// если второго char_to_find нет, то -1
	// иначе индекс второго char_to_find
	return i;
}

static void my_perror (const char *msg) {
	fprintf(stderr, "%s\n", msg);
}

int get_last_int (char *str, size_t len) {
	#if DEBUG
		printf("Debug: str=<%s>, len=%zu\n", str, len);
	#endif
	// находим позицию первой цифры последнего числа
	int pos = find_second_last (str, len, EOL) + 1;
	// убираем последний \n
	str[len-1] = ZERO_BYTE;
	// читаем число
	int last_int = atoi (str+pos);
	#if DEBUG
		printf("Debug: last_int=%d\n", last_int);
	#endif
	return last_int;
}

int read_last_int (int fd) {
	const int DEFAULT_VALUE = 0;
	const int BYTES_TO_READ = MAX_NUMBER_LEN + 1; // число + \n
	const int BUFSIZE = BYTES_TO_READ + 1; // строка + \0

	// сдвигаем указатель позиции в конец файла
	if (lseek(fd, 0, SEEK_END) < 0) {
		my_perror ("lseek в конец не удался");
		return ERROR;
	}
	// читаем позицию указателя
	int file_size; // FIXME: размер файла может превышать верхнюю границу int

	if ((file_size = lseek(fd, 0, SEEK_CUR)) < 0) {
		fprintf(stderr, "lseek в текущую позицию не удался");
		return ERROR;
	}
	// если файл пуст
	if (file_size == 0)
		return DEFAULT_VALUE;
	// файл непуст
	// узнаем, что больше: число байт, которые надо прочитать или размер файла
	int offset = file_size > BYTES_TO_READ ? -BYTES_TO_READ : -file_size;
	// смещаемся от конца файла на вычисленное смещение
	if (lseek(fd, offset, SEEK_END) < 0) {
		my_perror ("lseek на смещение не удался");
		return ERROR;
	}
	// выделяем буфер
	char *buffer = (char *) malloc (BUFSIZE);
	if (buffer == NULL) {
		my_perror ("malloc буфера не удался");
		return ERROR;
	}
	// читаем n_bytes символов
	ssize_t n_bytes = read (fd, buffer, BYTES_TO_READ);
	if (n_bytes < 0) {
		// произошла ошибка чтения
		my_perror ("read не удался");
		free (buffer);
		return ERROR;
	}
	// устанавливаем последний байт прочитанной строки в нулевой байт
	buffer[n_bytes] = ZERO_BYTE;
	// проверяем, что последний символ \n
	// n_bytes > 0, т.к. файл непустой
	if (buffer[n_bytes-1] != EOL) {
		my_perror ("формат нарушен: последний символ не \\n");
		free (buffer);
		return ERROR;
	}
	// читаем последнее число из строки
	int last_int = get_last_int(buffer, n_bytes);
	// освобождаем буфер
	free (buffer);
	// возвращаем его
	return last_int;
}

int write_incremented_int(int fd, int last_int) {
	if (last_int < 0) {
		my_perror ("формат нарушен: число для записи < 0");
	}
	// максимальный размер строки с числом
	const int MAX_STRLEN = MAX_NUMBER_LEN + 1; // число + \0
	// выделяем буфер под число
	char *buffer = (char *) malloc (MAX_STRLEN);
	if (buffer == NULL) {
		my_perror ("malloc буфера для числа не удался");
		return ERROR;
	}
	// переводим число из int в последовательность char
	// snprintf записывает \0 после числа и возвращает длину строки
	int len;
	if ((len = snprintf(buffer, MAX_STRLEN, "%d", last_int)) < 0) {
		my_perror ("snprintf не удался");
		free (buffer);
		return ERROR;
	}
	// вместо NULL BYTE ставим \n
	buffer[len] = EOL;
	// пишем его в файл
	if (write (fd, buffer, len+1) < 0) {
		my_perror ("write следующего числа не удался");
		return ERROR;
	}
	// освобождаем буфер
	free (buffer);
	return SUCCESS;
}