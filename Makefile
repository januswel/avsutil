# Makefile for avs2wav

# for avs2wav
avs2wav: avs2wav.o avsutil.o
	g++ -o avs2wav avs2wav.o avsutil.o

avs2wav.o: src/avs2wav.cpp
	g++ -c src/avs2wav.cpp

avsutil.o: src/avsutil.cpp
	g++ -c src/avsutil.cpp

avs2wav.cpp: src/avsutil.h src/avisynth.h
avsutil.cpp: src/avsutil.h src/avisynth.h
