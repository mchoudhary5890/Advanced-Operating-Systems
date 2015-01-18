OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g -Wall 
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

MPICC = mpicc
MPICH = /opt/openmpi-1.4.3-gcc44/bin/mpicc
#CFLAGS = -I$(MPICH)/include
CFLAGS = -I$(MPICH)

all: centralMPI diss merge mcs central

#diss: diss.o
#	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPFLAGS) $(OMPLIBS) -lm


diss: dissemination.c
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPFLAGS) $(OMPLIBS) -lm
central: central.c
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPFLAGS) $(OMPLIBS) -lm

mcs: mcs.c
	$(MPICH) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPLIBS)

merge: merge.c
	$(MPICH) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPLIBS) $(OMPFLAGS)

centralMPI: centralMPI.c
	$(MPICH) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPLIBS)

clean:
	rm -f *.o central centralMPI diss merge mcs

