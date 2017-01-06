#ifndef LOGGING_H
#define LOGGING_H

//#define DAEMON
//#define TRACE_ON
//#define DEBUG_ON
#define INFO_ON

// image consumer config
#define DISPLAY_GUI true
#define SAVE_IMAGES false

// video streamer config
#define USE_TEST_STREAM TRUE
#define SEND_RTP_STREAM FALSE
#define LOAD_MODULE FALSE
#define NUM_SKIP_FRAMES 10
#define TEST_STREAM "filesrc location=../test-files/matrix.mp4 ! decodebin2 ! video/x-raw-yuv, format=(fourcc)I420, pixel-aspect-ratio=(fraction)1/1 ! ffmpegcolorspace ! "
//#define TEST_STREAM "videotestsrc pattern=smpte ! video/x-raw-yuv, format=(fourcc)I420, width=(int)1280, height=(int)720, framerate=(fraction)30/1, pixel-aspect-ratio=(fraction)1/1 ! "

// module config
#define MODPROBE_PATH "/sbin/modprobe"
#define MODULE_NAME "mxc_v4l2_capture"

// simple logging setup
#ifdef DAEMON
    #include <syslog.h>
    #define OPENLOG openlog(NULL, LOG_CONS | LOG_NOWAIT | LOG_PID, LOG_DAEMON);
    #define CLOSELOG closelog();
    #define ERROR(fmt, ...) syslog(LOG_ERR, fmt, ##__VA_ARGS__);
    #define NOTICE(fmt, ...) syslog(LOG_NOTICE, fmt, ##__VA_ARGS__);
    #ifdef INFO_ON
        #define INFO(fmt, ...) syslog(LOG_INFO, fmt, ##__VA_ARGS__);
    #else
        #define INFO(...)
    #endif
    #ifdef DEBUG_ON
        #define DEBUG(fmt, ...) syslog(LOG_DEBUG, fmt, ##__VA_ARGS__);
    #else
        #define DEBUG(...)
    #endif
    #ifdef TRACE_ON
        #define TRACE(fmt, ...) syslog(LOG_DEBUG, "[TRACE]  " fmt, ##__VA_ARGS__);
    #else
        #define TRACE(...)
    #endif
#else
    #include <stdio.h>
    #define OPENLOG
    #define CLOSELOG
    #define ERROR(fmt, ...) fprintf(stderr, "[ERROR]  " fmt, ##__VA_ARGS__);
    #define NOTICE(fmt, ...) fprintf(stdout, "[NOTICE]  " fmt, ##__VA_ARGS__);
    #ifdef INFO_ON
        #define INFO(fmt, ...) fprintf(stdout, "[INFO]  " fmt, ##__VA_ARGS__);
    #else
        #define INFO(...) do{}while(0)
    #endif
    #ifdef DEBUG_ON
        #define DEBUG(fmt, ...) fprintf(stdout, "[DEBUG]  " fmt, ##__VA_ARGS__);
    #else
        #define DEBUG(...) do{}while(0)
    #endif
    #ifdef TRACE_ON
        #define TRACE(fmt, ...) fprintf(stdout, "[TRACE]  " fmt, ##__VA_ARGS__);
    #else
        #define TRACE(...)
    #endif
#endif


#endif // LOGGING_H
