# Variables
CXX = g++
CXXFLAGS = -I/opt/homebrew/opt/sdl2/include
LDFLAGS = -L/opt/homebrew/opt/sdl2/lib -lSDL2
TARGET = main
SRCS = main.cpp graphicsEngine.cpp
OBJS = $(SRCS:.cpp=.o)
STD = -std=c++17

# Default rule
all: $(TARGET)

# Linking rule
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(STD) -o $(TARGET)

# Compilation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(STD) -c $< -o $@

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)
