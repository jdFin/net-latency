PROGS=priority_send priority_recv bulk_send bulk_recv

CFLAGS+=-Wall -O2

all: $(PROGS)

priority_send: priority_send.o options.o
	cc -o $@ $^

priority_recv: priority_recv.o options.o
	cc -o $@ $^

bulk_send: bulk_send.o options.o
	cc -o $@ $^

bulk_recv: bulk_recv.o options.o
	cc -o $@ $^

clean:
	rm -f $(PROGS) *.o
