#源文件
SOURCE = main.cpp

#可执行文件名称
TARGET = server

#使用g++编译，且以C++11标准
CXX = g++ -Wall -std=c++17

#opencv需要的参数
CFLAGS = #`pkg-config opencv --cflags --libs`

TARGET:
	    $(CXX) -o $(TARGET) $(SOURCE) $(CFLAGS) $(UFLAGS)

clean:
		rm -f *.a *.o *.ymal $(TARGET) core *~ *.so *.lo *.swp

# useful to make a backup "make tgz"
tgz: clean
	mkdir -p backups
	tar czvf ./backups_`date +"%Y_%m_%d_%H.%M.%S"`.tgz --exclude backups *