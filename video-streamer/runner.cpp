#include"config.h"
#include "runner.h"


Runner::Runner()
{
}


bool Runner::init()
{
    TRACE("Runner::init()\n");

    TRACE("Runner::init() - calling on_init()\n");
    if (!on_init()) {
        ERROR("Could not initialize\n");
        return false;
    }
    return true;
}


bool Runner::iterate()
{
    TRACE("Runner::iterate()\n");
    if ((thread_ == NULL) && (!stop_)) {
        TRACE("Runner::iterate() - loop step\n");
        if (!on_loop()) {
            ERROR("Failed to step");
            return false;
        }
        ++step_count_;
        return true;
    }
    return false;
}


bool Runner::run()
{
    TRACE("Runner::run()\n");
    if (thread_ == NULL) {
        GError* error = NULL;
        TRACE("Runner::run() - creating thread\n");
        thread_ = g_thread_try_new(NULL, Runner::loop, (gpointer) this, &error);
        if (error != NULL) {
            ERROR("Could not start thread: %s\n", error->message);
            g_error_free(error);
            return false;
        }
        TRACE("Runner::run() - returning\n");
        return true;
    }
    TRACE("Runner::run() - returning\n");
    return false;
}


void Runner::stop()
{
    TRACE("Runner::stop()\n");
    stop_ = true;
    TRACE("Runner::stop() - calling on_stop()\n");
    on_stop();
    if (thread_ != NULL) {
        TRACE("Runner::stop() - joining thread\n");
        g_thread_join(thread_);
        thread_ = NULL;
    }
}


gpointer Runner::loop(gpointer instance_)
{
    TRACE("Runner::loop()\n");
    Runner* instance = (Runner*) instance_;
    while (!instance->stop_) {
        TRACE("Runner::loop() - loop step\n");
        if (!instance->on_loop()) {
            ERROR("Failed to step");
            break;
        }
        ++instance->step_count_;
    }
    return NULL;
}
