

/*
 * builtin_led_accessory.c
 * Define the accessory in pure C language using the Macro in characteristics.h
 *
 *  Created on: 2020-04-13
 *      Author: Mixiaoxiao (Wang Bin)
 */

#include <Arduino.h>
#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>
#include <port.h>
#include <esp_wifi_types.h>
#include <esp_wifi.h>

static char ACCESSORY_NAME[32] = "MY_NIGGA";
static int RELAY[3] = {16, 17, 5};
static int LED[4] = {2, 25, 26, 27};
static bool led_state = false;
int cur_speed = 0;
int last_state = 0;
static bool brightness = true;
//homekit_value_t h_state;
#define ACCESSORY_SN  ("SN_3232323")  //SERIAL_NUMBER
#define ACCESSORY_MANUFACTURER ("Arduino HomeKit")
#define ACCESSORY_MODEL  ("ESP32_DEVKIT")

#define PIN_LED  2

void choose_relay(int new_state) {
  if (cur_speed == new_state) {
    return;
  }
  digitalWrite(RELAY[0], HIGH);
  digitalWrite(RELAY[1], HIGH);
  digitalWrite(RELAY[2], HIGH);
  digitalWrite(LED[1], LOW);
  digitalWrite(LED[2], LOW);
  digitalWrite(LED[3], LOW);
  if(new_state > 0) {
    if (brightness)
      digitalWrite(LED[new_state], HIGH);
    //analogWrite(LED[new_state], brightness);
    delay(20);
    digitalWrite(RELAY[new_state - 1], LOW);
    //homekit_characteristic_notify(&led_on, led_on.value);
  }
  printf("Set fan to %d\n", new_state);
  //h_state.float_value = new_state * 33.0;
  cur_speed = new_state;
}

void led_set_power(bool on) {
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

homekit_value_t led_on_get() {
    return HOMEKIT_UINT8(brightness);
}

homekit_value_t fan_on_get() {
    return HOMEKIT_UINT8(cur_speed > 0);
}

homekit_value_t fan_spd_get() {
    return HOMEKIT_FLOAT(33.0 * cur_speed);
}


void active_setter(homekit_value_t value);
void speed_setter(homekit_value_t value);
void light_setter(homekit_value_t value);

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, ACCESSORY_NAME);
homekit_characteristic_t serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, ACCESSORY_SN);
homekit_characteristic_t fan_on = HOMEKIT_CHARACTERISTIC_(ACTIVE, 0, .setter=active_setter, .getter=fan_on_get);
homekit_characteristic_t led_on = HOMEKIT_CHARACTERISTIC_(ACTIVE, 0, .setter=light_setter, .getter=led_on_get);
//homekit_characteristic_t led_brightness = HOMEKIT_CHARACTERISTIC_(BRIGHTNESS, 0, .setter=light_setter);
homekit_characteristic_t fan_speed = HOMEKIT_CHARACTERISTIC_(ROTATION_SPEED, 0, .setter=speed_setter, .getter=fan_spd_get);
//homekit_characteristic_t target_speed = HOMEKIT_CHARACTERISTIC_(TARGET_FAN_STATE, 0, .getter=fan_spd_get);

void notify() {
  homekit_characteristic_notify(&fan_speed, HOMEKIT_FLOAT(33.0 * cur_speed));
  homekit_characteristic_notify(&fan_on, HOMEKIT_UINT8(cur_speed > 0));
}

void light_setter(homekit_value_t value) {
  if (value.format == homekit_format_uint8) {
    brightness = value.bool_value == 1;
    if (cur_speed > 0)
      digitalWrite(LED[cur_speed], brightness ? HIGH : LOW);
    //brightness = (int) power * 255;
    //analogWrite(LED[cur_speed], brightness);
  }
}

void speed_setter(homekit_value_t value) {
	if (value.format == homekit_format_float) {
    const float power = value.float_value;
    printf("speed is %f\n", power);
    if(power <= 0) {
      choose_relay(0);
      return;
    }
		if(power < 33) {
      choose_relay(1);
      return;
		}
		if(power < 66) {
      choose_relay(2);
      return;
    }
    choose_relay(3);
    return;
	}
  printf("Invalid on-value format: %d\n", value.format);
    return;
}

void active_setter(homekit_value_t value) {
  if (value.format != homekit_format_uint8) {
    printf("Invalid on-value format: %d\n", value.format);
    return;
  }
  const bool state = value.bool_value == 1;
  if (!state) {
    last_state = cur_speed;
  }
  choose_relay(state ? cur_speed == 0 ? last_state : cur_speed : 0);
}

void led_toggle() {
  bool power = !led_state;
  led_state = power;
  led_set_power(power);
  //homekit_characteristic_notify(&fan_on, fan_on.value);
}

void led_blink_task(void *_args) {
	for (int i = 0; i < 3; i++) {
		led_set_power(true);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		led_set_power(false);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	led_set_power(false);
	vTaskDelete(NULL);
}

void led_blink() {
	xTaskCreate(led_blink_task, "led_blink_task", 2048, NULL, 1, NULL);
}

void accessory_identify(homekit_value_t _value) {
	printf("accessory identify\n");
	led_blink();
}

homekit_accessory_t *accessories[] = {
				HOMEKIT_ACCESSORY(
						.id = 1,
						.category = homekit_accessory_category_fan,
						.services=(homekit_service_t*[]){
						  HOMEKIT_SERVICE(ACCESSORY_INFORMATION,
						  .characteristics=(homekit_characteristic_t*[]){
						    &name,
						    HOMEKIT_CHARACTERISTIC(MANUFACTURER, ACCESSORY_MANUFACTURER),
						    &serial_number,
						    HOMEKIT_CHARACTERISTIC(MODEL, ACCESSORY_MODEL),
						    HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0.1"),
						    HOMEKIT_CHARACTERISTIC(IDENTIFY, accessory_identify),
						    NULL
						  }),
						  HOMEKIT_SERVICE(FAN2, .primary=true,
						  .characteristics=(homekit_characteristic_t*[]){
						    HOMEKIT_CHARACTERISTIC(NAME, "Fan"),
						    &fan_on,
                &fan_speed,
						    NULL
						  }),
             HOMEKIT_SERVICE(LIGHTBULB, .primary=false,
             .characteristics=(homekit_characteristic_t*[]){
                HOMEKIT_CHARACTERISTIC(NAME, "Led"),
                &led_on,
                NULL
              }),
						  NULL
						}),
				NULL
		};

homekit_server_config_t config = {
		.accessories = accessories,
		.password = "111-11-111",
		//.on_event = on_homekit_event,
		.setupId = "ABCD"
};

void accessory_init() {
	pinMode(LED[0], OUTPUT);
 pinMode(LED[1], OUTPUT);
 pinMode(LED[2], OUTPUT);
 pinMode(LED[3], OUTPUT);
 pinMode(RELAY[0], OUTPUT);
 pinMode(RELAY[1], OUTPUT);
 pinMode(RELAY[2], OUTPUT);
 digitalWrite(RELAY[0], HIGH);
  digitalWrite(RELAY[1], HIGH);
  digitalWrite(RELAY[2], HIGH);
	led_set_power(false);
	//Rename ACCESSORY_NAME base on MAC address.
	uint8_t mac[6];
	esp_wifi_get_mac(WIFI_IF_STA, mac);
	sprintf(ACCESSORY_NAME, "ESP32_LED_%02X%02X%02X", mac[3], mac[4], mac[5]);
}
