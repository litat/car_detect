CXX = g++

CXXFLAGS = -c -Wall $(shell \
	pkg-config --cflags opencv \
	pkg-config --cflags opencv3)
LDFLAGS = $(shell \
	pkg-config --libs opencv \
	pkg-config --libs opencv3)

TARGET_NAME = car_detect
SRC_DIR = .
BIN_DIR = .
EXECUTABLE = $(BIN_DIR)/$(TARGET_NAME)

sources = $(wildcard $(SRC_DIR)/*.cpp)
objects = $(sources:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

all: $(BIN_DIR) $(EXECUTABLE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXECUTABLE): $(objects)
	$(CXX) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

.PHONY: clean print

clean:
	rm -f *.o $(EXECUTABLE)

print:
	echo $(sources)
	echo $(objects)
