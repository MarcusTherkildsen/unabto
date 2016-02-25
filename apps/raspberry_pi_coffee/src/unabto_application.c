/**
 *  uNabto application logic implementation
 */

#include "unabto/unabto_app.h"
#include <stdio.h>
#include <wiringPi.h>

// Function prototypes
uint8_t setLight(uint8_t id, uint8_t onOff);
uint8_t readLight(uint8_t id);

// Define each pin status
uint8_t theLight[] = {0, 0, 0};

// Define beverage names
const char * beverage_name[] = {"Coffee", "Coffee", "Coffee"};

// Define pin names
const char * pin_name[] = {"Red", "Green", "Blue", "Relay"};

// Define on/off
const char * on_off[] = {"OFF", "ON"};

// Define pin id (wiringPi pin number)
uint8_t pin_id[] = {10};

// Count variable
uint8_t i;

// Voltage and Power read from INA219 board
float voltage, power;
void getINA219_data(float* voltage, float* power);

// Temperature read from internal RPi sensor
float temperature;
void getRPi_temp(float* temperature);

uint8_t sendOrder(uint8_t id);

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
            <query name="cm_write.json" description="Turn light on and off" id="1">
              <request>
                <parameter name="beverage" type="uint8"/>
              </request>
              <response format="json">
                <parameter name="beverage_status" type="uint8"/>
              </response>
            </query>
            */

            uint8_t beverage_id;
            uint8_t beverage_state;

            uint8_t light_id;
            uint8_t light_on;
            uint8_t light_state;

            // Read parameters in request
            if (!buffer_read_uint8(read_buffer, &beverage_id)) {
                NABTO_LOG_ERROR(("Can't read light_id, the buffer is too small"));
                return AER_REQ_TOO_SMALL;
            }
            

            // Send order according to beverage_id
            //beverage_state = sendOrder(beverage_id);


            // Write back beverage state
            if (!buffer_write_uint8(write_buffer, beverage_state)) {
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

// Send order to coffee machine
uint8_t sendOrder(uint8_t id){
    NABTO_LOG_INFO(("Requested beverage id: %d\n", id));
    NABTO_LOG_INFO(("Requested beverage: %s\n", beverage_name[id]));

    /*
    SEND ACTUAL COMMAND TO COFFEE MACHINE
    */

    // Shut off manual control
    digitalWrite(11, LOW);
    delay(100);

    // Go all the way to the left
    
    for (i = 0; i < 8; i++){

        digitalWrite(13, HIGH);
        delay(40);
        digitalWrite(14, HIGH);
        delay(80);
        digitalWrite(13, LOW);
        delay(40);
        digitalWrite(14, LOW);

        delay(40);
    }
    

    // After this all pins are LOW
    
    // Go three steps right
    for (i = 0; i < 3; i++){

        digitalWrite(14, HIGH);
        delay(40);
        digitalWrite(13, HIGH);
        delay(80);
        digitalWrite(14, LOW);
        delay(40);
        digitalWrite(13, LOW);

        delay(40);
    }
    
    // Now we should be at coffee

    /*
    // Push button
    digitalWrite(10, LOW);
    digitalWrite(10, HIGH);
    delay(100); // 0.1 sec
    digitalWrite(10, LOW);
    */


    // Turn on manual control
    digitalWrite(11, HIGH);

    return 0;
}


// Set GPIO pin and return state,
uint8_t setLight(uint8_t id, uint8_t onOff) {
    theLight[id] = onOff;

    NABTO_LOG_INFO(("Nabto: %s turned %s!\n", pin_name[id], on_off[theLight[id]]));

#ifdef __arm__
    /* Toggle GPIO pins on Raspberry Pi	*/
    //Change pin output according to id and theLight state

    if (theLight[id]){
        //Activate pin
        digitalWrite(pin_id[id], LOW);
    }
    else if(theLight[id]==0){
        digitalWrite(pin_id[id], HIGH);
    }
#endif

    return theLight[id];
}

// Return light state
uint8_t readLight(uint8_t id) {
    return theLight[id];
}

// Get INA219 data
void getINA219_data(float* voltage, float* power){

    
    *voltage = 0;
    *power = 0;
        
}


// Get RPi temperature
void getRPi_temp(float* temperature){
    *temperature = 0;
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
