#ifndef RUNNER_H
#define RUNNER_H


#include <glib.h>


class Runner
{

public:
    Runner();
    bool init();
    bool iterate();
    bool run();
    void stop();

    bool is_stopped() {return stop_;}
    unsigned long step_count() {return step_count_;}

private:
    virtual bool on_init() {return true;}
    virtual bool on_loop() = 0;
    virtual void on_stop() {return;}

    static gpointer loop(gpointer instance_);

    bool stop_ = false;
    unsigned long step_count_ = 0;
    GThread* thread_ = NULL;

};


#endif // RUNNER_H
