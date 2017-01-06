#ifndef IMAGERECEIVER_H
#define IMAGERECEIVER_H


#include <glib.h>


class ImageReceiver
{

public:
    virtual void new_image(const guint8* data, guint width, guint height) = 0;

};


#endif // IMAGERECEIVER_H
