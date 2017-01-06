#include <glib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "face_detector_image_consumer.h"
#include "image_consumer.h"
#include "video_streamer.h"


static bool stop = false;
static FaceDetectorImageConsumer image_consumer(DISPLAY_GUI, SAVE_IMAGES);  // TODO: avoid making this a global

static void signal_handler(int sig)
{
    TRACE("signal_handler()\n");
    if (sig == SIGINT) {
        TRACE("signal_handler() -- sig is SIGINT; Stopping\n");
        stop = true;
        image_consumer.stop();
    }
}


#ifdef DAEMON
static void process_exit()
{
    TRACE("Closing log.\n");
    CLOSELOG
    TRACE("Log closed.\n");
}


static void become_daemon(void)
{
    /* In the following, we will create/run as a daemon. To do this, we fork the parent to create a child process.
     * This allows the child process to run the program code without being attached to the shell from which it was launch.
     * This also allows the parent to terminate successfully, and, hence, report back that the daemon was successfully launched.
     */

    pid_t pid;
    int pipefd[2];

    // get the pipe's input/output file descriptors
    if (pipe(pipefd)) {
        fprintf(stderr, "Could not get the pipe file descriptors.\n");
        exit(EXIT_FAILURE);
    }

    // create child process
    pid = fork();  // if successful, both child and parent return from here (with return values 0 and the child's ID, respectively)
    if (pid < (pid_t) 0) {  // pid == -1 signals that fork failed
        fprintf(stderr, "Failed to fork the parent process.\n");
        close(pipefd[0]);
        close(pipefd[1]);
        exit(EXIT_FAILURE);
    }
    else if (pid != (pid_t) 0) {  // this is the parent process -- the pid is the child's pid
        close(pipefd[1]);  // close the pipe's output
        printf("%d\n", pid);  // print out the child's pid
        // FIXME: do I need to clear the input out of the input pipe (ie, read and discard it)?
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }

    // only the child process will make it here
    close(pipefd[0]);  // close the pipe's input
    close(pipefd[1]);  // close the pipe's output

    // setup logging
    umask(0);
    OPENLOG
    atexit(process_exit);

    // create a session
    if (setsid() < (pid_t) 0) {
        ERROR("Could not create a new session.\n");
        exit(EXIT_FAILURE);
    }

    // change to root dir
    if ((chdir("/")) < 0) {
        ERROR("Could not change to root directory.\n");
        exit(EXIT_FAILURE);
    }

    // close standard in/out/err
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
#endif


int main(int argc, char *argv[])
{
#ifdef DAEMON
    // become a daemon (i.e., become an unattached child process)
    become_daemon();
#endif

    // register the signal handler callback
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        ERROR("Could not register the signal handler.\n");
        return EXIT_FAILURE;
    }

    // init the image consumer
    if (!image_consumer.init()) {
        ERROR("Failed to initialize the image consumer.\n");
        return EXIT_FAILURE;
    }

    // init and run the video streamer
    VideoStreamer video_streamer(LOAD_MODULE, USE_TEST_STREAM, SEND_RTP_STREAM, &image_consumer);
    if (!video_streamer.init()) {
        ERROR("Failed to initialize the video streamer.\n");
        return EXIT_FAILURE;
    }
    if (!video_streamer.run()) {
        ERROR("Failed to run the video streamer thread.\n");
        return EXIT_FAILURE;
    }

    // start the main event loop
    DEBUG("Starting the main loop.\n");
    while (!stop) {
        g_main_context_iteration(NULL, FALSE);
        image_consumer.iterate();
    }

    // shut down
    DEBUG("Shutting down.\n");
    TRACE("Calling video_streamer_stop()\n");
    video_streamer.stop();
    TRACE("Calling image_consumer.stop()\n");
    image_consumer.stop();
    DEBUG("Exiting main\n");
    return EXIT_SUCCESS;
}
