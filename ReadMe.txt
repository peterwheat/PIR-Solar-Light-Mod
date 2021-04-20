PIR Solar Light Mod Notes
=========================

Background
==========

1.  Watch Big Cllive's video: 100 LED solar garden light teardown (with schematic).
    https://www.youtube.com/watch?v=DH4zTmrdc1o

N.B. The circuits in these lights varey.  
Some have a dedicated bettery overcharge protection chip, 
Some have just one chip the does everything, and switchs off charging via a transistor.
Some have no on board protection, these may or may not have protection on the battery.
If not, you will need to get a 1C 18650 battery protection pcb off ebay.

All the models I have seen use a regular (non-logic level) PIR sensor (e.g. D203S).
The main chip handles amplificaion and signal processing.

Mod details
===========

Remove main controller chip.
Remove PIR Sensor, cut tracks to sensor, and put in a logicic level sensor (AM312).
This requires removing the sensor from the board, mounting the AM312 sensor on the Light's pcb,
then re-attach that AM312 pcb to the sensor legs.
Attach tap wires to the back of the resistor that drives the LEDs,
and to the centre of the voltage divider, used to read the solar panel voltage.

The controlling MCU is an ATTiny85.  You can program this using an Arduino Uno as the ISP.
The ATTiny85 sleeps until movement is detected (woken via PIR interupt).

ATTiny85 Pinout:

1 - Reset (N/C)
2 - Solar IN
3 - PIR IN
4 - GND
5 - SNDMSG (conntected to one input of "Mark's Bench" latch circuit.)
6 - LED Light OUT
7 - N/C (could be used for button?)
8 - VCC (from 18650)

The main loop looks like this:

  if(readVcc()<3000){  //Is battery really low?
    sleep();  
  } else {
    sleep();  //Sleep until movement detected.
    if(dark()) flash();
    sendMsg();
  }

Install program: ATTiny85

There are two other components: A "Mark's Bench" latch circuit used to control the power to an ESP 01 that sends the MQTT message.

For Mark's Bench Latch Circuit see:
https://marksbench.com/electronics/esp32-cam-low-power-trail-camera/
I found IRLZ44N "logic level" mosfet to be a much better choice (no opto-couplers required).

The ESP 01 is powered via an AMS1117 with two cap (IN 10uF, OUT: 22uF).

Install program: ESP01

MQTT Broker
===========

I use a Pi Zero W along with the dietpi image shared by Andreas Spiess.  See:

#126​ Cheap MQTT Broker on Raspberry Zero W / DietPi / MQTT Message Logger SQlite / PHPliteAdmin
https://www.youtube.com/watch?v=bpg6RSHS4Zc

MQTT Client
===========

You can either install a phone app, or make a dedicated pager device.  
See sample code: MQTTNotify
Uses Wemos D1, 2x16 I2C Display, Active Buzzer, ds3231 Chronodot Real Time Clock

NO WARRENTY EXPRESSED OR IMPLIED.  You make these modifications at your own risk.  
There is no support.  You are on your own.  Good luck!
