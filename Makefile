CC=clang
CFLAGS=-std=c11 -Weverything -Werror -Wno-format-nonliteral -Wno-cast-align -Wno-padded
# 'make DEBUG=0' disables debug mode
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS+=-O0 -g
else
    CFLAGS+=-O3 -Wno-disabled-macro-expansion
endif
UNAME := $(shell uname)
# Comments about flags on Darwin vs Linux: 
# https://lwn.net/Articles/590381/
ifeq ($(UNAME), Linux)
	CFLAGS+=-std=c11 -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 
else
	#Including Darwin
	CFLAGS+=-std=c11
endif
# gperftools
# 'make PROFILE=0' disables profiler mode
PROFILE ?= 1
ifeq ($PROFILE), 1)
		LFLAGS=-lprofiler
else
		LFLAGS=
endif
ODIR=target
ADDRINFO_OUT=$(ODIR)/getaddrinfo
HOSTBYNAME_OUT=$(ODIR)/gethostbyname
SDIR=src
_ADDRINFO_OBJS=common.o getaddrinfo.o 
_HOSTBYNAME_OBJS=common.o gethostbyname.o
ADDRINFO_OBJS=$(patsubst %,$(ODIR)/%,$(_ADDRINFO_OBJS))
HOSTBYNAME_OBJS=$(patsubst %,$(ODIR)/%,$(_HOSTBYNAME_OBJS))
PROFOUT=$(ODIR)/prof.out

compile: $(ADDRINFO_OUT) $(HOSTBYNAME_OUT)

all: analyze check

$(ODIR):
	mkdir $(ODIR)

$(ADDRINFO_OUT): $(ADDRINFO_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS) 

$(HOSTBYNAME_OUT): $(HOSTBYNAME_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS) 

$(ODIR)/%.o: $(SDIR)/%.c $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(ODIR)

analyze: 
	scan-build --status-bugs --use-cc=clang make clean build

check: compile
	./tests.py

$(PROFOUT): $(OUT)
	CPUPROFILE=$(PROFOUT) CPUPROFILE_REALTIME=1 $(OUT) ll

profile: $(PROFOUT)

view-profile: $(PROFOUT)
	google-pprof $(OUT) $(PROFOUT)
