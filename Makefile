main: main.c
	gcc -o main main.c sqlite3.c -lshout -pthread -ldl -lm
