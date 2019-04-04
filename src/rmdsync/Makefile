RM_INCLUDE_DIR=$(shell pwd)
DEBUGFLAGS = -g -ggdb -O2
INCLUDE_DIRS = -I"$(RM_INCLUDE_DIR)"
CFLAGS = $(INCLUDE_DIRS) -Wall -Werror $(DEBUGFLAGS) -fPIC -std=gnu99  -D_GNU_SOURCE
CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')

SHOBJ_CFLAGS ?=  -fno-common -g -ggdb
SHOBJ_LDFLAGS ?= -shared -Bsymbolic -Bsymbolic-functions

export CFLAGS
export DEBUGFLAGS

SOURCEDIR=$(shell pwd -P)
CC_SOURCES = $(wildcard $(SOURCEDIR)/*.c)
CC_OBJECTS = $(patsubst $(SOURCEDIR)/%.c, $(SOURCEDIR)/%.o, $(CC_SOURCES))

MODULE=rmdsync.so


all: $(MODULE)

$(MODULE): $(CC_OBJECTS)
	$(LD) -o $@ $(CC_OBJECTS) $(LIBS) $(SHOBJ_LDFLAGS) -lc -lm

clean:
	rm -rvf *.so *.o
