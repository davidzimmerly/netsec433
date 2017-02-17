CXX = g++
CXXFILES = main.cpp encrypt_ram.cpp 

CXXFLAGS = -std=c++11
CPPFLAGS = -maes -msse4 
LIBS = -lcurl

all:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CXXFILES) $(LIBS) 

clean:
	rm -f prog *.o
