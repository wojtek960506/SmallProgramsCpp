#CC specifies which compiler we're using
CC = g++

#flags for compilation
#g++ -std=c++14 -Wall -g -pedantic -Wno-long-long -Werror
COMPILER_FLAGS = -Wall -pedantic -Wextra -g -c -std=c++14

#directory in which I will store the application binary
BUILD_DIR = build

#final executable
TARGET_EXEC = exec

.PHONY: all compile run clean

all: clean compile run

#builds all from the sources
compile: $(BUILD_DIR)/main.o 
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(BUILD_DIR)/main.o -o $(TARGET_EXEC)
	@echo "Code compiled"

run: $(TARGET_EXEC)
	@echo "Execution of code started"
	@./$(TARGET_EXEC)
	@echo "Execution of code finished"

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET_EXEC)
	@echo "All compilation resources have been erased"

#The only file to compile with my full program
$(BUILD_DIR)/main.o: main.cpp
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(COMPILER_FLAGS) main.cpp -o $(BUILD_DIR)/main.o