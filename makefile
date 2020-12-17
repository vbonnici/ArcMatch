INCLUDES= -I ./rilib/ -I ./include/
CC=g++
CFLAGS=-std=c++11 -c -O3 
#CFLAGS=-std=c++11 -c -O3 -g

SOURCES= ri3.cpp
OBJECTS=$(SOURCES:.cpp=.o)
#EXECUTABLE=ri351slhreds
#EXECUTABLE=ri4
#EXECUTABLE=ri41
#EXECUTABLE=ri40
#EXECUTABLE=ri42
EXECUTABLE=ri402


all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< $(INCLUDES) -o $@  
