#include <pebble.h>

#define KEY_SUNRISE 0
#define KEY_SUNSET 1

static char sunrise_buffer[6]; 
static char sunset_buffer[6];
static char time_buffer[] = "00:00"; 
char temp_buffer[32];
char temp_buffer2[32];
Window *window;
TextLayer *text_layer, *text_layer2;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Value for compare %c", sunset_buffer[2]);
  if(sunset_buffer[2] != ':'){
  app_message_outbox_send();
  }
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
  snprintf(temp_buffer, sizeof(temp_buffer), "Sunrise: %s \nSunset: %s",sunrise_buffer, sunset_buffer);
  text_layer_set_text(text_layer2, temp_buffer);
  snprintf(temp_buffer2, sizeof(temp_buffer2), "%s",time_buffer);
  text_layer_set_text(text_layer, temp_buffer2);
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
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunrise time sent to pebble: %s", sunrise_buffer);
      break;
      case KEY_SUNSET:
      snprintf(sunset_buffer, sizeof(sunset_buffer), "%s", t->value->cstring);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunset time sent to pebble: %s", sunset_buffer);
      //Need to move the below to a mainloop
      //snprintf(temp_buffer, sizeof(temp_buffer), "%s\n\nSunrise: %s \nSunset: %s",time_buffer,sunrise_buffer, sunset_buffer);
      //text_layer_set_text(text_layer, temp_buffer);
      
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
	text_layer = text_layer_create(GRect(0, 12, 144, 84));
  text_layer2 = text_layer_create(GRect(0, 84, 144, 84));
	
	// Set the text, font, and text alignment
	text_layer_set_text(text_layer2, "Waiting for Info");
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
  text_layer_set_font(text_layer2, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(text_layer2, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer2));

	// Push the window
	window_stack_push(window, true);
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
  update_time();
}





void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}

