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

      
*/

#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"
#include "ESPButton.h"

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

void setup() {
    Serial.begin(115200);
    wifi_connect(); // in wifi_info.h
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
extern "C" homekit_characteristic_t fan_on;                         //风扇开关
extern "C" homekit_characteristic_t rotation_speed;                 //速度调节
extern "C" homekit_characteristic_t cha_switch_on0;  

static uint32_t next_heap_millis = 0;

bool fan_con = false;   // bool to store fan_condition if_True( Fan is ON) if_False( Fan is OFF)
float set_pwm = 0;      // Store the pwm vale to give to fan from 0 to 1024 
float per_send = 0;     // Store the percentage vale to give back to ios app.       存储百分比值以回馈给 ios 应用程序。

#define relay0 5        // 总开关继电器
#define relay1 4        // 低速继电器
#define relay2 14       // 中速继电器
#define relay3 12       // 高速继电器

#define button0 0      // 总开关按键
#define button1 2       // 低速按键
#define button2 13       // 中速按键
#define button3 15      // 高速按键

#define PIN_SWITCH0 16

//Called when the fan value is changed by iOS Home APP                  iOS Home APP更改风扇值(开关)时调用
void fan_on_setter(const homekit_value_t value)
{ 
    bool fan = value.bool_value;
    if (fan == 1)
    {
        bool fan_con = true;
        fan_on.value.bool_value = fan_con;
        per_send = map(set_pwm, 0, 3, 0, 100);                               // 将值从 pwm(0-1024) 转换为 %(0-100)
        rotation_speed.value.float_value = per_send;                             
        homekit_characteristic_notify(&fan_on, fan_on.value);                   // sending value to ios app
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);   // sending value to ios app
        // analogWrite(relay0, set_pwm);
        digitalWrite(relay0, HIGH);                                             //闭合总开关继电器
        Serial.println("FAN IS ON NOW !!!!!");
    }
    else
    {
        bool fan_con = false;
        fan_on.value.bool_value = fan_con;                                       
        rotation_speed.value.float_value = 0;                                    
        homekit_characteristic_notify(&fan_on, fan_on.value);                    // sending value to ios app
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);    // sending value to ios app
        digitalWrite(relay0, LOW);
        Serial.println("FAN IS OFF NOW !!!!!");
    }
}

//Called when the switch value is changed by iOS Home APP               当收到传来的信息执行的函数
void cha_switch_on_setter0(const homekit_value_t value) {
	bool on = value.bool_value;
	cha_switch_on0.value.bool_value = on;	//sync the value
	LOG_D("Switch0: %s", on ? "ON" : "OFF");
	digitalWrite(PIN_SWITCH0, on ? LOW : HIGH);
}

//Called when the rotatio speed value is changed by iOS Home APP       iOS Home APP改变转速值(转速)时调用

void rotation_speed_setter(const homekit_value_t v) 
{ 
    float per = v.float_value;
    per = map(per, 0, 100, 0, 3);                                       // converting values from %(0-100) to pwm(0-1024) 
    analogWrite(relay0, per);                                               // Setting the fan speed 
    Serial.println(per);
    set_pwm = per;                                                         // store pwm values to other variable
    per_send = map(set_pwm, 0, 3, 0, 100);                              // converting values from pwm(0-1024) to %(0-100)
    rotation_speed.value.float_value = per_send;                           
    homekit_characteristic_notify(&rotation_speed, rotation_speed.value);  // sending value to ios app
}

void my_homekit_setup() {

    pinMode(relay0, OUTPUT);
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(button0, INPUT_PULLUP);
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(PIN_SWITCH0, OUTPUT);
	digitalWrite(PIN_SWITCH0, HIGH);

  //Add the .setter function to get the switch-event sent from iOS Home APP.                    添加.setter函数，获取iOS Home APP发送的switch-event。
  //The .setter should be added before arduino_homekit_setup.                                   .setter 应该在 arduino_homekit_setup 之前添加
  //HomeKit sever uses the .setter_ex internally, see homekit_accessories_init function.        HomeKit 服务器在内部使用 .setter_ex，请参阅 homekit_accessories_init 函数。
  //Maybe this is a legacy design issue in the original esp-homekit library,                    也许这是原始 esp-homekit 库中的遗留设计问题，
  //and I have no reason to modify this "feature".                                              我没有理由修改这个“功能”。
    fan_on.setter = fan_on_setter;
    cha_switch_on0.setter = cha_switch_on_setter0;
    rotation_speed.setter = rotation_speed_setter;
    arduino_homekit_setup(&config);



  //  This function helps to change the value of the fan from on to off and vice-versa.    此功能有助于将风扇的值从打开更改为关闭，反之亦然
  // uint8_t _id, uint8_t _pin, uint8_t _pin_down_digital, bool _doubleclick_enable = false, bool _longclick_enable = true
    ESPButton.add(0, button0, LOW, true, true);
    // ESPButton.add(0, button1, LOW, true, true);
    // ESPButton.add(0, button2, LOW, true, true);
    // ESPButton.add(0, button3, LOW, true, true);
    ESPButton.setCallback([&](uint8_t id, ESPButtonEvent event) {

    if (event == ESPBUTTONEVENT_SINGLECLICK) //单击按键时
    {
        if (digitalRead(relay0) == 0)
        {
            bool fan_con = true;                //闭合继电器
            fan_on.value.bool_value = fan_con;

            per_send = map(set_pwm, 0, 3, 0, 100);
            rotation_speed.value.float_value = per_send;
            homekit_characteristic_notify(&fan_on, fan_on.value);
            homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
            // analogWrite(relay0, set_pwm);
            digitalWrite(relay0,HIGH);
            Serial.println("FAN IS ON NOW !!!!!");
        }
        else
        {
            bool fan_con = false;               //松开继电器
            fan_on.value.bool_value = fan_con;
            rotation_speed.value.float_value = 0;
            homekit_characteristic_notify(&fan_on, fan_on.value);
            homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
            digitalWrite(relay0, LOW);
            Serial.println("FAN IS OFF NOW !!!!!");
        }
    }

  });
  ESPButton.begin();
}



void my_homekit_loop() 
{
    arduino_homekit_loop();
    ESPButton.loop();
    const uint32_t t = millis();
    if (t > next_heap_millis) 
    {
        // show heap info every 5 seconds               每 5 秒显示一次堆信息
        next_heap_millis = t + 5 * 1000;
        LOG_D("Free heap: %d, HomeKit clients: %d",
            ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

    }
}
