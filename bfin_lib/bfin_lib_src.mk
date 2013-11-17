bfin_lib_src = control.c \
	dac.c \
	main.c \
	init.c \
	isr.c \
	spi.c \
	util.c

bfin_lib_obj = $(patsubst %.c, %.o, $(bfin_lib_src))

INC += -I$(bfin_lib_srcdir) \
	-I$(bfin_lib_srcdir)/libfixmath \
	-I$(common_dir) \
	-I$(audio_dir) \
	-I$(module_dir)