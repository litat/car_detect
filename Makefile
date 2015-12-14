CXX = g++

CXXFLAGS = -c -Wall $(shell \
	pkg-config --cflags opencv)
LDFLAGS = $(shell \
	pkg-config --libs opencv)

TARGET_NAME = car_detect
SRC_DIR = .
BIN_DIR = .
EXECUTABLE = $(BIN_DIR)/$(TARGET_NAME)

sources = $(wildcard $(SRC_DIR)/*.cpp)
objects = $(sources:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

all: $(BIN_DIR) $(EXECUTABLE)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)
	@echo $(BIN_DIR) " created."

$(EXECUTABLE): $(objects)
	@$(CXX) $< -o $@ $(LDFLAGS)
	@echo "Link " $< "successfully."

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CXX) $< -o $@ $(CXXFLAGS)
	@echo "Compile " $< "successfully."

.PHONY: clean

clean:
	rm -f *.o $(EXECUTABLE)
