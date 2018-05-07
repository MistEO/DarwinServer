SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))
TARGET = server

CXX = g++
CXXFLAGS = -Wall -std=c++11 -DDARWIN
DARWIN_CXXFLAGS = -Wall -std=c++98 -DDARWIN

DARWIN_INC = -I/darwin/Linux/include -I/darwin/Framework/include
DARWIN_LIB = /darwin/Linux/lib/darwin.a
OPENCV = `pkg-config opencv --cflags --libs`
LIBS = -lpthread

TARGET: darwin.a $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(DARWIN_LIB) $(OPENCV) $(LIBS)

darwin.a:
	make -C /darwin/Linux/build

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(DARWIN_INC)

motion.o: motion.cpp
	$(CXX) $(DARWIN_CXXFLAGS) -o $@ -c $< $(DARWIN_INC)

resourcecontrol.o: resourcecontrol.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(OPENCV)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean cleanlib
clean:
	rm -f *.a *.o *.ymal $(TARGET) core *~ *.so *.lo *.swp

cleanlib:
	make -C /darwin/Linux/build clean
