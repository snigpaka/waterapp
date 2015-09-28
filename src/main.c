#include <pebble.h>
#include <stdlib.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;

static GFont s_text_font;
static TextLayer *s_text_layer;

static GFont s_const_font;
static TextLayer *s_const_layer;

static int counter;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void update_Drinkmsg() {
  static char constbuffer[] = "Drink Water For:";
  static char *textbuffer[] = {"Beautiful Skin", "Long Life", "Happiness", "Health", "No Headaches", "No Cavities", "Look Younger", "Feel Good"};
  
  text_layer_set_text(s_const_layer, constbuffer);
  text_layer_set_text(s_text_layer, textbuffer[counter]);
  counter++;
  if(counter == 8) {
    counter = 0;
  }
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(20, 10, 100, 80));
  s_const_layer = text_layer_create(GRect(1, 50, 143, 120));
  s_text_layer = text_layer_create(GRect(1, 90, 143, 167));
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_35));
  s_const_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_18));
  s_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_18));
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");

  text_layer_set_font(s_const_layer, s_const_font);
  text_layer_set_background_color(s_const_layer, GColorClear);
  text_layer_set_text_color(s_const_layer, GColorBlack);
  text_layer_set_text(s_const_layer, "");
  
  text_layer_set_font(s_text_layer, s_text_font);
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_text_color(s_text_layer, GColorBlack);
  text_layer_set_text(s_text_layer, "");

  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_const_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_const_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_layer));
  // Make sure the time is displayed from the start
  update_time();
  update_Drinkmsg();
}

static void main_window_unload(Window *window) {
  // Unload GFont
  fonts_unload_custom_font(s_time_font);

  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_const_layer);
  text_layer_destroy(s_text_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  time_t temp = time(NULL); 
  tick_time = localtime(&temp);
  static char buf[] = "00";
  strftime(buf, sizeof("00"), "%M", tick_time);
  if((atoi(buf))%2 == 0) {
     update_Drinkmsg();
  }
}

/*static void tick_text_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_Drinkmsg();
}*/
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  counter = 0;
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  //tick_timer_service_subscribe(SECOND_UNIT, tick_text_handler);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}