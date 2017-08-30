SRC = $(wildcard *.c)

CPPFLAGS += 

LDFLAGS += 
LDFLAGS +=
LDFLAGS +=

BUG += -DDEBUG

#CROSS = arm-none-linux-gnueabi-
CC = $(CROSS)gcc

cloud.out:$(SRC)
	$(CC) $^ -o $@ $(CPPFLAGS) $(LDFLAGS)

debug.out:$(SRC)
	$(CC) $^ -o $@ $(CPPFLAGS) $(LDFLAGS) $(BUG)

clean:
	rm *.out
