pyaleph = py2aleph
cflags += $(shell /usr/bin/python2.7-config --cflags)
lflags += $(shell /usr/bin/python2.7-config --ldflags|sed -e s/"-Xlinker -export-dynamic"//)
