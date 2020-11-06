# Required on debian/ubuntu: gcc swig python-dev libgd2-noxpm libgd2-noxpm-dev python-imaging-dev
# ...or libgd2-xpm libgd2-xpm-dev 

_imgtools.so: imgtools.c
	swig -python imgtools.i
	gcc -c imgtools.c imgtools_wrap.c -I/usr/include/python2.7 -fPIC
	ld -shared imgtools.o imgtools_wrap.o -o _imgtools.so -lgd

clean:
	rm imgtools.o imgtools.py _imgtools.so imgtools_wrap.c imgtools_wrap.o

install:
	cp imgtools.py /usr/lib/python2.7/
	cp _imgtools.so /usr/lib/python2.7/

uninstall:
	rm /usr/lib/python2.7/imgtools.py
	rm /usr/lib/python2.7/_imgtools.so
