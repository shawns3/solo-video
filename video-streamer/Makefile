# optimize for the onboard computer if compiling onboard
UNAME := $(shell uname -m)
ifeq ($(UNAME), armv7l)
ARCHFLAGS = -mthumb -O3 -march=armv7-a -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -mvectorize-with-neon-quad -mfloat-abi=hard
else
ARCHFLAGS =
endif

CC = g++
CFLAGS = -std=c++11 -c -Wall $(ARCHFLAGS)
LDFLAGS =

SOURCES = main.cpp runner.cpp video_streamer.cpp image_consumer.cpp face_detector_image_consumer.cpp
COMMON_DEPS = config.h
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = video-streamer

CFLAGS += $(shell pkg-config --cflags gstreamer-0.10 gstreamer-base-0.10 gstreamer-plugins-base-0.10 gstreamer-app-0.10 gstreamer-video-0.10 glib-2.0 libxml-2.0 glib-2.0 opencv)
LDFLAGS += $(shell pkg-config --libs gstreamer-0.10 gstreamer-base-0.10 gstreamer-plugins-base-0.10 gstreamer-app-0.10 gstreamer-video-0.10 glib-2.0 libxml-2.0 glib-2.0 opencv)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o: $(COMMON_DEPS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
