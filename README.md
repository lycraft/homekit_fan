## homekit_fan

ESP8266方案基于[arduino-homekit-esp](https://gitee.com/mirrors_Mixiaoxiao/Arduino-HomeKit-ESP8266?_from=gitee_search)库

#### 引脚对应

| ESP_GPIO | ESP_Char | Use            | BOOT |
| -------- | -------- | -------------- | ---- |
| GPIO16   | D0       | LED            | 高   |
| GPIO5    | D1       | 继电器(总开关) |      |
| GPIO4    | D2       | 继电器(低速)   |      |
| GPIO0    | D3       | 按键总开关     | N低  |
| GPIO2    | D4       | 按键低速       | 高   |
| GPIO14   | D5       | 继电器(中速)   |      |
| GPIO12   | D6       | 继电器(高速)   |      |
| GPIO13   | D7       | 按键中速       |      |
| GPIO15   | D8       | 按键高速       | N高  |
| ADC      | A0       |                |      |

#### 怎么使用？

* 在vscode中搜索platformio插件并下载。
* 打开firmware文件夹中的platformio工程
* 根据WiFi信息修改wifi_info.h文件中的ssid和password
* 编译下载进esp8266
* 苹果手机的家庭APP右上角配对，配对码为11122333

---

完成esp32版本的开发会回来好好写文档的