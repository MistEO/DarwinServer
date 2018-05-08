SOURCES = $(wildcard *.cpp)
OBJSDIR = ./objs
OBJS = $(foreach obj, $(patsubst %.cpp, %.o, $(SOURCES)), $(OBJSDIR)/$(obj))
TARGET = server

CXX = g++
CXXFLAGS = -Wall -std=c++11# -DDARWIN
#DARWIN_CXXFLAGS = -Wall -std=c++98 -DDARWIN

#DARWIN_INC = -I/darwin/Linux/include -I/darwin/Framework/include
#DARWIN_LIB = /darwin/Linux/lib/darwin.a
OPENCV = `pkg-config opencv --cflags --libs`
LIBS = -lpthread

TARGET: prepare $(OBJS)# darwin.a 
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(DARWIN_LIB) $(OPENCV) $(LIBS)

prepare:
	@echo $(OBJS)
	mkdir -p $(OBJSDIR)

# darwin.a:
# 	make -C /darwin/Linux/build

# main.o: main.cpp
# 	$(CXX) $(CXXFLAGS) -o $@ -c $< $(DARWIN_INC)

# motion.o: motion.cpp
# 	$(CXX) $(DARWIN_CXXFLAGS) -o $@ -c $< $(DARWIN_INC)

$(OBJSDIR)/resourcecontrol.o: resourcecontrol.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(OPENCV)

$(OBJSDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: rebuild clean #  cleanlib
clean:
	rm -rf $(TARGET) $(OBJS)

# cleanlib:
#	make -C /darwin/Linux/build clean
