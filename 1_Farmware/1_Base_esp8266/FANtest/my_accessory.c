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

// Fan (HAP section 8.13)
// required: Active
// optional: ROTATION_SPEED
//           ROTATION_DIRECTION
//           NAME

//一个电风扇总开关
homekit_characteristic_t fan_on = HOMEKIT_CHARACTERISTIC_(ON, false);
//一个控制LED
homekit_characteristic_t cha_switch_on0 = HOMEKIT_CHARACTERISTIC_(ON, false);

//三档调节风扇速度
homekit_characteristic_t rotation_speed = HOMEKIT_CHARACTERISTIC_(ROTATION_SPEED, per_send);
// format: string; HAP section 9.62; max length 64
homekit_characteristic_t cha_name0 = HOMEKIT_CHARACTERISTIC_(NAME, "Switch0");

homekit_accessory_t *accessories[] = {
  HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_fan, .services = (homekit_service_t*[]) {           //.id后边的数字代表带几个设备，>1就是作为桥连接
    HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "FANtest"),
      HOMEKIT_CHARACTERISTIC(MANUFACTURER, "lycraft_Person"),
      HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "666666"),
      HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266"),
      HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
      HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
      NULL
    }),
    HOMEKIT_SERVICE(FAN, .primary = true, .characteristics = (homekit_characteristic_t*[]) {                             //建一个FAN类型服务
      &fan_on,
      &rotation_speed,
      NULL
    }),
    HOMEKIT_SERVICE(SWITCH, .characteristics=(homekit_characteristic_t*[]){                          //新建一个服务 switch类型
			&cha_switch_on0,
			&cha_name0,
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

