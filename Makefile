export CFLAGS = "-Wno-error"
NAME=shootin

shootin: $(NAME).h
	gcc -g -o $(NAME).exe $(NAME).c -lncurses -lpthread


IGNORE: clean
clean:
	-rm *.exe
