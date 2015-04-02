CC = gcc
CFLAGS = -Wall -g
TARGET = minishell
OBJS = parse.o init.o main.o execute.o

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJS):%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	-$(RM) $(OBJS) $(TARGET) core
