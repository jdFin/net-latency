PROGS=sched_test priority_send priority_recv bulk_send bulk_recv

CFLAGS+=-Wall -O2

LFLAGS+=-lrt

$(echo LINK.c)

all: $(PROGS)

sched_test: sched_test.o
	cc -o $@ $^ -lrt

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
