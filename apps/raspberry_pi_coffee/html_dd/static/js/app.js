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
function brew_stuff(x) {
  //alert(x);
  // Send a Nabto request with the selected beverage
  jNabto.request("cm_write.json?beverage_id="+x, function(err, data) {
  if (!err) {


    // I want the button to become "non-clickable" + display "Now brewing"

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
  
  
  // When brew button is pushed get the chosen radio button value
  $("#brew_button").click(function() {
    brew_stuff($('input[name="radio-choice"]:checked').val());
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
  
});
