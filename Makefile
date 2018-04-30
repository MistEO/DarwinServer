#源文件
SOURCE = main.cpp abstractmessage.cpp requestmessage.cpp responsemessage.cpp resourcecontrol.cpp

#可执行文件名称
TARGET = server.o

CXX = g++

CXXFLAGS = -Wall -std=c++11 -g

CVCFG = `pkg-config opencv --cflags --libs`

LIBS = -lpthread

TARGET:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(CVCFG) $(LIBS)

clean:
	rm -f *.a *.o *.ymal $(TARGET) core *~ *.so *.lo *.swp

# useful to make a backup "make tgz"
tgz:
	clean
	mkdir -p backups
	tar czvf ./backups_`date +"%Y_%m_%d_%H.%M.%S"`.tgz.gz --exclude backups *
	rm -rf backups
