xns: xns.o dnslib.o
	cc -o xns xns.o dnslib.o

xns.o: xns.c ../include/soclib.h dnslib.h
	cc -c xns.c

dnslib.o: dnslib.c dnslib.h
	cc -c dnslib.c

clean:
	rm -f *.o xns

install: xns
	cp xns ../../bin

run: xns
	./xns
