# gphotoShow

Tool to capture images triggered by a IR light barrier and display captured images on a screen.

An IR LED and IR sensor are connected to an Arduino that runs Firmata firmware. The sensor values
are read by gphotoShow, image capture is triggered by gphoto2 and captured images are displayed full screen.
The trigger threshold may be configured in calibration mode.

Runs on Raspberry Pi or other Linux machines.

Requirements:
- gphoto2
- openFrameworks 0.9.8
