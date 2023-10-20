BIN = openbikecomputer-core
ROOTDIR ?=
BINDIR ?= usr/bin
CONFDIR ?= etc/openbikecomputer

SRC = src/main.c \
      src/log/log.c \
      src/ui/ui.c \
      src/ui/screen/screen_manager.c \
      src/ui/screen/main_screen.c \
      src/ui/screen/bike_conf_screen.c \
      src/ui/screen/rider_conf_screen.c \
      src/ui/screen/system_conf_screen.c \
      src/ui/screen/data_screen.c \
      src/config/bike_config.c \
      src/config/rider_config.c \
      src/config/system_config.c \
      src/config/libconfig_helper.c

INCLUDE = -Isrc \
          -Isrc/log \
          -Isrc/ui \
          -Isrc/ui/screen \
          -Isrc/config \
          -I$(SYSROOT)/usr/include \
          -I$(SYSROOT)/usr/include/lvgl \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland/protocols

CC ?= gcc
CCLD ?= gcc

CFLAGS += $(INCLUDE) -D_DEFAULT_SOURCE -D_REENTRANT -Wall -Werror -pedantic -std=c99
LDFLAGS +=
LIBS += -L$(SYSROOT)/usr/lib/ -llvgl -llv_drivers -llv-wayland-protocol -lwayland-client -lxkbcommon -lwayland-cursor -lpthread -lconfig

OBJS = $(patsubst %.c, %.o, $(SRC))

all: $(BIN)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CCLD) $(LDFLAGS) -o $@  $(OBJS) $(LIBS)

install:
	install -D $(BIN) $(ROOTDIR)/$(BINDIR)/$(BIN)
	install -d $(ROOTDIR)/$(CONFDIR)
	install config/*.conf $(ROOTDIR)/$(CONFDIR)/

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean install
