export CFLAGS="-Wno-error"
export LDFLAGS=-L/usr/local/lib -lboost_system -lpthread
NAME=shootin
PROTOCOL=udp

shootin: $(NAME).h $(PROTOCOL)_server
	g++ -g -o $(NAME).exe $(NAME).cpp -lncurses -lpthread -lrt

$(PROTOCOL)_server:
	g++ -o net/$(PROTOCOL)_server.exe net/$(PROTOCOL)_server.cpp $(LDFLAGS)

all: shootin

IGNORE: clean
clean:
	-rm *.exe
