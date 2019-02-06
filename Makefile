CC=g++
OBJ = main.o init.o routing.o connection.o
LDFLAGS = -g -Wall

%.o: %.cpp %.h
	$(CC) $(LDFLAGS) -c -o $@ $< 

main: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm main *.o
