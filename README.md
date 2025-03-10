A diagnostic disk for testing various Apple II functions

[Download](https://github.com/markadev/AppleII-test/releases)

 * `MODETEST` - test the different Apple II and IIe video modes
 * `BANKTEST` - test IIe memory write behavior under various combinations of soft-switches
 * `MOUSE.MOVE` - Demo from the Apple Mouse Interface manual to read the mouse position
 * `SPLIT` - Apple video split-screen demo by Bob Bishop, Softalk, October 1982


## Running the image

Write the disk image (`AppleTest.dsk`) to a floppy or load it onto a disk emulator and boot it on
an Apple II+ or Apple IIe. You can `RUN` or `BRUN` the programs from the basic prompt

```
]BRUN MODETEST
```


## Building the image

This is generally easiest to build in a Linux-like environment. You will need:
 * cc65 - https://www.cc65.org/
 * b2d - https://github.com/markadev/b2d
 * AppleCommander - https://applecommander.github.io/


### Prepare build environment

```shell
sudo apt update
sudo apt install -y build-essential

# Install cc65 compiler
sudo apt install -y cc65

# Install a bugfixed version of b2d (Apple II image converter)
git clone git@github.com:markadev/b2d.git
make -C b2d/src && sudo cp b2d/b2d /usr/local/bin/

# Download AppleCommander and install a JRE for it
sudo apt install -y openjdk-19-jre
wget https://github.com/AppleCommander/AppleCommander/releases/download/1.8.0/AppleCommander-ac-1.8.0.jar
```


### Build the image

```shell
make
```
