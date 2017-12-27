export CFLAGS=-std=gnu99 -g -Wno-error
export CPPFLAGS=-std=c++0x -g -Wno-error
export LDFLAGS=-L/usr/local/lib -lboost_system -lpthread
NAME=shootin
OBJS=bintree.o $(NAME).o
PROTOCOL=udp

shootin: $(NAME).h $(PROTOCOL)_connection $(OBJS) # tcp_async
	g++ -o $(NAME).exe $(OBJS) -lncurses -lpthread -lrt

$(PROTOCOL)_connection: 
	g++ -o net/$(PROTOCOL)_server.exe net/$(PROTOCOL)_server.cpp $(LDFLAGS)
	g++ -o net/$(PROTOCOL)_client.exe net/$(PROTOCOL)_client.cpp $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $<
# tcp_async:
# g++ -std=c++11 -o net/tcp_async_server.exe net/tcp_async_server.cpp $(LDFLAGS)

all: shootin

IGNORE: clean
clean:
	-rm *.o *.exe
