/*!
 *  JavaScript application using Nabto communication.
 */

// Set the light state if no errors are returned
function setLight_r(err, data) {
  if (!err) {
    $("#rgb-r").val(data.light_state?"on":"off").slider("refresh");
    $("#light-div-on").stop().fadeTo(1000, data.light_state);
	
  }
}

function setLight_g(err, data) {
  if (!err) {
    $("#rgb-g").val(data.light_state?"on":"off").slider("refresh");
    $("#light-div-on").stop().fadeTo(1000, data.light_state);	
  }
}

function setLight_b(err, data) {
  if (!err) {
    $("#rgb-b").val(data.light_state?"on":"off").slider("refresh");
    $("#light-div-on").stop().fadeTo(1000, data.light_state);	
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
  
  /* The light_id correspond to the four pins */
  jNabto.request("light_read.json?light_id=1", setLight_r);
  jNabto.request("light_read.json?light_id=2", setLight_g);
  jNabto.request("light_read.json?light_id=3", setLight_b);
  jNabto.request("light_read.json?light_id=12", setLight_arb);
  
  // Bind change event to the four switches  
  $("#rgb-r").change(function() {
  var state = $(this).val() === "off"?0:1;
  jNabto.request("light_write.json?light_id=1&light_on=" + state, setLight_r);
  });
  
  $("#rgb-g").change(function() {
  var state = $(this).val() === "off"?0:1;
  jNabto.request("light_write.json?light_id=2&light_on=" + state, setLight_g);
  });
  
  $("#rgb-b").change(function() {
  var state = $(this).val() === "off"?0:1;
  jNabto.request("light_write.json?light_id=3&light_on=" + state, setLight_b);
  });
  
  $("#arb").change(function() {
  var state = $(this).val() === "off"?0:1;
  jNabto.request("light_write.json?light_id=12&light_on=" + state, setLight_arb);
  });
  
});
