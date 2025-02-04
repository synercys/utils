CXX = g++
CXXFLAGS = -O0 -Wall

.PHONY: clean

all: RT-apps

RT-apps: RT-apps.cc
	$(CXX) $(CXXFLAGS) -o RT-apps.o RT-apps.cc

clean:
	rm -f RT-apps.o
