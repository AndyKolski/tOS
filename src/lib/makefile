LIB_OBJS := \
ctype.o\
stdio.o\
string.o\
printf/printf.o

LIBCFLAGS := -DPRINTF_DISABLE_SUPPORT_FLOAT
LIB_OBJS := $(patsubst %.o,out/obj/lib/%.o,$(LIB_OBJS))