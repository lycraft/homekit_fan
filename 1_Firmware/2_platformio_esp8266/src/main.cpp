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

void my_homekit_setup();
void my_homekit_loop();

void setup() {
    Serial.begin(115200);
    wifi_connect(); // in wifi_info.h
    // homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
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
float set_pwm = 1;      // Store the pwm vale to give to fan from 0 to 3 
float per_send = 0;     // Store the percentage vale to give back to ios app.       存储百分比值以回馈给 ios 应用程序。

#define relay0 5        // 总开关继电器     D1
#define relay1 4        // 低速继电器       D2
#define relay2 14       // 中速继电器       D5
#define relay3 12       // 高速继电器       D6

#define button0 0      // 总开关按键        D3
#define button1 2       // 低速按键         D4
#define button2 13       // 中速按键        D7
#define button3 15      // 高速按键         D8

#define PIN_SWITCH0 16  //LED              D0

//Called when the fan value is changed by iOS Home APP                  iOS Home APP更改风扇值(开关)时调用
void fan_on_setter(const homekit_value_t value)
{ 
    bool fan = value.bool_value;
    if (fan == 1)                       //开启风扇总开关
    {
        bool fan_con = true;
        fan_on.value.bool_value = fan_con;
        per_send = map(set_pwm, 0, 3, 0, 100);                                  // 将值从 pwm(0-1024) 转换为 %(0-100)
        rotation_speed.value.float_value = per_send;                             
        homekit_characteristic_notify(&fan_on, fan_on.value);                   // sending value to ios app
        // homekit_characteristic_notify(&rotation_speed, rotation_speed.value);   // sending value to ios app
        // analogWrite(relay0, set_pwm);
        digitalWrite(relay0, HIGH);                                             //闭合总开关继电器   
        if(set_pwm == 1)
        {
            digitalWrite(relay2,LOW);
            digitalWrite(relay3,LOW);
            delay(20);
            digitalWrite(relay1,HIGH);
        }
        else if(set_pwm == 2)
        {
            digitalWrite(relay1,LOW);
            digitalWrite(relay3,LOW);
            delay(20);
            digitalWrite(relay2, HIGH);
        }
        else if(set_pwm == 3)
        {
            digitalWrite(relay2,LOW);
            digitalWrite(relay1,LOW);
            delay(20);
            digitalWrite(relay3, HIGH);
        }
        Serial.println("FAN IS ON NOW !!!!!");
    }
    else
    {
        bool fan_con = false;
        fan_on.value.bool_value = fan_con;                                       
        rotation_speed.value.float_value = 0;                                    
        homekit_characteristic_notify(&fan_on, fan_on.value);                    // sending value to ios app
        // homekit_characteristic_notify(&rotation_speed, rotation_speed.value);    // sending value to ios app
        digitalWrite(relay0, LOW);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
        Serial.println("FAN IS OFF NOW !!!!!");
    }
}

//Called when the switch value is changed by iOS Home APP               当收到传来的信息执行的函数
void cha_switch_on_setter0(const homekit_value_t value) {
	bool on = value.bool_value;
	cha_switch_on0.value.bool_value = on;	//sync the value
	LOG_D("Switch0: %s", on ? "ON" : "OFF");
	digitalWrite(PIN_SWITCH0, on ? HIGH : LOW);
}

//Called when the rotatio speed value is changed by iOS Home APP       iOS Home APP改变转速值(转速)时调用

void rotation_speed_setter(const homekit_value_t v) 
{ 
    bool fan_con = false;
    fan_on.value.bool_value = fan_con;
    float per = v.float_value;
    per = map(per, 0, 100, 0, 3);                                           // converting values from %(0-100) to pwm(0-1024) 
    // analogWrite(relay0, per);                                            // Setting the fan speed 
    if(per >= 1)
    {
        digitalWrite(relay0, HIGH);
        Serial.println(per);
        set_pwm = per;                                                          // store pwm values to other variable
        per_send = map(set_pwm, 0, 3, 0, 100);                                  // converting values from pwm(0-1024) to %(0-100)
        rotation_speed.value.float_value = per_send;                           
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);   // sending value to ios app
        if(per == 1)
        {
            digitalWrite(relay2,LOW);
            digitalWrite(relay3,LOW);
            delay(20);
            digitalWrite(relay1,HIGH);
        }
        else if(per == 2)
        {
            digitalWrite(relay1,LOW);
            digitalWrite(relay3,LOW);
            delay(20);
            digitalWrite(relay2, HIGH);
        }
        else if(per == 3)
        {
            digitalWrite(relay2,LOW);
            digitalWrite(relay1,LOW);
            delay(20);
            digitalWrite(relay3, HIGH);
        }
    }
    else
    {
        bool fan_con = false;
        fan_on.value.bool_value = fan_con; 
        digitalWrite(relay0, LOW);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
        Serial.println(per);
        set_pwm = per;                                                          // store pwm values to other variable
        per_send = map(set_pwm, 0, 3, 0, 100);                                  // converting values from pwm(0-1024) to %(0-100)
        rotation_speed.value.float_value = 0;                
        homekit_characteristic_notify(&fan_on, fan_on.value);                    // sending value to ios app           
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);   // sending value to ios app
    }
}

