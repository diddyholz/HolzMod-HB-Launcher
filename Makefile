ifndef CAS_SDK
$(error You have to define the CAS_SDK variable and point it to the cas-sdk directory!)
endif

OUTFILE = launcher
APPNAME = HolzModLauncher
APPAUTHOR = diddyholz
APPVERSION = v0.05

CC = sh-elf-gcc
BINCOPY = sh-elf-objcopy
PYTHON = python

SOURCEDIR = src
BUILDDIR = obj

CFLAGS = -mrenesas -mb -m4-nofpu -O2 -ffreestanding -nostartfiles -I $(CAS_SDK)/include -I $(PREFIX)/sh-elf/include -L$(PREFIX)/sh-elf/lib/
LDFLAGS = -T linker-script.ld
BCOPYFLAGS = -R .got*

SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

all: $(OUTFILE).tmp
	$(BINCOPY) -O binary $(OUTFILE).elf $(OUTFILE).bin

$(OUTFILE).tmp: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(addprefix $(BUILDDIR)/,$(notdir $(OBJECTS))) -o $(OUTFILE).elf $(CAS_SDK)/cas-sdk.o

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(SOURCEDIR) -I$(dir $<) -c $< -o $(BUILDDIR)/$(notdir $@)

.PHONY: all