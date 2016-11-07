FFMPEG_LIBS=    libavdevice                        \
				libavformat                        \
				libavfilter                        \
				libavcodec                         \
				libswresample                      \
				libswscale                         \
				libavutil                          \

FFMPEG_CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
FFMPEG_LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDLIBS := $(shell sdl2-config --libs)

CFLAGS += -Wall -g -fpermissive

CC := g++

decode: decode.o ffsdl.o
	$(CC) -o decode decode.o ffsdl.o $(CFLAGS) $(FFMPEG_LDLIBS) $(SDL_LDLIBS)

decode.o: decode.cpp ffsdl.h
	$(CC) -c $^ $(CFLAGS) $(FFMPEG_CFLAGS) $(SDL_CFLAGS)

ffsdl.o: ffsdl.cpp ffsdl.h
	$(CC) -c $^ $(CFLAGS) $(SDL_CFLAGS)
