OBJS 	= Manager.o Queue.o myfunctions.o
OBJS2	= Worker.o myfunctions.o
SOURCE	= Manager.c Queue.c myfunctions.c Worker.c
HEADER  = Queue.h myfunctions.h
OUT  	= sniffer
OUT2	= worker
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OUT) $(OUT2)

$(OUT): $(OBJS)
		$(CC) -g $(OBJS) -o $@ 

$(OUT2): $(OBJS2)
		$(CC) -g $(OBJS2) -o $@ 

# create/compile the individual files >>separately<< 
Manager.o: Manager.c
	$(CC) $(FLAGS) Manager.c

Queue.o: Queue.c
	$(CC) $(FLAGS) Queue.c

myfunctions.o: myfunctions.c
	$(CC) $(FLAGS) myfunctions.c

Worker.o: Worker.c
	$(CC) $(FLAGS) Worker.c


# clean house
clean:
	rm -f $(OBJS) $(OUT)
	rm -f $(OBJS2) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
	wc $(SOURCE2) $(HEADER)

