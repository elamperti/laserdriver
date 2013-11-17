laserdriver
===========

Arduino + servos + laser = drawings :D

This readme is not intended to be detailed. For more information about this project see [this post](http://blog.elamperti.com/2013/11/drawing-with-a-laser/).


Driver
------

The driver uses two servos (X and Y axis) to move the laser pointer. Right now the drawing array is hardcoded. To change the drawing use the *Creator*.
Once set up, you can use the commands below via serial connection (9600 baud). Remember to end your lines with ';'

  * **go** Will start drawing immediately. Note: It will only accept 'stop' command while it's drawing.
  * **stop** Stops drawing.
  * **servo [x|y] [min|max] [_value_|read]** Defines the min/max limits of each axis. A reasonable value should be between 600 and 2300. If 'read' is used instead of a numeric value, the program will read the potentiometer (and move the servo accordingly)
  * **walk delay _value_** Defines the time (miliseconds) the laser will stay at each point of a line (between two defined coordinates)
  * **step delay _value_** Defines the time (miliseconds) the program will wait for the servos to reach a new coordinate when the laser is off
  * **wait delay _value_** Defines the time (miliseconds) the laser will stay at each coordinate point


Creator
-------

This tool was made in Processing to create a drawing array of coordinates to be used later on the *Driver*. Usage:

1. Edit the code to load the SVG you want to use, or comment the line `shape(backdrop, 0,0, 800, 800);` if you don't want one.
2. Drawing!
  * **Right click** to move from the previous point to a new point without drawing with the laser.
  * **Left click** to define a visible line from the previous point to the new one.
  * Use the **delete** key to undo the last point. You can undo the whole drawing.
3. When finished, press **Enter**, choose a filename and a text file will be created. The text file contains the array ready to use (copy and paste into the code).

The array will work on both the *Driver* and the *Simulator*.


Simulator
---------

Simulates (approximately) the laser drawing. The green lines are servo movements, the red ones are laser (drawn) lines.
Usage: just edit the code to use your drawing coordinates and run it!

