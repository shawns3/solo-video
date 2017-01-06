#ifndef FACEDETECTORIMAGECONSUMER
#define FACEDETECTORIMAGECONSUMER


#include "image_consumer.h"
#include "opencv2/opencv.hpp"


using namespace cv;


class FaceDetectorImageConsumer : public ImageConsumer {

public:
    FaceDetectorImageConsumer(bool display_gui=false, bool save_images=false);

private:
    bool on_init();
    void on_stop();
    void process_image(Mat& image);

    bool display_gui_ = false;
    bool save_images_ = false;
    unsigned long image_count_ = 0;

    CascadeClassifier cascade_;
    String face_cascade_name_ = "haarcascade_frontalface_alt.xml";
};


#endif // FACEDETECTORIMAGECONSUMER




