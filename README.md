# WifiSimpleGetServer
arduino source code to create a simple Wifi server reading GET parameters

Introduction
-------

This project is a very simple extension of the `WifiWebServer` that adds the possibility to read the GET parameters of the request URL. This example code does nothing but display the GET parameters in the reply.

This Arduino code was tested only with a Genuino MKR1000.

`WifiSimpleGetServer` is the minimal version of the code.
`WifiSimpleGetServerDebug` outputs some debug information on `Serial`.
`WifiGetServerLED` turns on and off the onboard LED (pin 6).

Requirements
-------

* Download Arduino: http://arduino.cc/en/Main/Software
* Install WiFi101 library: https://github.com/arduino-libraries/WiFi101
* Install MKR1000 drivers: https://create.arduino.cc/projecthub/charifmahmoudi/arduino-mkr1000-getting-started-08bb4a
* Get a Genuino/Arduino MKR1000 board (or equivalent Arduino + WiFi shield)

How to test
-------

* Load the sketch in Arduino IDE
* Update the SSID and password to match your local network
* Compile and upload to your board
* Open the Serial Monitor and wait for the board to connect
* Copy the IP address, for instance 192.168.0.35
* Open a web browser (on a device also connected to your local network) and go to:
  * `http://192.168.0.35/?c=1&d=2`
* The browser should display
  * `c = 1`
  * `d = 2`

How to do your own `WifiGetServerAndDoSomething`
-------

* Follow the previous steps to be sure it works properly
* Edit function `String handle(String **params, int nb)`
  * `String **params` is the list of GET parameters read from the request: `[["a", "1"], ["b", "2"]]`
  * `nb` is the size of `params`, useful to create loops
  * The returned `String` value will be inserted into the body of the reply.
  * The only limit now is your imagination and your board :) (See `WifiGetServerLED` for an example). 
* Compile and upload to your board
