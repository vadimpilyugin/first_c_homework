# first_c_homework
Первое задание по курсу "Системное программирование в Linux"

# Компиляция и запуск
```bash

make all && ./main # запуск одного экземпляра

make all && bash parallel.sh # запуск нескольких экземпляров

```
# Описание
Программа записывает последовательные целые числа в файл. Она читает последнее
число в файле и записывает в него следующее в цикле. Если файл пуст, то 
записывается 1. Если запустить несколько экземпляров программы, то они
будут совместно записывать числа в правильном порядке.

# Имплементация
Файл открывается с флагами O_RDWR|O_APPEND, в цикле ставится exclusive lock 
на файл, прочитывается последнее число и записывается следующее. Дальше
происходит flush буферов на жесткий диск и лок снимается. Цикл повторяется.
