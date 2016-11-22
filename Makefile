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

CC := clang++

MediaDecoder: MediaDecoder.o ffsdl.o MediaBuffer.o QueueNode.o Queue.o
	$(CC) -o MediaDecoder MediaDecoder.o ffsdl.o $(CFLAGS) $(FFMPEG_LDLIBS) $(SDL_LDLIBS)

MediaDecoder.o: MediaDecoder.cpp ffsdl.h
	$(CC) -c $^ $(CFLAGS) $(FFMPEG_CFLAGS) $(SDL_CFLAGS)

ffsdl.o: ffsdl.cpp ffsdl.h
	$(CC) -c $^ $(CFLAGS) $(SDL_CFLAGS)

MediaBuffer.o: MediaBuffer.cpp MediaBuffer.h
	$(CC) -c $^ $(CFLAGS) $(FFMPEG_CFLAGS) $(SDL_CFLAGS)

QueueNode.o: QueueNode.cpp QueueNode.h
	$(CC) -c $^

Queue.o: Queue.cpp Queue.h
	$(CC) -c $^

clean:
	rm *.o *.gch
