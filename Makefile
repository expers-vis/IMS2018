#
# Project: IMS 2018
# File: Makefile
# Created on: 30.10.2018
# Authors: Roman Janik (xjanik20)
# 	       Karel Klečka (xkleck04)
#

CC=g++
CPPFLAGS=-Wextra -pedantic -Wall -O2 -std=c++11 -g
LDFLAGS=-lsimlib -lm

.PHONY: all ims

all: ims

ims:
	$(CC) $(CPPFLAGS) -o ims ims.cpp $(LDFLAGS)

run:
	./ims
	./ims -o tricetNavic.out -f 738
	./ims -o tricetNavicBalanced.out -f 738 -1 2 -2 2 -O 2 -h

pack:
	zip 03_xjanik20_xkleck04.zip *.cpp Makefile *.pdf README.md

clean:
	rm -f ims ./*.out