void my_homekit_setup() {

    pinMode(relay0, OUTPUT);
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(button0, INPUT_PULLUP);
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT);
    pinMode(PIN_SWITCH0, OUTPUT);
	digitalWrite(PIN_SWITCH0, LOW);

  //Add the .setter function to get the switch-event sent from iOS Home APP.                    
  //The .setter should be added before arduino_homekit_setup.                                   
  //HomeKit sever uses the .setter_ex internally, see homekit_accessories_init function.        
  //Maybe this is a legacy design issue in the original esp-homekit library,                    
  //and I have no reason to modify this "feature".                                              
    fan_on.setter = fan_on_setter;
    cha_switch_on0.setter = cha_switch_on_setter0;
    rotation_speed.setter = rotation_speed_setter;
    arduino_homekit_setup(&config);



  //  This function helps to change the value of the fan from on to off and vice-versa.    此功能有助于将风扇的值从打开更改为关闭，反之亦然
  // uint8_t _id, uint8_t _pin, uint8_t _pin_down_digital, bool _doubleclick_enable = false, bool _longclick_enable = true
    ESPButton.add(0, button0, LOW, true, true);
    ESPButton.add(1, button1, LOW, true, true);
    ESPButton.add(2, button2, LOW, true, true);
    ESPButton.add(3, button3, HIGH, true, true);
    ESPButton.setCallback([&](uint8_t id, ESPButtonEvent event) {
        if(id == 0 && event == ESPBUTTONEVENT_LONGCLICK){               //长按按键0重置与手机绑定信息
            homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
        }

    if (id == 0 && event == ESPBUTTONEVENT_SINGLECLICK)    //单击按键0时,决定是否开启总开关
    {
        if (digitalRead(relay0) == 0)           //当继电器总开关未启动，则启动也就是闭合继电器
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

            
            if(set_pwm == 1)
            {
                digitalWrite(relay2,LOW);
                digitalWrite(relay3,LOW);
                delay(20);
                digitalWrite(relay1,HIGH);
            }
            else if(set_pwm == 2)
            {
                digitalWrite(relay1,LOW);
                digitalWrite(relay3,LOW);
                delay(20);
                digitalWrite(relay2, HIGH);
            }
            else if(set_pwm == 3)
            {
                digitalWrite(relay2,LOW);
                digitalWrite(relay1,LOW);
                delay(20);
                digitalWrite(relay3, HIGH);
            }

        }
        else
        {
            bool fan_con = false;               //松开继电器
            fan_on.value.bool_value = fan_con;
            rotation_speed.value.float_value = 0;
            homekit_characteristic_notify(&fan_on, fan_on.value);
            homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
            digitalWrite(relay0, LOW);
            digitalWrite(relay1, LOW);
            digitalWrite(relay2, LOW);
            digitalWrite(relay3, LOW);
            Serial.println("FAN IS OFF NOW !!!!!");
        }
    }
    else if(id == 1 && event == ESPBUTTONEVENT_SINGLECLICK)     //按键1对应低速
    {
        rotation_speed.value.float_value = 33;
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
        digitalWrite(relay2,LOW);
        digitalWrite(relay3,LOW);
        delay(20);
        digitalWrite(relay1,HIGH);
    }
    else if(id == 2 && event == ESPBUTTONEVENT_SINGLECLICK)     //按键2对应中速
    {   
        rotation_speed.value.float_value = 67;
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
        digitalWrite(relay1,LOW);
        digitalWrite(relay3,LOW);
        delay(20);
        digitalWrite(relay2,HIGH);
    }
    else if(id == 3 && event == ESPBUTTONEVENT_SINGLECLICK)     //按键3对应高速
    {
        rotation_speed.value.float_value = 100;
        homekit_characteristic_notify(&rotation_speed, rotation_speed.value);
        digitalWrite(relay2,LOW);
        digitalWrite(relay1,LOW);
        delay(20);
        digitalWrite(relay3,HIGH);
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
