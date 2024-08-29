# Compiler and compiler flags

CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lX11 -lsqlite3

#Directories
SRC_DIR = source
INC_DIR = include
OBJ_DIR = object
LOG_DIR = log
DB_DIR = db
ERROR_LOG = $(LOG_DIR)/ErrorLog.txt
TARGET = build

#Create object directory if it does not exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

#Create Log directory if it does not exist
$(LOG_DIR):
	mkdir -p $(LOG_DIR)

#Create db directory if it does not exist
$(DB_DIR):
	mkdir -p $(DB_DIR)

#Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)


#Default target
all: $(LOG_DIR) $(OBJ_DIR) $(DB_DIR) $(TARGET)

#Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

#Compile source files into object files
#Verify $(OBJ_DIR) is created before compiling object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

#Removes build files
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
	rm -f $(ERROR_LOG)

#Runs the program and redirects all error output to ErrorLog.txt
run: $(TARGET) $(LOG_DIR) $(DB_DIR)
	./$(TARGET) 2>> $(ERROR_LOG)

#Indicates these are not real files
.PHONY : all clean run