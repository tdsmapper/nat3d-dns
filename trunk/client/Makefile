CC = gcc
CFLAGS = -DDEBUG
PROG = addrchange 
OBJS = httpinform.o externalip.o

all: $(PROG)

.c.o:
	$(CC) $(CFLAGS)  -c $<

$(PROG): $(OBJS)
	$(CC) $(CFLAGS)  -o $@ $@.c $(OBJS)
    
clean:
	rm -f $(PROG) $(OBJS)

