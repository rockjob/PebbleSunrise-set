#include <pebble.h>

#define KEY_SUNRISE 0
#define KEY_SUNSET 1
#define KEY_TEMP 2
#define KEY_TEMPMIN 3
#define KEY_TEMPMAX 4

static char sunrise_buffer[6];
static char sunset_buffer[6];
static char time_buffer[] = "00:00";
static char date_buffer[16];
static int tempnow,temp_min,temp_max;
static char tempnow_buffer[6],temp_min_buffer[12],temp_max_buffer[12];
char riseset_buffer[32];
char tmp[16];
char tmp2[10];
Window *window;
TextLayer *text_time,*text_date, *text_riseset, *text_temp,*text_tempmin,*text_tempmax;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  strftime(time_buffer, sizeof("00"), "%M", tick_time);
  if(((int)time_buffer % 30)== 0){
    app_message_outbox_send();
  }
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Value for compare %c", sunset_buffer[2]);
  //if(sunset_buffer[2] != ':'){
  //app_message_outbox_send();
  //}
  // Create a long-lived buffer
    // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  //snprintf(temp_buffer, sizeof(temp_buffer), "%s\n\nSunrise: %s \nSunset: %s",time_buffer,sunrise_buffer, sunset_buffer);
  snprintf(riseset_buffer, sizeof(riseset_buffer), "Sunrise: %s \nSunset: %s",sunrise_buffer, sunset_buffer);
  
  strftime(tmp, sizeof(' '),"%w",tick_time);
  
  switch(tmp[0]){
    case('0'):
    strcpy(tmp, "Sun ");
    break;
    case('1'):
    strcpy(tmp, "Mon ");
    break;
    case('2'):
    strcpy(tmp, "Tue ");
    break;
    case('3'):
    strcpy(tmp, "Wed ");
    break;
    case('4'):
    strcpy(tmp, "Thu ");
    break;
    case('5'):
    strcpy(tmp, "Fri ");
    break;
    case('6'):
    strcpy(tmp, "Sat ");
    break;
  }
  //APP_LOG(APP_LOG_LEVEL_ERROR, "tmp= %s", tmp);
  strftime(date_buffer, sizeof(date_buffer), "%d %b %Y",tick_time);
  //strncpy(tmp, date_buffer, sizeof(date_buffer));
 memcpy(&tmp[4], &date_buffer[0], 11);
  //APP_LOG(APP_LOG_LEVEL_ERROR, "tmp= %s", tmp);
  
  
  text_layer_set_text(text_riseset, riseset_buffer);
  text_layer_set_text(text_date, tmp);
  text_layer_set_text(text_time, time_buffer);
  text_layer_set_text(text_temp, tempnow_buffer);
  text_layer_set_text(text_tempmin, temp_min_buffer);
  text_layer_set_text(text_tempmax, temp_max_buffer);
  
  
  APP_LOG(APP_LOG_LEVEL_ERROR, "time buffer %s \n date %s \n riseset %s",time_buffer,date_buffer,riseset_buffer);
  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox Received call!");
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_SUNRISE:
      
    snprintf(sunrise_buffer, sizeof(sunrise_buffer), "%s", t->value->cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunrise time sent to pebble: %s", sunrise_buffer);
    break;
      case KEY_SUNSET:
      snprintf(sunset_buffer, sizeof(sunset_buffer), "%s", t->value->cstring);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunset time sent to pebble: %s", sunset_buffer);
      //Need to move the below to a mainloop
      //snprintf(temp_buffer, sizeof(temp_buffer), "%s\n\nSunrise: %s \nSunset: %s",time_buffer,sunrise_buffer, sunset_buffer);
      //text_layer_set_text(text_layer, temp_buffer);
      
      break;
      case KEY_TEMP:
      tempnow = t->value->int32; 
      break;
      case KEY_TEMPMIN:
      temp_min = t->value->int32; 
      break;
      case KEY_TEMPMAX:
      temp_max = t->value->int32; 
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  //snprintf(temp_buffer, sizeof(temp_buffer), "Sunrise: %s \nSunset: %s",sunrise_buffer, sunset_buffer);
  //snprintf(temp_buffer, sizeof(temp_buffer), "%s\n\nSunrise: %s \nSunset: %s",time_buffer,sunrise_buffer, sunset_buffer);
  //text_layer_set_text(text_layer2, temp_buffer);
  snprintf(tempnow_buffer, sizeof(tempnow_buffer), "%i°C", tempnow);
  snprintf(temp_min_buffer, sizeof(temp_min_buffer), "Min: %i°C", temp_min);
  snprintf(temp_max_buffer, sizeof(temp_max_buffer), "Max: %i°C", temp_max);
  APP_LOG(APP_LOG_LEVEL_ERROR, "temp %i tempmin %i tempmax %i ",tempnow,temp_min,temp_max);
  update_time();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
	text_time = text_layer_create(GRect(0, 0, 144, 58));
  text_date = text_layer_create(GRect(0, 58, 144, 20));
  text_riseset = text_layer_create(GRect(0, 110, 144, 58));
  text_temp = text_layer_create(GRect(13, 85, 46, 35));
  text_tempmax = text_layer_create(GRect(71, 80, 60, 20));
  text_tempmin= text_layer_create(GRect(71, 97, 60, 20));
	
	// Set the text, font, and text alignment
	//text_layer_set_text(text_time, "Waiting for Info");
	text_layer_set_font(text_time, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	text_layer_set_text_alignment(text_time, GTextAlignmentCenter);
  
  text_layer_set_font(text_date, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(text_date, GTextAlignmentCenter);
  
  text_layer_set_font(text_riseset, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(text_riseset, GTextAlignmentCenter);
	
  text_layer_set_text_alignment(text_temp, GTextAlignmentCenter);
  text_layer_set_font(text_temp, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  text_layer_set_text_alignment(text_tempmin, GTextAlignmentCenter);
  text_layer_set_font(text_tempmin, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
  text_layer_set_text_alignment(text_tempmax, GTextAlignmentCenter);
  text_layer_set_font(text_tempmax, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_time));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_date));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_riseset));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_temp));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_tempmax));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_tempmin));

	// Push the window
	window_stack_push(window, true);
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	// App Logging!
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
  
  if(persist_exists(KEY_SUNRISE)){
    persist_read_data(KEY_SUNRISE, sunrise_buffer,  sizeof(sunrise_buffer));
    persist_read_data(KEY_SUNSET, sunset_buffer,  sizeof(sunset_buffer));
    persist_read_data(KEY_TEMP, tempnow_buffer,  sizeof(tempnow_buffer));
    persist_read_data(KEY_TEMPMIN, temp_min_buffer,  sizeof(temp_min_buffer));
    persist_read_data(KEY_TEMPMAX, temp_max_buffer,  sizeof(temp_max_buffer));
  }
  app_message_outbox_send();
  update_time();
}





void handle_deinit(void) {
	// Destroy the text layer
  persist_write_data(KEY_SUNRISE, sunrise_buffer,  sizeof(sunrise_buffer));
  persist_write_data(KEY_SUNSET, sunset_buffer,  sizeof(sunset_buffer));  
  persist_write_data(KEY_TEMP, tempnow_buffer,  sizeof(tempnow_buffer));
  persist_write_data(KEY_TEMPMIN, temp_min_buffer,  sizeof(temp_min_buffer));
  persist_write_data(KEY_TEMPMAX, temp_max_buffer,  sizeof(temp_max_buffer));
	text_layer_destroy(text_time);
  text_layer_destroy(text_date);
  text_layer_destroy(text_riseset);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}

