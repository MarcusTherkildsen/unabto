/**
 *  uNabto application logic implementation
 */

#include "unabto/unabto_app.h"
#include <stdio.h>
#include <wiringPi.h>

// Function prototypes
uint8_t setLight(uint8_t id, uint8_t onOff);
uint8_t readLight(uint8_t id);

// The virtual light bulb variable
static uint8_t theLight = 0;

// Voltage and Power read from INA219 board
float voltage, power;
void getINA219_data(float* voltage, float* power);

// Temperature read from internal RPi sensor
float temperature;
void getRPi_temp(float* temperature);

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


	    case 3: {
            /*
            <query name="ina_voltage.json" description="Read voltage status" id="3">
            <request>
            </request>
            <response format="json">
            <parameter name="voltage_v" type="uint32"/>
            </response>
            </query>
            */

            // Get voltage from INA219
            getINA219_data(&voltage, &power);

            printf("Read voltage=%f\n", voltage);

            // Write back data
            if (!buffer_write_uint32(write_buffer, voltage*10000)) return AER_REQ_RSP_TOO_LARGE;
	       
            return AER_REQ_RESPONSE_READY;
        }
        case 4: {
            /*
            <query name="ina_power.json" description="Read power status" id="4">
            <request>
            </request>
            <response format="json">
            <parameter name="power_w" type="uint32"/>
            </response>
            </query>
            */

            // Get power from INA219
            getINA219_data(&voltage, &power);

            printf("Read power=%f\n", power);

            // Prepare for negative numbers. This will be converted back in the html_dd app.js file
            if (power<0)
            {
                power = power + 200;
            }

            // Write back data
            if (!buffer_write_uint32(write_buffer, power*10000)) return AER_REQ_RSP_TOO_LARGE;
            
            return AER_REQ_RESPONSE_READY;
        }
        case 5: {
            /*
            <query name="rpi_temperature.json" description="Read temperature status" id="5">
            <request>
            </request>
            <response format="json">
            <parameter name="temperature_c" type="uint32"/>
            </response>
            </query>
            */

            // Get internal temperature of the RPi
            getRPi_temp(&temperature);

            printf("Read temperature=%f\n", temperature);

            // Write back data
            if (!buffer_write_uint32(write_buffer, temperature*10000)) return AER_REQ_RSP_TOO_LARGE;
            
            return AER_REQ_RESPONSE_READY;
        }
    }
    return AER_REQ_INV_QUERY_ID;
}

// Set GPIO pin and return state,
uint8_t setLight(uint8_t id, uint8_t onOff) {
    theLight = onOff;
    if (id == 1) {
        NABTO_LOG_INFO((theLight?("Nabto: Red turned ON!"):("Nabto: Red turned OFF!")));
    }
    else if (id == 2) {
        NABTO_LOG_INFO((theLight?("Nabto: Green turned ON!"):("Nabto: Green turned OFF!")));
    }
    else if (id == 3) {
        NABTO_LOG_INFO((theLight?("Nabto: Blue turned ON!"):("Nabto: Blue turned OFF!")));
    }
    else if (id == 12) {
        NABTO_LOG_INFO((theLight?("Nabto: Relay turned ON!"):("Nabto: Relay turned OFF!")));
    }


#ifdef __arm__
    /* Toggle GPIO pins on Raspberry Pi	*/
    //Change pin output according to id and theLight state
    if (id == 1) {
        if (theLight){
            //Activate R
            digitalWrite(7, LOW);
        }
        else{
            digitalWrite(7, HIGH);
        }
    }
    else if (id == 2) {
        if (theLight){
            //Activate G
            digitalWrite(0, LOW);
        }
        else{
            digitalWrite(0, HIGH);
        }
    }
    else if (id == 3) {
        if (theLight){
            //Activate B
            digitalWrite(2, LOW);
        }
        else{
            digitalWrite(2, HIGH);
        }
    }
    // Check if relay slider activated
    else if (id == 12){
        if (theLight){
            digitalWrite(3, LOW);
        }
        else{
            digitalWrite(3, HIGH);
        }
    }

#endif

    return theLight;
}

// Return light state
uint8_t readLight(uint8_t id) {
    return theLight;
}

// Get INA219 data
void getINA219_data(float* voltage, float* power){

    FILE *fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("sudo python /home/pi/nabto_test/unabto_sdk/unabto_sdk/unabto/demo/raspberry_pi_sunpi/src/ina219_python_c.py", "r");

    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    int k = 0;
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        k = k + 1;
        if (k==1){
            *voltage = atof(path);
        }
        else if(k==2){
            *power = atof(path);
        }
    }

    /* close */
    pclose(fp);

    //Print results
    //printf("Voltage=%f\n", *voltage);
    //printf("Power=%f\n", *power);
}


// Get RPi temperature
void getRPi_temp(float* temperature){

    FILE *fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("/opt/vc/bin/vcgencmd measure_temp", "r");

    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    int k = 0;
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        k = k + 1;
        if (k==1){
            // Working method to extract 39.5 from temp=39.5'C
            int j = 0;
            char *str = path, *p = str;
            while (*p) { // While there are more characters to process...
                if (isdigit(*p)) { // Upon finding a digit, ...
                    long val = strtol(p, &p, 10); // Read a number, ...
                    j = j + 1;
                    if (j == 1)
                    {
                        *temperature = val;
                    }
                    else if (j == 2)
                    {
                        *temperature = *temperature + 0.1*val;
                    }
                } else { // Otherwise, move on to the next character.
                    p++;
                }
            }
        }
    }

    /* close */
    pclose(fp);
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
