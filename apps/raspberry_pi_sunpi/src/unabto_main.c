
/**
 *  Implementation of main for uNabto SDK
 */

#include "unabto/unabto_env_base.h"
#include "unabto/unabto_common_main.h"
#include "unabto/unabto_logging.h"
#include "unabto_version.h"
#include "modules/cli/unabto_args.h"
#include <wiringPi.h>


void nabto_yield(int msec);

/**
 *  main using gopt to check command line arguments
 *  -h for help
 */
int main(int argc, char* argv[])
{

    // Set nabto to default values
    nabto_main_setup* nms = unabto_init_context();

#ifdef __arm__

    // Initialize GPIO pins
    // 7,0,2,3
    //(R,G,B,Relay)
    wiringPiSetup();
    pinMode(7, OUTPUT);
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    digitalWrite(2, HIGH);
    digitalWrite(0, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(3, HIGH);

#endif

    // Optionally set alternative url to html device driver
    //nmc.nabtoMainSetup.url = "https://www.dropbox.com/s/8568ylaaen9grz8/html_dd.zip?dl=1";

    // Overwrite default values with command line args
    if (!check_args(argc, argv, nms)) {
        return 1;
    }
    NABTO_LOG_INFO(("Identity: '%s'", nms->id));
    NABTO_LOG_INFO(("Program Release %ul.%ul", RELEASE_MAJOR, RELEASE_MINOR));
    NABTO_LOG_INFO(("Buffer size: %i", nms->bufsize));

    // Initialize nabto
    if (!unabto_init()) {
        NABTO_LOG_FATAL(("Failed at nabto_main_init"));
    }

    // The main loop gives nabto a tick from time to time.
    // Everything else is taken care of behind the scenes.
    while (true) {
        unabto_tick();
        nabto_yield(10);
    }

    unabto_close();
    return 0;
}

void nabto_yield(int msec)
{
#ifdef WIN32
    Sleep(msec);
#elif defined(__MACH__)
    if (msec) usleep(1000*msec);
#else
    if (msec) usleep(1000*msec); else sched_yield();
#endif
}
