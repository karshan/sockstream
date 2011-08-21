all: sockstream.o
sockstream.o: sockstream.cpp sockstream.h
	gcc -c sockstream.cpp
clean:
	rm *.o
