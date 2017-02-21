CXX = g++
CXXFILES = main.cpp encrypt_ram.cpp encrypt_ram_rsa.cpp BigInteger.cpp

CXXFLAGS = -std=gnu++11
CPPFLAGS = -g -maes -msse4 -Wall 
LIBS = -lcurl

all:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CXXFILES) $(LIBS) 

clean:
	rm -f prog *.o