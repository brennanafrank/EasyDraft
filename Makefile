CC = g++
CFLAGS = -std=c++17 -Wall
SRCS = file_operations.cpp config.cpp TemplateParser.cpp main.cpp KZip.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = run

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) config.json