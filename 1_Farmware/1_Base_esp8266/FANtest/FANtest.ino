/*
   fan.ino

   Created on: 07-04-2021
     Author: KUNAL VERMA

   HAP section 8.38 Switch
   An accessory contains a switch.

   This example shows how to:
   1. define a fan accessory and its characteristics (in my_accessory.c).
   2. get the fan-event sent from iOS Home APP.
   3. report the fan value to HomeKit.

   You should:
   1. add library from github https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266
   2. change ssid and password in wifi_info.h.
   3. erase the full flash or call homekit_storage_reset() in setup()
      to remove the previous HomeKit pairing storage and
      enable the pairing with the new accessory of this new HomeKit example.

   For Connection.
   1. CONNECT SOLID STATE RELAY TO PIN 5
   2. CONNECT SOLID STATE RELAY TO PIN 4
      

   This example code is only for personal/hobby use not for sale.   
*/

#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"
#include "ESPButton.h"

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

bool fan_con = false;   // bool to store fan_condition if_True( Fan is ON) if_False( Fan is OFF)
float set_pwm = 0;      // Store the pwm vale to give to fan from 0 to 1024 
float per_send = 0;     // Store the percentage vale to give back to ios app.

#define RELAY 5      // CONNECT SOLID STATE RELAY TO PIN 5
#define buttonpin 4  // CONNECT SOLID STATE RELAY TO PIN 4

void setup() {
  Serial.begin(115200);
  wifi_connect(); // in wifi_info.h
  pinMode(RELAY, OUTPUT);
  pinMode(buttonpin, INPUT_PULLUP);
  homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
  my_homekit_setup();
}

void loop() {
  my_homekit_loop();
  delay(10);
}

//==============================
// HomeKit setup and loop
//==============================

// access your HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t fan_on;
extern "C" homekit_characteristic_t rotation_speed;

static uint32_t next_heap_millis = 0;

//Called when the fan value is changed by iOS Home APP
void fan_on_setter(const homekit_value_t value)
{ bool fan = value.bool_value;
  if (fan == 1)
  { bool fan_con = true;
    fan_on.value.bool_value = fan_con;
    per_send = map(set_pwm, 0, 1024, 0, 100);                               //converting values from pwm(0-1024) to %(0-100)
    rotation_speed.value.float_value = per_send;                             
    homekit_characteristic_notify(&fan_on, fan_on.value);                   // sending value to ios app
    homekit_characteristic_notify(&rotation_speed, rotation_speed.value);   // sending value to ios app
    analogWrite(RELAY, set_pwm);
    Serial.println("FAN IS ON NOW !!!!!");
  }
  else
  {
    bool fan_con = false;
    fan_on.value.bool_value = fan_con;                                       
    rotation_speed.value.float_value = 0;                                    
    homekit_characteristic_notify(&fan_on, fan_on.value);                    // sending value to ios app
    homekit_characteristic_notify(&rotation_speed, rotation_speed.value);    // sending value to ios app
    analogWrite(RELAY, 0);
    Serial.println("FAN IS OFF NOW !!!!!");
  }
}

//Called when the rotatio//speed value is changed by iOS Home APP

void rotation_speed_setter(const homekit_value_t v)
{ float per = v.float_value;
  per = map(per, 0, 100, 0, 1024);                                       // converting values from %(0-100) to pwm(0-1024) 
  analogWrite(RELAY, per);                                               // Setting the fan speed 
  Serial.println(per);
  set_pwm = per;                                                         // store pwm values to other variable
  per_send = map(set_pwm, 0, 1024, 0, 100);                              // converting values from pwm(0-1024) to %(0-100)
  rotation_speed.value.float_value = per_send;                           
  homekit_characteristic_notify(&rotation_speed, rotation_speed.value);  // sending value to ios app
}

void my_homekit_setup() {

  //Add the .setter function to get the switch-event sent from iOS Home APP.
  //The .setter should be added before arduino_homekit_setup.
  //HomeKit sever uses the .setter_ex internally, see homekit_accessories_init function.
  //Maybe this is a legacy design issue in the original esp-homekit library,
  //and I have no reason to modify this "feature".

  fan_on.setter = fan_on_setter;
  
  rotation_speed.setter = rotation_speed_setter;
  
  arduino_homekit_setup(&config);



  //  This function helps to change the value of the fan from on to off and vice-versa.    
  ESPButton.add(0, buttonpin, LOW, true, true);
  ESPButton.setCallback([&](uint8_t id, ESPButtonEvent event) {

    if (event == ESPBUTTONEVENT_SINGLECLICK) {
      if (analogRead(RELAY) == 0)
      {
        bool fan_con = true;
        fan_on.value.bool_value = fan_con;

        per_send = map(set_pwm, 0, 1024, 0, 100);
        rotation_speed.value.float_value = per_send;
        homekit_characteristic_notify(&fan_on, fan_on.value);
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
        analogWrite(RELAY, set_pwm);
        Serial.println("FAN IS ON NOW !!!!!");
      }
      else
      {
        bool fan_con = false;
        fan_on.value.bool_value = fan_con;
        rotation_speed.value.float_value = 0;
        homekit_characteristic_notify(&fan_on, fan_on.value);
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
        analogWrite(RELAY, 0);
        Serial.println("FAN IS OFF NOW !!!!!");
      }
    }

  });
  ESPButton.begin();
}



void my_homekit_loop() {
  arduino_homekit_loop();
  ESPButton.loop();
  const uint32_t t = millis();
  if (t > next_heap_millis) {
    // show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d",
          ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

  }
}
