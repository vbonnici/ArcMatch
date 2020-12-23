INCLUDES= -I ./rilib/ -I ./include/
CC=g++
CFLAGS=-std=c++11 -c -O3 
#CFLAGS=-std=c++11 -c -O3 -g

SOURCES= rids4.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=thindr0
#first version with original ordering

#EXECUTABLE=thindr0_nolp
#first version with no depth limit for path reduction

#EXECUTABLE=thindr0_nolp_mono
#first version, no depth limit, specific for mono search

EXECUTABLE=thindr0_mono
#first version, specific for mono search

#EXECUTABLE=thindr0_sew
#first version with orignal ordering by taking into account edge domain size as edge weight

all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< $(INCLUDES) -o $@  
