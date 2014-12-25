INC = -I/usr/include/python2.7 -I/usr/include/boost -I/usr/include
CXXFLAGS = -fPIC -O3 -std=c++0x
SHARED = -shared
LDLIBS = -lpython2.7 -lboost_python -lpqxx -lhdf5 -lhdf5_hl
SRCS = *.hpp *.cpp
TARGET = mygraph.so
EXE = a.out
 
$(TARGET): $(SRCS)
		g++ $^ $(INC) $(CXXFLAGS) $(SHARED) -o $@ $(LDLIBS)
		 
$(EXE): $(SRCS)
		g++ $^ $(INC) $(CXXFLAGS) -o $@ $(LDLIBS)
		 
memcheck: $(EXE)
		valgrind --tool=memcheck ./a.out
		 
.PHONY:
		all clean
		 
all: $(TARGET) $(EXE) 
	 
clean:
		rm -rf *.o $(TARGET) $(EXE)
