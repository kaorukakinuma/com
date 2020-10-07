# Makefile

CC = gcc
CFLAGS = -O2 -Wall

TARGET = a.out

OBJPATH = OBJ
OBJS =\
	$(OBJPATH)/main.o

LIBPATH = /home/lib/
LIBS =\
	-lcom

INCPATH = /home/include/


.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJPATH) $(OBJS)
	$(CC) $(OBJS) -o $(OBJPATH)/$(TARGET) $(LIBS) -L$(LIBPATH)

$(OBJPATH)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCPATH)

$(OBJPATH):
	mkdir $(OBJPATH)

.PHONY: clean
clean:
	rm -rf $(OBJPATH)
