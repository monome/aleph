pyaleph = py3aleph
cflags += $(shell /usr/bin/python3.1-config --cflags)
lflags += $(shell /usr/bin/python3.1-config --ldflags)
