CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = assembler

all: $(TARGET)

$(TARGET): main.cpp structures.cpp pass1.cpp pass2.cpp display.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) intermediate.txt symbol_table.txt literal_table.txt output.txt

.PHONY: all run clean
