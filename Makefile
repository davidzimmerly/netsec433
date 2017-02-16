CC=g++
CFLAGS=-maes -msse4 -std=c++11 
LDFLAGS=-lcurl


src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

LDFLAGS = -lcurl

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(obj) myprog