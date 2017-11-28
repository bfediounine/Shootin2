export CFLAGS = "-Wno-error"
NAME=shootin

shootin: $(NAME).h
	g++ -g -o -fpermissive $(NAME).exe $(NAME).cpp -lncurses -lpthread -lrt


IGNORE: clean
clean:
	-rm *.exe
