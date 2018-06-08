SOURCES = $(wildcard *.cpp)
OBJSDIR = ./objs
OBJS = $(foreach obj, $(patsubst %.cpp, %.o, $(SOURCES)), $(OBJSDIR)/$(obj))
TARGET = server

DARWINDEF = $(shell if [ -d /darwin ]; then echo "-DDARWIN"; elif [ -d /robotis ]; then echo "-DROBOTIS"; fi;)

CXX = g++
CXXFLAGS := -Wall -std=c++11
OPENCV = `pkg-config opencv --cflags --libs`
LIBS = -lpthread

BUILDSTEPS := prepare $(OBJS)
ifeq ($(DARWINDEF), -DDARWIN)
	CXXFLAGS += $(-DDARWINDEF)
	BUILDSTEPS += darwin.a
	DARWIN_CXXFLAGS = -Wall $(DARWINDEF)
	DARWIN_INC = -I/darwin/Linux/include/ -I/darwin/Framework/include
	DARWIN_LIB = /darwin/Linux/lib/darwin.a
endif
ifeq ($(DARWINDEF), -DROBOTIS)
	CXXFLAGS += $(DARWINDEF)
	BUILDSTEPS += robotis.a
	DARWIN_CXXFLAGS = -Wall $(DARWINDEF)
	DARWIN_INC = -I/robotis/Linux/include/ -I/robotis/Framework/include
	DARWIN_LIB = /robotis/Linux/lib/darwin.a
endif

TARGET: $(BUILDSTEPS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(DARWIN_LIB) $(OPENCV) $(LIBS)

test:
	@echo $(CXX) $(CXXFLAGS)

prepare:
	@mkdir -p $(OBJSDIR)

robotis.a:
	make -C /robotis/Linux/build

darwin.a:
	make -C /darwin/Linux/build

$(OBJSDIR)/requestcontrol.o: requestcontrol.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(DARWIN_INC)

$(OBJSDIR)/motion.o: motion.cpp
	$(CXX) $(DARWIN_CXXFLAGS) -o $@ -c $< $(DARWIN_INC)

$(OBJSDIR)/resourcecontrol.o: resourcecontrol.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(OPENCV)

$(OBJSDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean #  cleanlib
clean:
	rm -rf $(TARGET) $(OBJS)

# cleanlib:
#	make -C /darwin/Linux/build clean
