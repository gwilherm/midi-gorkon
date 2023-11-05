# Arduino ProMicro MIDI Gorkon

Arduino SysEx programmable 8 Knobs MIDI USB Controller

## Photo

![](doc/photo.jpg)

## Breadboard

![](doc/schematics/midi-gorkon_bb.png)

## Schematics

![](doc/schematics/midi-gorkon_schema.png)

## Fritzing parts

[KY-023 XY Joystick (jorgechacblogspot)](https://github.com/jorgechacblogspot/librerias_fritzing/raw/main/KY-023%20Dual%20Axis%20Joystick%20Module.fzpz)

## Flash Arduino ProMicro
### Arduino IDE

* Open midi-gorkon/midi-gorkon.ino
* Sketch menu
  * Card type: `Arduino Micro`
  * Port: `/dev/ttyACM0`
  * Upload

### Command line (Ubuntu/Debian)

```shell
sudo apt install arduino-mk
cd midi-gorkon
make
make upload
```

## Gorkon Configuration App (Ubuntu/Debian)

```shell
sudo apt install python3-rtmidi
cd gorkon-config-app
python3 -m venv venv
source venv/bin/activate
python3 -m pip install -r requirements.txt
# Launch using Jack
./gorkon-config-app.py
# Launch using Alsa
./gorkon-config-app.py -a
```

![](doc/gorkon-config-app.png)
