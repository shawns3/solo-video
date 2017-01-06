#include <glib.h>
#include <glib/gprintf.h>
#include <sys/time.h>
#include <unistd.h>

#include "config.h"
#include "face_detector_image_consumer.h"


using namespace cv;


FaceDetectorImageConsumer::FaceDetectorImageConsumer(bool display_gui, bool save_images):
    display_gui_(display_gui),
    save_images_(save_images),
    image_count_(0)
{
}


bool FaceDetectorImageConsumer::on_init()
{
    TRACE("FaceDetectorImageConsumer::on_start()\n");

    if(!cascade_.load(face_cascade_name_)) {
        ERROR("Error loading face cascade classifier.\n");
        return false;
    }

    if (display_gui_) {
        namedWindow("original",1);
        namedWindow("processed",1);
    }
    return true;
}


void FaceDetectorImageConsumer::on_stop()
{
    //cvReleaseHaarClassifierCascade(&&cascade_); // TODO
}


void FaceDetectorImageConsumer::process_image(Mat& image)
{
    TRACE("FaceDetectorImageConsumer::process_image()\n");
    // TODO: remove timing calculations
    struct timeval t1, t2;
    double elapsedTime = 0;
    gettimeofday(&t1, NULL);
    Mat mod;
    cvtColor(image, mod, CV_RGB2GRAY);
    std::vector<Rect> faces;
    equalizeHist(mod, mod);
    cascade_.detectMultiScale(mod, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
    for(size_t i = 0; i < faces.size(); i++) {
        rectangle(mod, faces[i], Scalar( 255, 0, 255 ), 4, 8, 0);
    }
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
    TRACE("FaceDetectorImageConsumer::process_image() - image processing - elasped time: %f ms\n", elapsedTime);
#ifdef DEBUG_ON
        if (image_count_ % 5 == 0) {
            DEBUG("FaceDetectorImageConsumer::process_image() - image processing - elasped time: %f ms\n", elapsedTime);
        }
#endif
    if (display_gui_) {
        TRACE("FaceDetectorImageConsumer::process_image() - displaying images\n");
        imshow("processed", mod);
        imshow("original", image);
        TRACE("FaceDetectorImageConsumer::process_image() - done displaying images; sleeping\n");
        waitKey(1);
        TRACE("FaceDetectorImageConsumer::process_image() - done sleeping\n");
    }
    if (save_images_) {
        TRACE("FaceDetectorImageConsumer::process_image() - saving images\n");
        gchar origFilename[100];
        g_sprintf(origFilename, "images/original-%lu.jpg", image_count_);  // TODO: use std cpp libs to do this and get rid of glib includes
        imwrite(origFilename, image);
        gchar modFilename[100];
        g_sprintf(modFilename, "images/mod-%lu.jpg", image_count_);
        imwrite(modFilename, mod);
        TRACE("FaceDetectorImageConsumer::process_image() - done saving images\n");
    }
    ++image_count_;
    TRACE("FaceDetectorImageConsumer::process_image() - done\n");
}
