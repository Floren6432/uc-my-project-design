# uc-project-design
This repository contains the source codes developed for my Project Design subject during my 4th year of studies at university (S.Y. 2024–2025).

<p align="center">
  <img src="https://github.com/Floren6432/uc-my-project-design/blob/main/gifs/ChairGif.gif" alt="animated" />
</p>

**Features**
1. A pyton script that reads inputs from the Logitech G29 steering wheel using the <ins>pygame</ins> library and translates steering angles into directional commands which are "**LEFT**", "**RIGHT**", and "**NEUTRAL**". These **_commands_** will be sent to a connected Arduino via <ins>pyserial</ins>.
2. Arduino code interprets incoming commands and drives two BTS7960 motor drivers to control dual linear actuators.

<p align="center">
  <img src="https://github.com/Floren6432/uc-my-project-design/blob/main/gifs/ChairAnimationClose.gif" alt="animated" />
</p>

<p align="center">
  <img src="https://github.com/Floren6432/uc-my-project-design/blob/main/gifs/ChairAnimationFar.gif" alt="animated" />
</p>

**Requirements**
1. Arduino IDE
2. Python (Since the development of the project, codes that were written in Python have been in version 3.13)

**Schematic**
<p align="center">
  <img src = http://github.com/Floren6432/uc-my-project-design/blob/main/Schematic%20Diagram/Schematic_Schematic_of_Immersive_Chair_Base.png />
</p>

**Note**
1. The steering angle thresholds for determining "LEFT" (≥ +60°), "RIGHT" (≤ –60°), and "NEUTRAL" (between –60° and +60°) are hardcoded in the Python script, **_wheel_code_project_design.py_**. You can change these by modifying the values in the conditional statements within the script. For example, in  **_LINE 33_** "if steering_angle <= -60:", if we want this to be like –120° then we can write this as "if steering_angle <= -120:".
2. The system uses time in seconds to control how far the actuators extend or retract, longer durations correspond to greater tilt. I've also included codes (.ino files in the **Debug Actuators** folder) that can debug each of actuators, which allows manual testing of actuator movements to help determine the correct duration needed for specific tilt angles or heights. 1000 = 1 second which is 1mm of stroke.
