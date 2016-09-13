CC=g++
LIB=lib
INC=include

all:	main.o lsm303.o	ublox6.o INIReader.o ini.o
	g++ main.o lsm303.o ublox6.o INIReader.o ini.o -o bin/main -lmraa

main.o: src/main.cpp $(INC)/lsm303.h $(INC)/ublox6.h $(INC)/INIReader.h
	g++ -c src/main.cpp -I $(INC)

lsm303.o: $(INC)/lsm303.h 
	g++ -c $(LIB)/lsm303.cxx -I $(INC)

ublox6.o: $(INC)/ublox6.h
	g++ -c $(LIB)/ublox6.cxx -I $(INC)

INIReader.o: $(INC)/INIReader.h $(INC)/ini.h
	g++ -c $(LIB)/INIReader.cpp -I $(INC)

ini.o: $(INC)/ini.h
	gcc -c $(LIB)/ini.c -I $(INC)

.PHONY: clean
clean:
	rm main.o lsm303.o ublox6.o INIReader.o ini.o
