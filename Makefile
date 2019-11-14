CC=gcc


all: shell

shell: shell.o
			$(CC)  shell.o  -o  shell

shell.o: shell.c
	 		$(CC)  -c shell.c

clean:
	 		@echo "On efface les fichiers objets"
	 		rm *.o
			rm shell
