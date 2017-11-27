export CFLAGS = "-Wno-error"
NAME=shootin

shootin: $(NAME).h
	g++ -g -o $(NAME).exe $(NAME).cpp -lncurses -lpthread


IGNORE: clean
clean:
	-rm *.exe
