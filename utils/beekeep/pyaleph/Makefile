all: pyaleph.so

src += pyaleph.c

#obj = $(addprefix $(build-dir), $(addsuffix .o,$(basename $(src))))

cflags += $(shell /usr/local/bin/python-config --cflags)

#%.o: %.c
#	echo $(cflags)
#	$(Q)test -d $(dir $@) || mkdir -p $(dir $@)
#	gcc $(cflags) -g -c $< -o $@

clean:
	rm pyaleph.so pyaleph.pyc

pyaleph.so: pyaleph.c
	gcc $(cflags) -shared -Wl,-soname,pyaleph -o pyaleph.so -fPIC pyaleph.c
