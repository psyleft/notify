SHELL := /bin/sh
CC := gcc

CFLAGS := -Wall -Wextra -ggdb $(shell pkg-config --cflags guile-3.0 libcurl)
LIBS := $(shell pkg-config --libs guile-3.0 libcurl)

OUT := notify
SRC := src/main.c

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(LIBS) \
	-I. \
	-DGUILE_DIR='"$(PWD)/guile/"' \
	-o $(OUT) $(SRC)
