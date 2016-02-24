/*!
 *  JavaScript application using Nabto communication.
 */


/*MAYBE*/
// Get water temp in coffee machine 
function queryTemperature(input) {
  window.alert("Ordering stuff");
  jNabto.request("cm_temperature.json?", function(err, data) {
    if (!err) {
      // Show this somewhere
      input.val("Water temperature: " + (data.temperature_c/10000).toFixed(2) + "C").button("refresh");
    }
  });
}

// Send order
function brew_stuff(input) {
  
  // Need a Nabto request here to check if connected 
  jNabto.request("cm_write.json?beverage_id=0", function(err, data) {
  if (!err) {
    //window.alert("Ordering stuff");
    //input.val("Something is brewing").button("refresh");
    //document.getElementById("#brew_button".value="Something is brewing");
    /*
    newState = 0;
    setTimeout(function () {
        if (newState == -1) {
            alert('VIDEO HAS STOPPED');
        }
    }, 5000);
    */
  }
  
  //window.alert("Ordering stuff");
  });
}

$(document).on("pageinit", function() {
  
  // Initialize Nabto JavaScript library
  jNabto.init({
    debug: false
  });
  
  
  // The three data functions
  $("#brew_button").click(function() {
    brew_stuff($(this));
  });
  

  /*
  $("#power_update").click(function() {
    queryPower($(this));
  });
  */

  
  $("#temperature_update").click(function() {
    queryTemperature($(this));
  });

  // Read coffee machine status
  //jNabto.request("cm_status.json?", setLight_r);
  
  
  /*Bind change event to the four switches */
  /*
  $("#rgb-r").change(function() {
    var state = $(this).val() === "off"?0:1;
    jNabto.request("light_write.json?light_id=0&light_on=" + state, setLight_r);
  });

  $("#rgb-g").change(function() {
    var state = $(this).val() === "off"?0:1;
    jNabto.request("light_write.json?light_id=1&light_on=" + state, setLight_g);
  });
  
  $("#rgb-b").change(function() {
    var state = $(this).val() === "off"?0:1;
    jNabto.request("light_write.json?light_id=2&light_on=" + state, setLight_b);
  });
  */
});
