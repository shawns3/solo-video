#ifndef IMAGECONSUMER_H
#define IMAGECONSUMER_H


#include <glib.h>
#include "image_receiver.h"
#include "opencv2/opencv.hpp"
#include "runner.h"


using namespace cv;


class ImageConsumer : public Runner, public ImageReceiver
{
public:
    ImageConsumer();
    void new_image(const guint8* data, guint width, guint height);

private:
    virtual bool on_init() {return true;}
    bool on_loop();
    virtual void process_image(Mat& image) = 0;
    virtual void on_stop();

    GMutex mutex_;
    GCond cv_;
    bool new_images_ = false;
    std::deque<cv::Mat> image_queue_;
    unsigned long image_count_ = 0;
};


#endif // IMAGECONSUMER_H
