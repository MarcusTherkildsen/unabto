/**
 *  uNabto application logic implementation
 */

#include "unabto/unabto_app.h"
#include <stdio.h>
#include <pthread.h>

#include "c_serial.h"

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h> //memset

#include <features.h>

#include <fcntl.h>
 
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/ioctl.h>

// Function prototypes
uint8_t setLight(uint8_t id, uint8_t onOff);
uint8_t readLight(uint8_t id);

// The virtual light bulb variable
static uint8_t theLight = 0;

// Initialize the other thread
pthread_t thread1;
const char *message1 = "Thread 1";
int iret1;
int i;
void *print_message_function( void *ptr );

// sendOrder prototype
void* sendOrder(void *arg);

// serial socket
int fd = 0;

/***************** The uNabto application logic *****************
 * This is where the user implements his/her own functionality
 * to the device. When a Nabto message is received, this function
 * gets called with the message's request id and parameters.
 * Afterwards a user defined message can be sent back to the
 * requesting browser.
 ****************************************************************/
application_event_result demo_application(application_request* request, buffer_read_t* read_buffer, buffer_write_t* write_buffer) {
    switch(request->queryId) {
        case 1: {
            //  <query name="light_write.json" description="Turn light on and off" id="1">
            //    <request>
            //      <parameter name="light_id" type="uint8"/>
            //      <parameter name="light_on" type="uint8"/>
            //    </request>
            //    <response>
            //      <parameter name="light_state" type="uint8"/>
            //    </response>
            //  </query>

            uint8_t light_id;
            uint8_t light_on;
            uint8_t light_state;

            // Read parameters in request
            if (!buffer_read_uint8(read_buffer, &light_id)) {
                NABTO_LOG_ERROR(("Can't read light_id, the buffer is too small"));
                return AER_REQ_TOO_SMALL;
            }
            if (!buffer_read_uint8(read_buffer, &light_on)) {
                NABTO_LOG_ERROR(("Can't read light_state, the buffer is too small"));
                return AER_REQ_TOO_SMALL;
            }

            // Set light according to request
            light_state = setLight(light_id, light_on);

            // Write back led state
            if (!buffer_write_uint8(write_buffer, light_state)) {
                return AER_REQ_RSP_TOO_LARGE;
            }

            return AER_REQ_RESPONSE_READY;
        }
        case 2: {
            //  <query name="light_read.json" description="Read light status" id="2">
            //    <request>
            //      <parameter name="light_id" type="uint8"/>
            //    </request>
            //    <response>
            //      <parameter name="light_state" type="uint8"/>
            //    </response>
            //  </query>

            uint8_t light_id;
            uint8_t light_state;

            // Read parameters in request
            if (!buffer_read_uint8(read_buffer, &light_id)) return AER_REQ_TOO_SMALL;

            // Read light state
            light_state = readLight(light_id);

            // Write back led state
            if (!buffer_write_uint8(write_buffer, light_state)) return AER_REQ_RSP_TOO_LARGE;

            return AER_REQ_RESPONSE_READY;
        }
    }
    return AER_REQ_INV_QUERY_ID;
}

/* This is our thread function.  It is like main(), but for a thread */
void* sendOrder(void *arg)
{

    // Set thread to be detachable such that it will release resources when finished.
    pthread_detach(pthread_self());

    printf("Now running thread\n");

    // Get the passed item/integer
    fd = *((int *) arg);

    // Wake Roomba
    setRTS(fd, 0);
    usleep(100000); //0.1 sec
    setRTS(fd, 1);
    sleep(2);

    // The trick is to send char arrays

    // Put into safe mode
    char safe[] = {128, 131};
    write(fd, &safe, sizeof(safe));
    usleep(20*1000); // When changing mode, allow 20 milliseconds

    // Set display to NAb T/O
    char display[] = {163, 55, 119, 124, 93};
    write(fd, &display, sizeof(display));
    usleep ((sizeof(display)+25) * 100);

    // Start clean cycle
    //char clean[] = {135};
    //write(fd, &clean, sizeof(clean));
    //usleep ((sizeof(clean)+25) * 100);


    return NULL;
}

