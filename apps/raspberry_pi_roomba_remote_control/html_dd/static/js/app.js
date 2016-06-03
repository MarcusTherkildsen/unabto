/*!
 *  JavaScript application using Nabto communication.
 */

// Set the light state if no errors are returned

function setLight(err, data) {
  if (!err) {
//    $("#living-room-status").val(data.light_state?"on":"off").slider("refresh");
//    $("#light-div-on").stop().fadeTo(1000, data.light_state);
  }
}

function roomba_sensors(input) {
  jNabto.request("light_read.json?light_id=1", function(err, data) {
    if (!err) {

      /*
      Should return
      data.charging_state
      data.bat_temp
      data.bat_level

      all interpreted as uint8_t (although temp actually is signed, but the battery will never reach minus degrees)
      
      alert(data.bat_temp);
      alert(data.bat_level);
      alert(data.charging_state);
      */
      
      if (data.charging_state==1){

        cs="Charging"
      }
      else{
        cs="Not charging"
      }

      input.val("Charging state: " + cs + ", battery level: " + data.bat_level + " %, battery temperature: " + data.bat_temp + "°C").button("refresh");    
    }
  });
}

$(document).on("pageinit", function() {
  // Initialize Nabto JavaScript library
  jNabto.init({
    debug: false
  });

  // Wake Roomba
  jNabto.request("light_write.json?light_id=80&light_on=" + 1, setLight);

  // Pretend like we will not get a problem here

  // Get sensor data
  $("#roomba_sensor_button").click(function() {
    roomba_sensors($(this));
  });

  // Prepare text field
  var test = document.getElementById('test');

  test.onkeydown = function(evt) {
    if ( this.className === 'hold' ) { return false; }
    this.className = 'hold';

    evt = evt || window.event;
    var charCode = evt.which || evt.keyCode;

    jNabto.request("light_write.json?light_id=" + charCode + "&light_on=" + 1, setLight);

  };

  test.onkeyup = function(evt) {
    this.className = '';
  };

});
