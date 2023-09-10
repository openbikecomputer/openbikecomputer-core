BIN = openbikecomputer-core
DESTDIR ?= /usr/bin

SRC = src/main.c \

INCLUDE = -Isrc \

CC ?= gcc
CCLD ?= gcc

CFLAGS += $(INCLUDE)
LDFLAGS += -lwayland-client -lxkbcommon -lwayland-cursor -llvgl

OBJS = $(patsubst %.c, %.o, $(CFILES))

all: $(BIN)

%.o : %.c
	echo "BUILD $(CC) -c $(CFLAGS) $< -o $@"
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN): $(OBJS)
	$(CCLD) $(OBJS) -o $@ $(LDFLAGS)

install:
	install -D $(BIN) $(DESTDIR)/$(BIN)

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean
