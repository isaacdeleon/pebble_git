#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_text_layer;
static TextLayer *s_output_layer;
static TextLayer *s_bluetooth_layer;
static TextLayer *s_date_layer;


static void battery_handler(BatteryChargeState new_state){
	//write to buffer and display
	static char s_battery_buffer[32];
	BatteryChargeState charge_state = battery_state_service_peek();
	//snprintf(s_battery_buffer,sizeof(s_battery_buffer),"Bateria Restante: %d/100", new_state.charge_percent);
	
	if(charge_state.is_charging){
		snprintf(s_battery_buffer,sizeof(s_battery_buffer),"charging");
	}else{
		snprintf(s_battery_buffer,sizeof(s_battery_buffer),"%d%% Restante", new_state.charge_percent);
	}
	//text_layer_set_text(battery_layer, s_battery_buffer);
	text_layer_set_text(s_output_layer,s_battery_buffer);
	
}

static void bt_handler(bool connected){
	
	//show current connection state
	if(connected){
		text_layer_set_text(s_bluetooth_layer,"Connected lml");
	}else{
		text_layer_set_text(s_bluetooth_layer,"Disconnected");
	}
}

static void main_window_load(Window *window){
	
	Layer *window_layer = window_get_root_layer(window);
	GRect window_bounds = layer_get_bounds(window_layer);
	
	//bluetooth output
	s_bluetooth_layer = text_layer_create(GRect(0,5,144,50));
	
	// my text
	s_text_layer = text_layer_create(GRect(0,30,144,50));
	text_layer_set_text(s_text_layer,"Marco :)");
	
	//create output layer for battery
	s_output_layer = text_layer_create(GRect(0,140,144,50));
	
	//date layer
	s_date_layer = text_layer_create(GRect(0, 115, 144, 50));
	
	
	//set color , padding  and text for time
	s_time_layer = text_layer_create(GRect(0,70,144,50));
	text_layer_set_background_color(s_time_layer,GColorClear);
	text_layer_set_text_color(s_time_layer,GColorBlack);
	text_layer_set_text(s_time_layer,"00:00");
	
	 // Improve the layout to be more like a watchface
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
	
	text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text_alignment(s_output_layer,GTextAlignmentCenter);
	
	text_layer_set_font(s_bluetooth_layer,fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text_alignment(s_bluetooth_layer,GTextAlignmentCenter);
	
	text_layer_set_font(s_date_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(s_date_layer,GTextAlignmentCenter);
	
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_layer));	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_text_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
	
	//get current battery level
	battery_handler(battery_state_service_peek());
	
	//show current connection state
	bt_handler(bluetooth_connection_service_peek());
}

static void update_time(){
	//time structured
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	//create a long lived buffer
	static char buffer[] = "00:00";
	static char bufferDate[] = "00:00:00";
	
	//write curretn hours and minutes into buffer
	if(clock_is_24h_style() == true){
		//use 24 hour format"
		strftime(buffer,sizeof("00:00"),"%H:%M",tick_time);
	}else{
		strftime(buffer,sizeof("00:00"),"%I:%M",tick_time);
	}
	
	strftime(bufferDate, sizeof("dd/mm/yy"), "%d/%m/%y", tick_time);
	
	//display this time on the textlayer
	text_layer_set_text(s_time_layer,buffer);

	// Display this date on the TextLayer
	text_layer_set_text(s_date_layer, bufferDate);
}

static void main_window_unload(Window *window){
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_text_layer);
	text_layer_destroy(s_output_layer);
	text_layer_destroy(s_bluetooth_layer);
	text_layer_destroy(s_date_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	update_time();
}
static void init(){
	// Create main Window element and assign to pointer
	s_main_window = window_create();
	
	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window,(WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
	
	 // Show the Window on the watch, with animated=true
	window_stack_push(s_main_window,true);
	
	//subscribe to the battery state service
	battery_state_service_subscribe(battery_handler);
	
	// Subscribe to Bluetooth updates
	bluetooth_connection_service_subscribe(bt_handler);
}


static void deinit(){
	window_destroy(s_main_window);
}

int main(void){
	init();
	
	//registerthick timer
	tick_timer_service_subscribe(MINUTE_UNIT,tick_handler);
	//update_time();
	app_event_loop();
	deinit();
}
