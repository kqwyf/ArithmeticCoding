BUILD_DIR = ./build

all: $(BUILD_DIR)
	g++ main.cpp arithmetic.cpp utils.cpp -o $(BUILD_DIR)/main

$(BUILD_DIR):
	mkdir $(BUILD_DIR)
