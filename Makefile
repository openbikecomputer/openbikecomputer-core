BIN = openbikecomputer-core
ROOTDIR ?=
BINDIR ?= usr/bin
CONFDIR ?= etc/openbikecomputer
SHAREDIR ?= usr/share/openbikecomputer

DISPLAY_BACKEND ?= X11

SRC = src/main.c \
      src/log/log.c \
      src/ui/ui.c \
      src/ui/mouse_img.c \
      src/ui/screen/main_screen.c \
      src/ui/screen/data_screen.c \
      src/ui/screen/navigation_screen.c \
      src/ui/screen/profile_screen.c \
      src/ui/screen/rider_screen.c \
      src/ui/screen/bike_screen.c \
      src/ui/screen/results_screen.c \
      src/ui/screen/routes_screen.c \
      src/ui/screen/settings_screen.c \
      src/ui/lvgl_helper.c \
      src/config/obc_config.c \
      src/config/bike_config.c \
      src/config/rider_config.c \
      src/config/system_config.c \
      src/config/user_config.c \
      src/config/libconfig_helper.c \
      src/data/data.c \
      src/data/data_manager.c \
      src/data/data_recorder.c \
      src/utils/simulator.c \
      src/utils/fifo.c \
      src/ui/styles/styles.c \
      src/ui/styles/topbar_styles.c

INCLUDE = -Isrc \
          -Isrc/log \
          -Isrc/ui \
          -Isrc/ui/screen \
          -Isrc/ui/styles \
          -Isrc/data \
          -Isrc/config \
          -Isrc/utils \
          -I$(SYSROOT)/usr/local/include \
          -I$(SYSROOT)/usr/local/include/lvgl \

ifeq ($(DISPLAY_BACKEND), WAYLAND)
LIB_DISPLAY_BACKEND = -llv_drivers -llv-wayland-protocol -lwayland-client -lxkbcommon -lwayland-cursor
INCLUDE += -I$(SYSROOT)/usr/local/include/lvgl/lv_drivers \
           -I$(SYSROOT)/usr/local/include/lvgl/lv_drivers/wayland \
           -I$(SYSROOT)/usr/local/include/lvgl/lv_drivers/wayland/protocols
endif

ifeq ($(DISPLAY_BACKEND), SDL_X11)
LIB_DISPLAY_BACKEND = -lxkbcommon -lSDL2
endif

ifeq ($(DISPLAY_BACKEND), X11)
LIB_DISPLAY_BACKEND = -lxkbcommon -lX11
endif

ifeq ($(DISPLAY_BACKEND), FRAMEBUFFER)
LIB_DISPLAY_BACKEND =
endif

CC ?= gcc
CCLD ?= gcc

CFLAGS += $(INCLUDE) -D_DEFAULT_SOURCE -D_REENTRANT -Wall -Werror -pedantic -std=c99 -DDISPLAY_BACKEND=$(DISPLAY_BACKEND)
LDFLAGS +=
LIBS += -L$(SYSROOT)/usr/lib/ -llvgl $(LIB_DISPLAY_BACKEND) -lpthread -lconfig -lpng

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
	install -d $(ROOTDIR)/$(SHAREDIR)/simulation
	install simulation/*.csv $(ROOTDIR)/$(SHAREDIR)/simulation
	install -d $(ROOTDIR)/$(SHAREDIR)/images
	install images/* $(ROOTDIR)/$(SHAREDIR)/images

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean install