// Set virtual light and return state,
// only using ID #1 in this simple example
uint8_t setLight(uint8_t id, uint8_t onOff) {
    theLight = onOff;
    //NABTO_LOG_INFO((theLight?("Nabto: Light turned ON!"):("Nabto: Light turned OFF!")));


    NABTO_LOG_INFO(("Key pressed%d, onOff%d\n", id, onOff));

    // W - drive straight
    if (id == 87)
    {
        //char w[] = {137, 0, 100, 128, 0}; // char w[] = {137, 0, 50, 128, 0};
        char w[] = {137, 0, 255, 128, 0};
        write(fd, &w, sizeof(w));
        usleep ((sizeof(w)+25) * 100);      
    }
    // S
    else if (id == 83){

        // Play reverse sound
        char reverse[] = {141, 1};
        write(fd, &reverse, sizeof(reverse));
        usleep ((sizeof(reverse)+25) * 100);

        //char s[] = {137, 255, 155, 128, 0}; // char s[] = {137, 255, 235, 128, 0};
        char s[] = {137, 255, 0, 128, 0};
        write(fd, &s, sizeof(s));
        usleep ((sizeof(s)+25) * 100); 
    }
    // A
    else if (id == 65){
        //char a[] = {137, 0, 100, 0, 1};
        char a[] = {137, 0, 255, 0, 1};
        write(fd, &a, sizeof(a));
        usleep ((sizeof(a)+25) * 100); 
    }
    // D
    else if (id == 68){
        //char d[] = {137, 0, 100, 255, 255};
        char d[] = {137, 0, 255, 255, 255};
        write(fd, &d, sizeof(d));
        usleep ((sizeof(d)+25) * 100); 
    }
    // K - DSB
    else if(id == 75){
        char DSB[] = {141, 0};
        write(fd, &DSB, sizeof(DSB));
        usleep ((sizeof(DSB)+25) * 100);
    }
    // L - March Kwai
    else if(id == 76){
        char MARCH[] = {141, 3};
        write(fd, &MARCH, sizeof(MARCH));
        usleep ((sizeof(MARCH)+25) * 100);
    }
    // Wake up Roomba by pressing P
    else if(id == 80)
    {
        // Preparing the integer to pass to the thread
        int *theOrder = malloc(sizeof(*theOrder));
        *theOrder = fd;

        // Send order according to beverage_id
        iret1 = pthread_create( &thread1, NULL, &sendOrder, theOrder);
        if(iret1)
        {
            // Something went wrong with the thread
            NABTO_LOG_INFO(("Good old error!\n"));
        }
        else
        {
            // Succesfully sent the command to the thread. 
            NABTO_LOG_INFO(("Success!\n"));
        }
    }
    // Stop driving if any other key is pressed
    else{
        char stop_driving[] = {137, 0, 0, 0, 0};
        write(fd, &stop_driving, sizeof(stop_driving));
        usleep ((sizeof(stop_driving)+25) * 100); 
    }

    return theLight;
}

// Return virtual light state,
// only using ID #1 in this simple example
uint8_t readLight(uint8_t id) {
    return theLight;
}

