#include <gst/video/video.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "config.h"
#include "video_streamer.h"


VideoStreamer::VideoStreamer(bool load_module, bool use_test_stream, bool send_rtp_stream, ImageReceiver* image_receiver):
    load_module_(load_module),
    use_test_stream_(use_test_stream),
    send_rtp_stream_(send_rtp_stream),
    image_receiver_(image_receiver)
{
}


bool VideoStreamer::on_init()
{
    TRACE("VideoStreamer::on_start()\n");

    /* load module */
    if (load_module_) {
        TRACE("VideoStreamer::on_start() - load module\n");
        if (!VideoStreamer::load_module()) {
            ERROR("Failed to load the module.\n");
            return false;
        }
    }

    /* init gstreamer */
    TRACE("VideoStreamer::on_start() - init gstreamer\n");
    GError* error = NULL;
    if (!gst_init_check(NULL, NULL, &error)) {
        ERROR("Failed to initialize gstreamer.\n");
        return false;
    }

    /* define the video pipeline */
    gchar* source_defn;
    gchar* out_defn;
    gchar* pipeline_defn;
    if (use_test_stream_) {
        source_defn = g_strdup(TEST_STREAM);
    }
    else {
        source_defn = g_strdup("mfw_v4lsrc device=/dev/video0 name=src ! "
                               "mfw_ipucsc ! "
                               "video/x-raw-yuv, format=(fourcc)I420, width=(int)1280, height=(int)720, framerate=(fraction)30/1, pixel-aspect-ratio=(fraction)1/1 ! ");
    }
    if (send_rtp_stream_) {
        out_defn = g_strdup("tee name=t ! "
                            "queue ! "
                            "vpuenc codec=6 bitrate=1800000 gopsize=15 force-framerate=true framerate-nu=25 ! "
                            "rtph264pay config-interval=1 pt=96 ! "
                            "udpsink host=10.1.1.1 port=5550 "
                            "t. ! "
                            "queue ! "
                            "appsink name=appsink sync=true");
    }
    else {
        out_defn = g_strdup("queue ! "
                            "appsink name=appsink sync=true");
    }
    pipeline_defn = g_strconcat(source_defn, out_defn, NULL);
    g_free(source_defn);
    g_free(out_defn);

    /* create the video pipeline */
    INFO("Creating pipeline: %s\n", pipeline_defn);
    pipeline_ = gst_parse_launch(pipeline_defn, &error);
    g_free(pipeline_defn);
    if (error != NULL) {
        ERROR("Could not create pipeline: %s\n", error->message);
        g_error_free(error);
        return false;
    }

    /* configure the appsink */
    GstElement* appsink = gst_bin_get_by_name(GST_BIN(pipeline_), "appsink");
    gst_app_sink_set_emit_signals((GstAppSink*)appsink, TRUE);
    gst_app_sink_set_drop((GstAppSink*)appsink, TRUE);
    gst_app_sink_set_max_buffers((GstAppSink*)appsink, 1);
    GstAppSinkCallbacks callbacks = {NULL, VideoStreamer::new_preroll, VideoStreamer::new_sample};  // TODO: add callback for EOS
    gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks, (gpointer) this, NULL);  // pass a pointer to this instance in the "user data" argument
    gst_object_unref(appsink);

    /* listen for bus messages */
    GstBus* bus;
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
    gst_bus_add_watch(bus, VideoStreamer::new_message, (gpointer) this);
    gst_object_unref(bus);

    /* start the pipeline and send an initial key frame */
    gst_element_set_state(GST_ELEMENT(pipeline_), GST_STATE_PLAYING);
    gst_element_send_event(pipeline_, gst_event_new_custom(GST_EVENT_CUSTOM_UPSTREAM, gst_structure_new("GstForceKeyUnit", "all-headers", G_TYPE_BOOLEAN, TRUE, NULL)));

    TRACE("VideoStreamer::on_start() - done\n");
    return true;
}


bool VideoStreamer::on_loop()
{
    TRACE("VideoStreamer::on_loop()\n");
    // send key frames occasionally to get newly connected clients working
    // TODO: make this a separate function and re-use in the init() function above
    gst_element_send_event(pipeline_, gst_event_new_custom(GST_EVENT_CUSTOM_UPSTREAM, gst_structure_new("GstForceKeyUnit", "all-headers", G_TYPE_BOOLEAN, TRUE, NULL)));
    sleep(1.0);
    return true;
}


void VideoStreamer::on_stop()
{
    TRACE("VideoStreamer::on_stop()\n");
    TRACE("VideoStreamer::on_stop() - stopping pipeline\n");
    gst_element_set_state(GST_ELEMENT(pipeline_), GST_STATE_NULL);
    TRACE("VideoStreamer::on_stop() - unref pipeline\n");
    gst_object_unref(GST_OBJECT(pipeline_));
    TRACE("VideoStreamer::on_stop() - returning\n");
    return;
}


bool VideoStreamer::load_module()
{
    int res = system(MODPROBE_PATH " " MODULE_NAME);
    if (res != 0) {
        ERROR("Could not load module: %d\n", res);
        return false;
    }
    return true;
}


gboolean VideoStreamer::new_message(GstBus* bus, GstMessage* message, gpointer instance_)
{
    // TODO: clean up message handling
    INFO("Received %s message type\n", GST_MESSAGE_TYPE_NAME(message));
    switch(GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR: {
        GError* error;
        gchar* debug;
        gst_message_parse_error(message, &error, &debug);
        ERROR("The message is an error: %s\n", error->message);
        g_error_free(error);
        g_free(debug);
        // TODO: shutdown on error
        break;
    }
    case GST_MESSAGE_EOS:
        ERROR("The message is EOS\n");
        // TODO: shutdown on end of stream
        break;
    default: // ignore other message types
        break;
    }
    return TRUE;  // continue receiving bus messages
}


GstFlowReturn VideoStreamer::new_preroll(GstAppSink* appsink, gpointer instance_)
{
    TRACE("VideoStreamer::new_preroll()\n");
    return GST_FLOW_OK;
}


GstFlowReturn VideoStreamer::new_sample(GstAppSink* appsink, gpointer instance_)
{
    TRACE("VideoStreamer::new_sample()\n");
    VideoStreamer* instance = (VideoStreamer*) instance_;

    GstBuffer* buffer = gst_app_sink_pull_buffer(appsink);
    if (buffer) {

        if ((instance->sample_count_ % NUM_SKIP_FRAMES == 0) || (instance->sample_count_ == 0)) {
            guint8* sample = GST_BUFFER_DATA(buffer);
            GstCaps* caps = GST_BUFFER_CAPS(buffer);
            int width, height;
            GstVideoFormat format;
            if (!gst_video_format_parse_caps(caps, &format, &width, &height)) {
                ERROR("Could not parse video capabilities.\n");
                // TODO: stop operation
            }

#ifdef DEBUG_ON
            if (instance->sample_count_ % 30 == 0) {  // TODO: make sure these bits always run reguardless of value of NUM_SKIP_FRAMES
                DEBUG("VideoStreamer sample count: %lu\n", instance->sample_count_);
            }
            if (instance->sample_count_ == 1) {
                DEBUG("Capabilities: %s\n", gst_caps_to_string(caps));
                DEBUG("Width = %i, Height = %i\n, Size = %i", width, height, GST_BUFFER_SIZE(buffer));
            }
#endif

            instance->image_receiver_->new_image(sample, width, height);
        }
        ++(instance->sample_count_);
    }
    gst_buffer_unref(buffer);
    return GST_FLOW_OK;
}
