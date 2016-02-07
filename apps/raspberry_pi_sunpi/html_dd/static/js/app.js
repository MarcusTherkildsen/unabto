/*!
 *  JavaScript application using Nabto communication.
 */

// Set initial global values for the three RGB LEDs
var rgbr_val = 0;
var rgbg_val = 0;
var rgbb_val = 0;
 
function queryVoltage(input) {
  jNabto.request("ina_voltage.json?", function(err, data) {
    if (!err) {
      input.val("Voltage: " + (data.voltage_v/10000).toFixed(2) + "V").button("refresh");
    }
  });
}

function queryPower(input) {
  jNabto.request("ina_power.json?", function(err, data) {
    if (!err) {

      temp_power = data.power_w/10000

      // Check if negative
      if (temp_power > 100)
      {
        temp_power = temp_power - 200
      }

      input.val("Power: " + temp_power.toFixed(2) + "W").button("refresh");
    }
  });
}

function queryTemperature(input) {
  jNabto.request("rpi_temperature.json?", function(err, data) {
    if (!err) {
      input.val("Temperature: " + (data.temperature_c/10000).toFixed(2) + "C").button("refresh");
    }
  });
}

// Set the light state dependent on the RGB settings
function setRGB_bulb(rgbr_val, rgbg_val, rgbb_val){

  var image = document.getElementById('light_state_image');

  // 8 possible states

  if (rgbr_val == 0 && rgbg_val == 0 && rgbb_val == 0 ){
    // All LEDs off
    image.src = "img/___.png";
  }

  else if(rgbr_val == 1 && rgbg_val == 1 && rgbb_val == 1 ){
    // All LEDs on -> White
    image.src = "img/RGB.png";
  }
  
  else if(rgbr_val == 1 && rgbg_val == 0 && rgbb_val == 0 ){
    // Only red LED
    image.src = "img/R__.png";
  }

  else if(rgbr_val == 0 && rgbg_val == 1 && rgbb_val == 0 ){
    // Only green LED
    image.src = "img/_G_.png";
  }

  else if(rgbr_val == 0 && rgbg_val == 0 && rgbb_val == 1 ){
    // Only blue LED
    image.src = "img/__B.png";
  }

  else if(rgbr_val == 1 && rgbg_val == 1 && rgbb_val == 0 ){
    // Both red and green -> Yellowish
    image.src = "img/RG_.png";
  }

  else if(rgbr_val == 1 && rgbg_val == 0 && rgbb_val == 1 ){
    // Both red and blue -> Purple
    image.src = "img/R_B.png";
  }

  else if(rgbr_val == 0 && rgbg_val == 1 && rgbb_val == 1 ){
    // Both green and blue -> Turkis
    image.src = "img/_GB.png";
  }
}


// Set the switch state
function setLight_r(err, data) {
  if (!err) {
    $("#rgb-r").val(data.light_state?"on":"off").slider("refresh");
    rgbr_val = data.light_state;
    setRGB_bulb(rgbr_val, rgbg_val, rgbb_val);
  }
}

function setLight_g(err, data) {
  if (!err) {
    $("#rgb-g").val(data.light_state?"on":"off").slider("refresh");
    rgbg_val = data.light_state;
    setRGB_bulb(rgbr_val, rgbg_val, rgbb_val);
  }
}

function setLight_b(err, data) {
  if (!err) {
    $("#rgb-b").val(data.light_state?"on":"off").slider("refresh");
    rgbb_val = data.light_state;
    setRGB_bulb(rgbr_val, rgbg_val, rgbb_val);
  }
}

function setLight_arb(err, data) {
  if (!err) {
    $("#arb").val(data.light_state?"on":"off").slider("refresh");	
  }
}

$(document).on("pageinit", function() {
  
  // Initialize Nabto JavaScript library
  jNabto.init({
    debug: false
  });
  
  // The three data functions
  $("#voltage_update").click(function() {
    queryVoltage($(this));
  });
  
  $("#power_update").click(function() {
    queryPower($(this));
  });

  $("#temperature_update").click(function() {
    queryTemperature($(this));
  });

  /* The light_id correspond to the four pins
  Bind change event to the four switches */  
  jNabto.request("light_read.json?light_id=1", setLight_r);
  $("#rgb-r").change(function() {
    var state = $(this).val() === "off"?0:1;
    jNabto.request("light_write.json?light_id=1&light_on=" + state, setLight_r);
  });

  jNabto.request("light_read.json?light_id=2", setLight_g);
  $("#rgb-g").change(function() {
    var state = $(this).val() === "off"?0:1;
    jNabto.request("light_write.json?light_id=2&light_on=" + state, setLight_g);
  });

  jNabto.request("light_read.json?light_id=3", setLight_b);
  $("#rgb-b").change(function() {
    var state = $(this).val() === "off"?0:1;
    jNabto.request("light_write.json?light_id=3&light_on=" + state, setLight_b);
  });

  jNabto.request("light_read.json?light_id=12", setLight_arb);
  $("#arb").change(function() {
    var state = $(this).val() === "off"?0:1;
    jNabto.request("light_write.json?light_id=12&light_on=" + state, setLight_arb);
  });
});
