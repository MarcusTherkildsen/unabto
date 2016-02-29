/**
 *  uNabto application logic implementation
 */

#include "unabto/unabto_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>

// Initialize the other thread
pthread_t thread1;
const char *message1 = "Thread 1";
int iret1;
void *print_message_function( void *ptr );

// Define beverage names
const char * beverage_name[] = {"Coffee", "Coffee", "Coffee"};

// sendOrder prototype
void* sendOrder(void *arg);


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
            
            /*
            <!--Write to coffee machine, i.e. this is where an order is parsed through. Return a "job accepted" response.-->
            <query name="cm_write.json" description="Send order to coffee machine" id="1">
              <request>
                <parameter name="beverage" type="uint8"/>
              </request>
              <response format="json">
                <parameter name="beverage_status" type="uint8"/>
              </response>
            </query>
            */

            uint8_t beverage_id;
            uint8_t beverage_status;

            // Read parameters in request
            if (!buffer_read_uint8(read_buffer, &beverage_id)) {
                NABTO_LOG_ERROR(("Can't read beverage_id, the buffer is too small"));
                return AER_REQ_TOO_SMALL;
            }

            NABTO_LOG_INFO(("Requested beverage id: %d\n", beverage_id));
            NABTO_LOG_INFO(("Requested beverage: %s\n", beverage_name[beverage_id]));

            // Preparing the integer to pass to the thread
            int *theOrder = malloc(sizeof(*theOrder));
            *theOrder = beverage_id;

            // Send order according to beverage_id
            iret1 = pthread_create( &thread1, NULL, &sendOrder, theOrder);
            if(iret1)
            {
                // Something went wrong with the thread
                beverage_status = 0;
            }
            else
            {
                // Succesfully sent the command to the thread. 
                beverage_status = 1;
            }
            
            // Write back beverage state
            if (!buffer_write_uint8(write_buffer, beverage_status)) {
                return AER_REQ_RSP_TOO_LARGE;
            }
            
            return AER_REQ_RESPONSE_READY;
            
        }
        case 2: {
            /*
            <!--Possibly return string instead, depending on the machine in question-->
            <query name="cm_status.json" description="Get coffee machine status" id="2">
              <request>
              </request>
              <response format="json">
                <parameter name="cm_status" type="uint8"/>
              </response>
            </query>
            */

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


	    case 3: {
            /*
            <!-- Get current water temperature of the coffee machine-->
            <query name="cm_temperature.json" description="Read temperature status" id="3">
              <request>
              </request>
              <response format="json">
                <parameter name="temperature_c" type="uint32"/>
              </response>
            </query>
            */

            // Get voltage from INA219
            getINA219_data(&voltage, &power);

            NABTO_LOG_INFO(("Nabto: Read voltage=%f\n", voltage));

            // Write back data
            if (!buffer_write_uint32(write_buffer, voltage*10000)) return AER_REQ_RSP_TOO_LARGE;
	       
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
    int id = *((int *) arg);

    // Experiment with this value
    int wait_msec = 400; 

    #ifdef __arm__

    // Shut off manual control
    digitalWrite(11, LOW);
    delay(wait_msec);

    // Go all the way to the left
    for (i = 0; i < 5; i++){

        digitalWrite(13, HIGH);
        delay(wait_msec);
        digitalWrite(14, HIGH);
        delay(wait_msec);
        digitalWrite(13, LOW);
        delay(wait_msec);
        digitalWrite(14, LOW);

        delay(wait_msec);
    }

    // After this all pins are LOW
        
    // Go id amount of steps to the right to the desired item
    for (i = 0; i < id; i++){
        // If odd
        if (i % 2 != 0){
            digitalWrite(14, LOW);
            delay(wait_msec);
            digitalWrite(13, LOW);
        }
        else{
            digitalWrite(14, HIGH);
            delay(wait_msec);
            digitalWrite(13, HIGH);
        }
        delay(wait_msec);
    }
    
    // Now we should be at the wanted beverage
    
    // Push button
    digitalWrite(10, LOW);
    digitalWrite(10, HIGH);
    delay(100); // 0.1 sec
    digitalWrite(10, LOW);   

    // Set the steppers to low
    digitalWrite(13, LOW);
    digitalWrite(14, LOW);

    // Turn on manual control
    digitalWrite(11, HIGH);

    #endif
    return NULL;
}


void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
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
      
        if (++fake_delay >= 200) { //3000
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
