SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))

TARGET = server

CXX = g++

CXXFLAGS = -Wall -std=c++11 -g

CVCFG = `pkg-config opencv --cflags --libs`

LIBS = -lpthread

TARGET: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(CVCFG) $(LIBS)

resourcecontrol.o: resourcecontrol.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(CVCFG)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f *.a *.o *.ymal $(TARGET) core *~ *.so *.lo *.swp