// Initialise serial cable and run one time setup
bool application_init(void)
{

    char *portname = "/dev/ttyUSB0";

    fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        NABTO_LOG_INFO(("error %d opening %s: %s\n", errno, portname, strerror (errno)));
        return 0;
    }
    else{
        NABTO_LOG_INFO(("Opening %s: %s\n", portname, strerror (errno)));
    }

    set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd, 0);                // set no blocking

    // Wake Roomba
    setRTS(fd, 0);
    usleep(100000); //0.1 sec
    setRTS(fd, 1);
    sleep(2);

    // Songs can be defined while in passive mode (which we are in right now)
    char DSB[] = {140, 0, 3, 74, 40, 75, 20, 70, 32};
    write(fd, &DSB, sizeof(DSB));
    usleep ((sizeof(DSB)+25) * 100);

    char reverse[] = {140, 1, 6, 84, 32, 30, 32, 84, 32, 30, 32, 84, 32, 30, 32};
    write(fd, &reverse, sizeof(reverse));
    usleep ((sizeof(reverse)+25) * 100);

    //char car_low[] = {140, 2, 12, 74,10, 71,10, 67,10, 67,10, 67,10, 69,10, 71,10, 72,10, 74,10, 74,10, 74,10, 71,10};
    //write(fd, &car_low, sizeof(car_low));
    //usleep ((sizeof(car_low)+25) * 100);

    char car_high[] = {140, 3, 12, 86,10, 83,10, 79,10, 79,10, 79,10, 81,10, 83,10, 84,10, 86,10, 86,10, 86,10, 83,10};
    write(fd, &car_high, sizeof(car_high));
    usleep ((sizeof(car_high)+25) * 100);

    // Put into safe mode
    char safe[] = {128, 131};
    write(fd, &safe, sizeof(safe));
    usleep(20*1000); // When changing mode, allow 20 milliseconds

    // Set display to NAb T/O
    char display[] = {163, 55, 119, 124, 93};
    write(fd, &display, sizeof(display));
    usleep ((sizeof(display)+25) * 100);

    return 1;
}

/** Asynchronous event model - queue the request for later response */
#if NABTO_APPLICATION_EVENT_MODEL_ASYNC

// Holds one saved request
static application_request* saved_app_req = 0;

application_event_result application_event(application_request* request, buffer_read_t* r_b, buffer_write_t* w_b)
{
    if (request->queryId == 1 || request->queryId == 2 || request->queryId == 3) {
        NABTO_LOG_INFO(("Application event: Respond immediately"));
        return demo_application(request, r_b, w_b);
    }
    if (saved_app_req) {
        NABTO_LOG_INFO(("Application event: No resources"));
        return AER_REQ_OUT_OF_RESOURCES;
    } else {
        NABTO_LOG_INFO(("Application event: Accept request"));
        saved_app_req = request;
        return AER_REQ_ACCEPTED;
    }
}

// Query whether a response to a queued request is ready
bool application_poll_query(application_request** appreq)
{
    if (saved_app_req) {
        /**
        * Fake a delay for demonstration purpose.
        * This could be replaced with a variable that is first
        * set when some proccessing is done and a response
        * to the client should be send.
        */
        static int fake_delay = 0;
        nabto_yield(20);

        if (++fake_delay >= 450) { //3000
            fake_delay = 0;
            *appreq = saved_app_req;
            NABTO_LOG_INFO(("Application poll query: Response ready"));
            return true;
        }
    }
    return false;
}

// Retrieve the response from a queued request
application_event_result application_poll(application_request* request, buffer_read_t* r_b, buffer_write_t* w_b)
{
    application_event_result res;

    if (saved_app_req == 0) {
        NABTO_LOG_FATAL(("No queued request"));
        return AER_REQ_SYSTEM_ERROR;
    }

    res = demo_application(request, r_b, w_b);
    if (res == AER_REQ_RESPONSE_READY) {
        NABTO_LOG_INFO(("Application poll: Response delivered"));
    }

    saved_app_req = 0;
    return res;
}

// Drop the queued request (framework discarded it internally)
void application_poll_drop(application_request* request)
{
    NABTO_LOG_INFO(("Application poll drop: Response discarded"));
    saved_app_req = 0;
}

/** Synchronous event model - just call the demo application directly */

#else

application_event_result application_event(application_request* request, buffer_read_t* r_b, buffer_write_t* w_b)
{
    return demo_application(request, r_b, w_b);
}

#endif /* NABTO_APPLICATION_EVENT_MODEL_ASYNC */

void setTimeFromGSP(uint32_t time)
{
}
