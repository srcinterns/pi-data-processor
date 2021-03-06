#
# Makefile graciously borrowed from http://github.com/richteer/iiag,
# 	courtesy of sjrct.
#
# NOTE: Please do not push updates to this file unless it is meant
#   to be a permanent change!

SHELL  = bash
TARGET = processor
CC     = gcc
CCFL   = -c -g -Wall
LDFL   = -Wall -lm -lfftw3 #-lasound

SRCS := main.c network.c rti.c dbv.c ifft_wrapper.c utility.c debug_print.c #alsa.c

OBJS := $(addprefix obj/,$(patsubst %.c,%.o,$(SRCS)))
DEPS := $(addprefix dep/,$(patsubst %.c,%.d,$(SRCS)))
SRCS := $(addprefix src/,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFL) -o $(TARGET)

obj/%.o: src/%.c
	@ mkdir -p $(@D)
	@ mkdir -p $(subst obj,dep,$(@D))
	@ $(CC) -MM -MP -MT $@ -MF $(patsubst %.o,%.d,$(subst obj,dep,$@)) $<
	$(CC) $(CCFL) $< -o $@

clean:
	rm -rf obj
	rm -rf dep
	rm -f $(TARGET)

-include $(DEPS)
