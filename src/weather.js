
var dictionary = {
  'KEY_SUNRISE' : 0,
  'KEY_SUNSET' : 0,
  'KEY_TEMP' : 0,
  'KEY_TEMPMIN' : 0,
  'KEY_TEMPMAX' : 0,
  'KEY_TEMP_CF' : 0
};

var lon,lat;
var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 10000, 
  timeout: 10000
};

function locationSuccess(pos) {
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
  lat = pos.coords.latitude;
  lon = pos.coords.longitude;
}

function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}

function getLocation(){
 if(localStorage.getItem('gps') == "gps_yes") { //If null, undefined or no
    //Call GPS
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
    if(lon !== undefined){ //GPS data avaliable
         return "lat=" + lat + "&lon="  + lon;
       } else { //No GPS data to use, using location instead
         return "q=" + localStorage.getItem('location');
       }
  }
  if(localStorage.getItem('gps') == "gps_no"){ //Don't use GPS
    return "q=" + localStorage.getItem('location');   
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
                          console.log('gps = '+ localStorage.getItem('gps'));
                          console.log('location = '+ localStorage.getItem('location'));
                          console.log('temp_CF = '+ localStorage.getItem('temp_CF'));
                          if(localStorage.getItem('gps') == "gps_yes") navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                          //Setup variables if any are empty
                          if(!localStorage.getItem('gps')){
                            console.log('GPS value null or undefined');
                            navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                            localStorage.setItem('gps','gps_yes');
                          }
                          if(!localStorage.getItem('location'))  localStorage.setItem('location','Montreal,Canada');
                          if(!localStorage.getItem('temp_CF')) localStorage.setItem('temp_CF',"C");                           
                          getweather();
                        }
                       );

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
                        function(e) {
                          console.log('AppMessage received!');
                          //console.log(dictionary);
                          getweather();
                        }                     
                       );



var xhrRequest = function (url, type, callback, count) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.onreadystatechange = function(){
    if( xhr.readyState == 4 && xhr.status != 200 & count < 6){
      setTimeout(function(){xhrRequest(url,type,callback,count +1);}, 120000);
      //console.log("URL CALL FAILED: " + url);
      //console.log("Attempt: " + count);
    }
  };
  xhr.open(type, url);
  xhr.send();
};

function getweather() {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?' + getLocation() + "&APPID=b585355033bf54a340e1daacc9ea2c09";
console.log("Get weather run!");
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
             function(responseText) {
               // responseText contains a JSON object with weather info
               var json = JSON.parse(responseText);
               console.log("location: " + url + "Response: " + responseText);
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
               var temp;
              if(localStorage.getItem('temp_CF') !== "F"){ //C
               temp = Math.round(json.main.temp - 273.15);
              } else { //F
               temp = Math.round((json.main.temp - 273.15)*1.8+32);
              }
               updatedictionary('KEY_SUNRISE', sunrise);
               updatedictionary('KEY_SUNSET', sunset);
               updatedictionary('KEY_TEMP', temp);
               url = 'http://api.openweathermap.org/data/2.5/forecast/daily?' + getLocation() + "&APPID=b585355033bf54a340e1daacc9ea2c09";
               xhrRequest(url, 'GET', 
                          function(responseText) {
                            //console.log(" Dictionary 2 read begins \n");
                            //console.log( dictionary.KEY_TEMPMIN + " " + dictionary.KEY_TEMPMAX + " " + dictionary.KEY_SUNRISE + " " + dictionary.KEY_SUNSET + " " + dictionary.KEY_TEMP);
                            var json = JSON.parse(responseText);
                            var temp_min,temp_max;
                            if(localStorage.getItem('temp_CF') !== "F"){ //C
                              
                            temp_min = Math.round(json.list[0].temp.min - 273.15);
                            temp_max = Math.round(json.list[0].temp.max - 273.15);
                            } else { //F
                              console.log("USING F for temp");
                            temp_min = Math.round((json.list[0].temp.min - 273.15)*1.8 +32);
                            temp_max = Math.round((json.list[0].temp.max - 273.15)*1.8 +32);                              
                            }
                            //console.log("temp_min " + temp_min +"\ntemp_max " +temp_max);
                            //console.log("temp_max" + temp_max);
                            updatedictionary('KEY_TEMPMIN', temp_min);
                            updatedictionary('KEY_TEMPMAX', temp_max);
                            updatedictionary('KEY_TEMP_CF', localStorage.getItem('temp_CF'));
                            //console.log("reached sending");
                            senddictionary();
                          },1     
                         );
          

             },1//function inside xhrrequest ends      
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
  localStorage.setItem('gps',json.gps);
  localStorage.setItem('temp_CF',json.temp_CF);
  if(json.gps == "gps_yes") navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  getweather();
  
  
});