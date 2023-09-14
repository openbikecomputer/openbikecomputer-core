BIN = openbikecomputer-core
DESTDIR ?= /usr/bin

SRC = src/main.c \

INCLUDE = -Isrc \
          -I$(SYSROOT)/usr/include \
          -I$(SYSROOT)/usr/include/lvgl \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland/protocols

CC ?= gcc
CCLD ?= gcc

CFLAGS += $(INCLUDE) -D_REENTRANT
LDFLAGS +=
LIBS += -L$(SYSROOT)/usr/lib/ -llvgl -llv_drivers -llv-wayland-protocol -lwayland-client -lxkbcommon -lwayland-cursor -lpthread

OBJS = $(patsubst %.c, %.o, $(SRC))

all: $(BIN)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CCLD) $(LDFLAGS) -o $@  $(OBJS) $(LIBS)

install:
	install -D $(BIN) $(DESTDIR)/$(BIN)

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean install
