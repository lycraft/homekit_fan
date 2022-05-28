/*
   my_accessory.c
   Define the accessory in C language using the Macro in characteristics.h

       Created on: 07-04-2021
         Author: KUNAL VERMA
*/

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void my_accessory_identify(homekit_value_t _value) {
  printf("accessory identify\n");
}

// Switch (HAP section 8.38)
// required: ON
// optional: ROTATION_SPEED
//           ROTATION_DIRECTION
//           NAME


homekit_characteristic_t fan_on = HOMEKIT_CHARACTERISTIC_(ON, false);

homekit_characteristic_t rotation_speed = HOMEKIT_CHARACTERISTIC_(ROTATION_SPEED, per_send);


homekit_accessory_t *accessories[] = {
  HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_fan, .services = (homekit_service_t*[]) {
    HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "FAN"),
      HOMEKIT_CHARACTERISTIC(MANUFACTURER, "HOMEKIT"),
      HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "1234567"),
      HOMEKIT_CHARACTERISTIC(MODEL, "FAN"),
      HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
      HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
      NULL
    }),
    HOMEKIT_SERVICE(FAN, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
      &fan_on,
      &rotation_speed,
      NULL
    }),
    NULL
  }),
  NULL
};

homekit_server_config_t config = {
  .accessories = accessories,
  .password = "111-11-111"
};

