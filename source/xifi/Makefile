xifi: xifi.o nm_wrapper.o
	cc -lncurses xifi.o nm_wrapper.o -o xifi

xifi.o: xifi.c nm_wrapper.h ca.pem.h
	cc -c xifi.c

nm_wrapper.o: nm_wrapper.c nm_wrapper.h
	cc -c nm_wrapper.c


clean:
	rm -f *.o xifi

install: xifi
	cp xifi ../../bin

run: xifi
	./xifi
