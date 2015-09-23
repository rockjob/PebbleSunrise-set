

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    getweather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getweather();
  }                     
);
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getweather() {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?q=Montreal,Canada';

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      
      var unixsunrise = json.sys.sunrise;
      var date = new Date(unixsunrise*1000);
      var hours = "0" + date.getHours();
      var minutes = "0" + date.getMinutes();
      var sunrise = hours.substr(-2) + ':' + minutes.substr(-2);
      
      var unixsunset = json.sys.sunset;
      date = new Date(unixsunset*1000);
      hours = date.getHours();
      minutes = "0" + date.getMinutes();
      var sunset = hours + ':' + minutes.substr(-2);
      //console.log('Sunrise is ' + sunrise);
/*
      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log('Temperature is ' + temperature);

      // Conditions
      var conditions = json.weather[0].main;      
      console.log('Conditions are ' + conditions);
      */
      var temp = json.main.temp - 272.15;
      var temp_min = json.main.temp_min - 272.15;
      var temp_max = json.main.temp_max - 272.15;
      var dictionary = {
      'KEY_SUNRISE': sunrise,
      'KEY_SUNSET': sunset,
      'KEY_TEMP' : temp,
      'KEY_TEMPMIN': temp_min,
      'KEY_TEMPMAX' : temp_max
        
      };
      //console.log(dictionary);
      Pebble.sendAppMessage(dictionary,
  function(e) {
    console.log('Info sent to Pebble successfully!');
  },
  function(e) {
    console.log('Error sending info to Pebble!');
  }
);
    }      
  );
}