

.PHONY: all linux emscripten clean

all: linux emscripten

linux:
	g++ ../source/swe_synth_envelope.cpp \
		../source/swe_system_alsa.cpp \
		../source/swe_system_openal.cpp \
		../source/swe_system_wasapi.cpp \
		../source/swe_wave_engine.cpp \
		../source/swe_wave_wave.cpp \
		../source/swe_synth_modular.cpp \
		../source/swe_system_base.cpp \
		../source/swe_system_pulse.cpp \
		../source/swe_system_winmm.cpp \
		../source/swe_wave_file.cpp \
		../source/swe_synth_osc.cpp \
		../source/swe_system_miniaudio.cpp \
		../source/swe_system_sdlmixer.cpp \
		../source/swe_wave_view.cpp \
		Test_MiniAudio.cpp \
		-o Test_MiniAudio \
		-DSOUNDWAVE_USING_MINIAUDIO \
		-ldl -lm -lX11 -lGL -lpthread -lpng -lstdc++fs \
		-std=c++17 -I../ -I../source -I../demos

emscripten:
	em++ ../source/swe_synth_envelope.cpp \
		../source/swe_system_alsa.cpp \
		../source/swe_system_openal.cpp \
		../source/swe_system_wasapi.cpp \
		../source/swe_wave_engine.cpp \
		../source/swe_wave_wave.cpp \
		../source/swe_synth_modular.cpp \
		../source/swe_system_base.cpp \
		../source/swe_system_pulse.cpp \
		../source/swe_system_winmm.cpp \
		../source/swe_wave_file.cpp \
		../source/swe_synth_osc.cpp \
		../source/swe_system_miniaudio.cpp \
		../source/swe_system_sdlmixer.cpp \
		../source/swe_wave_view.cpp \
		Test_MiniAudio.cpp \
		-o Test_MiniAudio.html \
		-DSOUNDWAVE_USING_MINIAUDIO \
		-std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1 \
		-I../ -I../demos -I../source --preload-file=../demos/assets

clean: 
	@rm -f Test_MiniAudio Test_MiniAudio.data Test_MiniAudio.html Test_MiniAudio.wasm Test_MiniAudio.js
