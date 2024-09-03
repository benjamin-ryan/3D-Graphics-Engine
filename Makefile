# Variables
CXX = g++
CXXFLAGS = -std=c++17 -I./include
LDFLAGS = -L./lib -lSDL2 -lSDL2main
TARGET = main
SRCS = src/main.cpp src/graphicsEngine.cpp src/fontRenderer.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to build object files
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)