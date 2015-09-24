
var dictionary = {
  'KEY_SUNRISE' : 0,
  'KEY_SUNSET' : 0,
  'KEY_TEMP' : 0,
  'KEY_TEMPMIN' : 0,
  'KEY_TEMPMAX' : 0
};

function getLocation(){
  
  if(localStorage.getItem('location') !== null) {
    return localStorage.getItem('location');
    
  }else
    {
       return "Montreal,Canada";
    }
}

function updatedictionary(key1,value){
 console.log("key " + key1 + " value "+value);
  dictionary[key1] = value; 
}
function senddictionary(){
  Pebble.sendAppMessage(dictionary,
                                     function(e) {
                                       console.log('Info sent to Pebble successfully!');
                                     },
                                     function(e) {
                                       console.log('Error sending info to Pebble!');
                                     }
                                    );
}
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
  var url = 'http://api.openweathermap.org/data/2.5/weather?q=' + getLocation();

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

               var temp = json.main.temp - 272.15;
               //var temp_min = json.main.temp_min - 272.15;
               //var temp_max = json.main.temp_max - 272.15;

               updatedictionary('KEY_SUNRISE', sunrise);
               updatedictionary('KEY_SUNSET', sunset);
               updatedictionary('KEY_TEMP', temp);
               url = 'http://api.openweathermap.org/data/2.5/forecast/daily?q=' + getLocation();
               xhrRequest(url, 'GET', 
                          function(responseText) {
                            //console.log(" Dictionary 2 read begins \n");
                            //console.log( dictionary.KEY_TEMPMIN + " " + dictionary.KEY_TEMPMAX + " " + dictionary.KEY_SUNRISE + " " + dictionary.KEY_SUNSET + " " + dictionary.KEY_TEMP);
                            var json = JSON.parse(responseText);
                            var temp_min = json.list[0].temp.min - 272.15;
                            var temp_max = json.list[0].temp.max - 272.15;
                            //console.log("temp_min " + temp_min +"\ntemp_max " +temp_max);
                            //console.log("temp_max" + temp_max);
                            updatedictionary('KEY_TEMPMIN', temp_min);
                            updatedictionary('KEY_TEMPMAX', temp_max);
                            //console.log("reached sending");
                            senddictionary();
                          }     
                         );
          

             }//function inside xhrrequest ends      
            ); //xhrRequest end
  }

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://rockjob.tripod.com/config.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('Configuration window returned: ' + decodeURIComponent(e.response));
     var json = JSON.parse(decodeURIComponent(e.response));
   //console.log(json);
  localStorage.setItem('location',json.location);
  getweather();
  
  
});