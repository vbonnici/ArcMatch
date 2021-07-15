INCLUDES= -I ./rilib/ -I ./include/
CC=g++
#CFLAGS=-std=c++11 -c -O3 
CFLAGS=-std=c++11 -c -O3 -g

#SOURCES= rids4.cpp
SOURCES= rids4_frread.cpp
OBJECTS=$(SOURCES:.cpp=.o)

#EXECUTABLE=rids4_dbg
EXECUTABLE=rids4

all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< $(INCLUDES) -o $@  
