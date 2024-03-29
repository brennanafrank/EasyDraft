CC = g++
CFLAGS = -std=c++17 -Wall -Werror
SRCS = config.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = run

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(MINIZIP_OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MINIZIP_OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(MINIZIP_OBJS) $(TARGET) config.json