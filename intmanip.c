#include "intmanip.h"
// lseek, read
#include <unistd.h>
// fprintf
#include <stdio.h>
// atoi
#include <stdlib.h>

#define EOL '\n'
#define ZERO_BYTE '\0'

int find_second_last (const char *str, const size_t len, const char char_to_find) {
	// считаем, что последний символ равен char_to_find
	int i;
	for (i = len-2; i > -1 && str[i] != char_to_find; i--) {}
	// если второго char_to_find нет, то -1
	// иначе индекс второго char_to_find
	return i;
}

int get_last_int (char *str, size_t len) {
	printf("Debug: str=<%s>, len=%zu\n", str, len);
	// находим позицию первой цифры последнего числа
	int pos = find_second_last (str, len, EOL) + 1;
	// убираем последний \n
	str[len-1] = ZERO_BYTE;
	// читаем число
	int last_int = atoi (str+pos);
	printf("Debug: last_int=%d", last_int);
	return last_int;
}

int read_last_int (int fd) {
	const int DEFAULT_VALUE = 0;
	const int MAX_NUMBER_LEN = 10; // 2**31 - 1 = 2147483647 (10 цифр)
	const int BYTES_TO_READ = MAX_NUMBER_LEN + 1; // число + \n
	const int BUFSIZE = BYTES_TO_READ + 1; // строка + \0

	// сдвигаем указатель позиции в конец файла
	if (lseek(fd, 0, SEEK_END) < 0) {
		fprintf(stderr, "lseek в конец не удался");
		return ERROR;
	}
	// читаем позицию указателя
	off_t file_size;
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
		fprintf(stderr, "lseek на смещение не удался");
		return ERROR;
	}
	// выделяем буфер
	char *buffer = (char *) malloc (BUFSIZE);
	if (buffer == NULL) {
		fprintf(stderr, "malloc буфера не удался");
		return ERROR;
	}
	// читаем n_bytes символов
	ssize_t n_bytes = read (fd, buffer, BYTES_TO_READ);
	if (n_bytes < 0) {
		// произошла ошибка чтения
		fprintf(stderr, "read не удался");
		return ERROR;
	}
	// устанавливаем последний байт прочитанной строки в нулевой байт
	buffer[n_bytes] = ZERO_BYTE;
	// проверяем, что последний символ \n
	// n_bytes > 0, т.к. файл непустой
	if (buffer[n_bytes-1] != EOL) {
		fprintf(stderr, "формат нарушен: последний символ не \\n");
		return ERROR;
	}
	// читаем последнее число из строки
	int last_int = get_last_int(buffer, n_bytes);
	// возвращаем его
	return last_int;
}