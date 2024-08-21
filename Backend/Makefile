# Compiler and compiler flags

CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lX11 -lsqlite3

#Source files and object files
SRCS = main.cpp QueryClass.cpp WWindow.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = build

#Default target
all: $(TARGET)

#Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

#Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)

#Indicates clean is not a real file
.PHONY: clean
