# Arduino ProMicro MIDI Gorkon

Arduino SysEx programmable 8 Knobs MIDI USB Controller using Arduino ProMicro

## Photo

![](doc/photo.jpg)

## Breadboard

![](doc/schematics/midi-gorkon_bb.png)

## Schematics

![](doc/schematics/midi-gorkon_schema.png)

## Fritzing parts

[KY-023 XY Joystick (jorgechacblogspot)](https://github.com/jorgechacblogspot/librerias_fritzing/raw/main/KY-023%20Dual%20Axis%20Joystick%20Module.fzpz)

## Compile and flash firmware

### Arduino IDE

#### Install libraries

Sketch > Include Library > Manage Libraries

* MIDIUSB (1.0.5)
* USB-MIDI (1.1.2)
* Control Surface (2.0.0)
* Adafruit NeoPixel (1.12.5)

#### Compile and flash

* Open firmware/firmware.ino
* Sketch menu
  * Card type: `Arduino Micro`
  * Port: `/dev/ttyACM0`
  * Upload

### Command line

#### Install arduino-cli

##### Ubuntu/Debian

```shell
sudo apt install arduino-cli
```

##### Arch/Manjaro

```shell
sudo pacman -s arduino-cli
```

#### Compile and flash

```shell
cd firmware
make
make upload
```

## Gorkon Configuration App (Ubuntu/Debian)

```shell
# Install dependencies
sudo apt install python3-rtmidi

# Launch using Jack
./gorkon-config
# Launch using Alsa
./gorkon-config -a
```

![](doc/gorkon-config-app.png)
