nohtml: nohtml.c ../include/html_entity.h
	cc -o nohtml nohtml.c

run: nohtml
	./nohtml

install: nohtml
	cp nohtml ../../bin/nohtml

clean:
	rm -f nohtml
