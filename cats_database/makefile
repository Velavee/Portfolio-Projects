
cats5: cats5.o cats5Imp.o
	g++ -std=c++11 -Wall cats5.o cats5Imp.o -o cats5

cats5.o: cats5.cpp
	g++ -std=c++11 -Wall -c cats5.cpp

cats5Imp.o: cats5Imp.cpp cats5Header.h
	g++ -std=c++11 -Wall -c cats5Imp.cpp


clean:
	rm *.o cats5
