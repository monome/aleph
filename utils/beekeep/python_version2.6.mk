pyaleph = py2aleph
cflags += $(shell /usr/bin/python2.6-config --cflags)
lflags += $(shell /usr/bin/python2.6-config --ldflags)
