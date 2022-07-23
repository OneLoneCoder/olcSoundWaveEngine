ifeq ($(DEBUG), 1)
	CXXFLAGS=-O0 -g
else
	CXXFLAGS=-O3
endif

LIBS=-lGL -lX11 -lpng -pthread

all: alsa wave openal pulse

hm: HeaderMaker.cpp
	g++ -o hm HeaderMaker.cpp

olcSoundWaveEngine.h: $(wildcard source/*.h source/*.cpp) hm
	(cd source; ../hm olc_swe_template.h ../olcSoundWaveEngine.h)

alsa: TEST_SoundPGEX.cpp olcSoundWaveEngine.h
	g++ $< -o $@ ${CXXFLAGS} ${LIBS} -DSOUNDWAVE_USING_ALSA -lasound

wave: TEST_Wave.cpp olcSoundWaveEngine.h
	g++ $< -o $@ ${CXXFLAGS} ${LIBS} -DSOUNDWAVE_USING_ALSA -lasound

openal: TEST_SoundPGEX.cpp olcSoundWaveEngine.h
	g++ $< -o $@ ${CXXFLAGS} ${LIBS} -DSOUNDWAVE_USING_OPENAL -lopenal

pulse: TEST_SoundPGEX.cpp olcSoundWaveEngine.h
	g++ $< -o $@ ${CXXFLAGS} ${LIBS} -DSOUNDWAVE_USING_PULSE $(shell pkg-config --cflags --libs libpulse-simple)
