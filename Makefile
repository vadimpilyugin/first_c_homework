
main: main.c intmanip.c intmanip.h
	gcc -o main --std=c11 intmanip.c main.c

.PHONY: all 
all:  main
.PHONY: clean
clean: 
	rm -f *.o main
.PHONY: rerun
rerun: clean all
	./main