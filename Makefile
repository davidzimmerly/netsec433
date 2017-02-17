CC=g++
CFLAGS=-maes -msse4 -std=c++11 
LDFLAGS=-lcurl
SOURCES=main.cpp encrypt_ram.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=encram
all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@