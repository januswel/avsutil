# Makefile for avs2wav

# for avs2wav
avs2wav: avs2wav.o avs.o binarywriter.o audiowriter.o wavwriter.o
	g++ -o avs2wav avs2wav.o avs.o binarywriter.o audiowriter.o wavwriter.o

avs2wav.o: src/apps/avs2wav.cpp
	g++ -c src/apps/avs2wav.cpp

avs.o: src/avs.cpp
	g++ -c src/avs.cpp

binarywriter.o: src/binarywriter.cpp
	g++ -c src/binarywriter.cpp

audiowriter.o: src/audiowriter.cpp
	g++ -c src/audiowriter.cpp

wavwriter.o: src/wavwriter.cpp
	g++ -c src/wavwriter.cpp

avs2wav.cpp: src/avsutil.h src/avisynth.h
avs.cpp: src/avsutil.h src/avisynth.h
binarywriter.cpp: src/avsutil.h src/avisynth.h
audiowriter.cpp: src/avsutil.h src/avisynth.h
wavwriter.cpp: src/avsutil.h src/avisynth.h
