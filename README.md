# Tap-Water-Control-System

This a tap water spent controlling system. Each faucet has a usage of water limit. When the limit has been reached, this system shuts the tap down autonomaticly. The ml values ​​flowing from the tap are renewed at the beginning of each month and the taps open.

As an example: A tap's limit is 3L per month. When it completes 3L, it shuts the tap down. The following month, whether or not the previous month completed 3L, it can be use 3L again.

This system runs by ESP32, saves the tap water datas to the Firebase database system. And the ESP32 module was programmed by Arduino IDE Software.

Also you can see the tap water datas at the android application. This aplication was made by App Inventor.
