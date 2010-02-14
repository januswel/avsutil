# Makefile for avs2wav

# for avs2wav
avs2wav: avs2wav.o avsutil.o
	g++ -o avs2wav avs2wav.o avsutil.o

avs2wav.o: avs2wav.cpp
	g++ -c avs2wav.cpp

avsutil.o: avsutil.cpp
	g++ -c avsutil.cpp

avs2wav.cpp: avsutil.h avisynth.h
avsutil.cpp: avsutil.h avisynth.h
