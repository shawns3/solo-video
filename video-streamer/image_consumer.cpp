#include <sys/time.h>
#include <unistd.h>
#include "config.h"
#include "image_consumer.h"


using namespace cv;


ImageConsumer::ImageConsumer():
    mutex_(),
    cv_(),
    new_images_(false),
    image_queue_(),
    image_count_(0)
{
}


void ImageConsumer::on_stop()
{
    TRACE("ImageConsumer::on_stop()\n");
    g_cond_signal(&cv_);
    TRACE("ImageConsumer::on_stop() - done\n");
}


void ImageConsumer::new_image(const guint8* data, guint width, guint height)
{
    TRACE("ImageConsumer::new_image()\n");
    g_mutex_lock(&mutex_);
    TRACE("ImageConsumer::new_image() - got mutex\n");
    ++image_count_;
#ifdef DEBUG_ON
    if (image_count_ % 30 == 0) {
        DEBUG("IC image count: %lu\n", image_count_);
    }
#endif

    TRACE("ImageConsumer::new_image() - adding image to queue\n");
    Mat image = Mat(Size(width, (int) height*1.5), CV_8UC1, const_cast<unsigned char *>(data)).clone();
    image_queue_.push_back(image);
    new_images_ = true;
    TRACE("ImageConsumer::new_image() - signaling condition variable\n");
    g_cond_signal(&cv_);
    TRACE("ImageConsumer::new_image() - unlocking mutex\n");
    g_mutex_unlock(&mutex_);
    TRACE("ImageConsumer::new_image() - done\n");
}


bool ImageConsumer::on_loop()
{
    TRACE("ImageConsumer::on_loop()\n");

    // wait for the queue to have images
    TRACE("ImageConsumer::on_loop() - try to get mutex lock\n");
    g_mutex_lock(&mutex_);
    TRACE("ImageConsumer::on_loop() - got mutex lock\n");
    while ((image_queue_.size() == 0) and (!is_stopped())) {
        TRACE("ImageConsumer::on_loop() - wait on cond\n");
        g_cond_wait(&cv_, &mutex_);
        TRACE("ImageConsumer::on_loop() - done waiting\n");
    }
    TRACE("ImageConsumer::on_loop() - got lock and cond\n");
    TRACE("ImageConsumer::on_loop() - image queue size: %lu\n", image_queue_.size());

    if (is_stopped()) {
        TRACE("ImageConsumer::on_loop() - stop is shown as set; breaking out\n");
        return true;
    }

    // TODO: remove timing calculations
    struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
    TRACE("ImageConsumer::on_loop() - getting most recent image\n");
    Mat tmp = image_queue_.front();
    Mat orig = Mat(Size(1280, 720), CV_8UC3);
    TRACE("ImageConsumer::on_loop() - converting color space\n");
    cvtColor(tmp, orig, COLOR_YUV2BGR_I420);
    TRACE("ImageConsumer::on_loop() - clearing image queue\n");
    image_queue_.clear();
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
    TRACE("color conversion: %f\n", elapsedTime);
#ifdef DEBUG_ON
    if (step_count() % 5 == 0) {
        DEBUG("color conversion: %f\n", elapsedTime);
    }
#endif
    TRACE("ImageConsumer::on_loop() - unlocking mutex\n");
    g_mutex_unlock(&mutex_);

    TRACE("ImageConsumer::on_loop() - calling process_image()\n");
    process_image(orig);
    TRACE("ImageConsumer::on_loop() - loop done\n");
    return true;
}
