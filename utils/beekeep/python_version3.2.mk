pyaleph = py3aleph
cflags += $(shell /usr/bin/python3.2-config --cflags)
lflags += $(shell /usr/bin/python3.2-config --ldflags|sed -e s/"-Xlinker -export-dynamic"//)
