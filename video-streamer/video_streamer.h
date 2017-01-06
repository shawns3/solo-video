/*
 * Gstreamer-based video pipeline that consumes a v4l2 device and both produces an H264/RTP video stream and provides a callback for processing individual images.
 *
 */
#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H


#include <glib.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include "image_receiver.h"
#include "runner.h"


class VideoStreamer : public Runner
{

public:
    VideoStreamer(bool load_module, bool use_test_stream, bool send_rtp_stream, ImageReceiver* image_receiver);

private:
    /* threaded runner */
    bool on_init();
    bool on_loop();
    void on_stop();

    /* gstreamer video pipeline callbacks */
    static gboolean new_message(GstBus* bus, GstMessage* message, gpointer instance_);
    static GstFlowReturn new_preroll(GstAppSink* appsink, gpointer instance_);
    static GstFlowReturn new_sample(GstAppSink* appsink, gpointer instance_);

    /* utility functions */
    static bool load_module();

    /* user params */
    bool load_module_;
    bool use_test_stream_;
    bool send_rtp_stream_;
    ImageReceiver* image_receiver_;

    /* internal data */
    GstElement* pipeline_;
    unsigned long sample_count_;
};


#endif // VIDEOSTREAMER_H
