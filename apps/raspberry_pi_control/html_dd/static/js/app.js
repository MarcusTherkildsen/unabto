/*!
 *  JavaScript application using Nabto communication.
 */

// Set the light state if no errors are returned
function setLight(err, data) {
  if (!err) {
    $("#living-room-status").val(data.light_state?"on":"off").slider("refresh");
    $("#light-div-on").stop().fadeTo(1000, data.light_state);
  }
}

$(document).on("pageinit", function() {
  // Initialize Nabto JavaScript library
  jNabto.init({
    debug: false
  });
    
  // Update living room status on startup. Should probably also check if Roomba is awake or not
  jNabto.request("light_read.json?light_id=1", setLight);
  
  // Bind change event to living room switch
  $("#living-room-status").change(function() {
    var state = $(this).val() === "off"?0:1;
    //jNabto.request("light_write.json?light_id=1&light_on=" + state, setLight);
  });


  var test = document.getElementById('test');

  test.onkeydown = function(evt) {
    if ( this.className === 'hold' ) { return false; }
    this.className = 'hold';

    evt = evt || window.event;
    var charCode = evt.which || evt.keyCode;
    /*
    var charStr = String.fromCharCode(charCode);
    alert(charCode);
    alert(charStr);
*/
    jNabto.request("light_write.json?light_id=" + charCode + "&light_on=" + 1, setLight);

  };

  test.onkeyup = function(evt) {
    this.className = '';

    evt = evt || window.event;
    var charCode = evt.which || evt.keyCode;
  /*
    var charStr = String.fromCharCode(charCode);
    alert(charCode);
    alert(charStr);
    */
    jNabto.request("light_write.json?light_id=" + charCode + "&light_on=" + 0, setLight);

  };



});
