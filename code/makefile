CFLAGS=-w -O2 --static -s
COMPILER=gcc

all: kepler

kepler:
	$(COMPILER) $(CFLAGS) src/kepler.c -o build/kepler -lm

clean:
	rm -f *.o

.PHONY: all vuln patch clean
