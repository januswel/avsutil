# Makefile for avs2wav

# for avs2wav
avs2wav: avs2wav.o avsutil.o filewriter.o audiowriter.o wavwriter.o
	g++ -o avs2wav avs2wav.o avsutil.o filewriter.o audiowriter.o wavwriter.o

avs2wav.o: src/apps/avs2wav.cpp
	g++ -c src/apps/avs2wav.cpp

avsutil.o: src/avsutil.cpp
	g++ -c src/avsutil.cpp

filewriter.o: src/filewriter.cpp
	g++ -c src/filewriter.cpp

audiowriter.o: src/audiowriter.cpp
	g++ -c src/audiowriter.cpp

wavwriter.o: src/wavwriter.cpp
	g++ -c src/wavwriter.cpp

avs2wav.cpp: src/avsutil.h src/avisynth.h
avsutil.cpp: src/avsutil.h src/avisynth.h
filewriter.cpp: src/avsutil.h src/avisynth.h
audiowriter.cpp: src/avsutil.h src/avisynth.h
wavwriter.cpp: src/avsutil.h src/avisynth.h
