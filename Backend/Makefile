# Compiler and compiler flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

#Directories
SRC_DIR = source
INC_DIR = include
OBJ_DIR = object
LOG_DIR = log
LIB_DIR = lib
DB_DIR = db
LIBRARY = $(LIB_DIR)/libbackend.a
ERROR_LOG = $(LOG_DIR)/ErrorLog.txt


#Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

#Create object directory if it does not exist
$(OBJ_DIR):
	@echo "Creating object directory..."
	mkdir -p $(OBJ_DIR)

#Create Log directory if it does not exist
$(LOG_DIR):
	@echo "Creating log directory..."
	mkdir -p $(LOG_DIR)

#Create db directory if it does not exist
$(DB_DIR):
	@echo "Creating database directory..."
	mkdir -p $(DB_DIR)
#Create lib directory if it does not exist
$(LIB_DIR):
	@echo "Creating library directory..."
	mkdir -p $(LIB_DIR)
#Default target
all: $(LOG_DIR) $(OBJ_DIR) $(DB_DIR) $(LIB_DIR) $(LIBRARY)

#Build the static library
$(LIBRARY) : $(OBJS)
	@echo "Building static library..."
	ar rcs $@ $(OBJS)

#Compile source files into object files
#Verify $(OBJ_DIR) is created before compiling object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $< to $@..."
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

#Removes build files
clean:
	rm -f $(OBJ_DIR)/*.o $(LIBRARY)
	rm -f $(ERROR_LOG)

#cleans and rebuilds
rebuild : clean all

#Indicates these are not real files
.PHONY : all clean