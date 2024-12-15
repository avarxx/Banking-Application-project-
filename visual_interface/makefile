CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror

INCLUDES = -I./include
LIBS = -lQt5Widgets -lQt5Core

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
EXECUTABLE = banking_system

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o $(EXECUTABLE) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)

.PHONY: all clean