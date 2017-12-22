export CFLAGS=-std=c++0x -g -Wno-error
export LDFLAGS=-L/usr/local/lib -lboost_system -lpthread
NAME=shootin
PROTOCOL=udp

shootin: $(NAME).h $(PROTOCOL)_connection # tcp_async
	g++ $(CFLAGS) -o $(NAME).exe $(NAME).cpp -lncurses -lpthread -lrt

$(PROTOCOL)_connection: 
	g++ -o net/$(PROTOCOL)_server.exe net/$(PROTOCOL)_server.cpp $(LDFLAGS)
	g++ -o net/$(PROTOCOL)_client.exe net/$(PROTOCOL)_client.cpp $(LDFLAGS)

# tcp_async:
# g++ -std=c++11 -o net/tcp_async_server.exe net/tcp_async_server.cpp $(LDFLAGS)

all: shootin

IGNORE: clean
clean:
	-rm *.exe
