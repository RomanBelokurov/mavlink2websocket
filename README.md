
# mavlink2websocket
Service for streaming MavLink telemetry via websocket based backend service. 
Two way communication with callbacks. Feel free to fork and modify as you wish.

mavlink2websocket is a tiny Raspberry Pi service writen in C++. 

Please make sure that all necessary libraries are installed before running.

Update: no boost libraries needed anymore as in previous version.

## Dependencies:

Install GLib-2.0 libraries:

    sudo apt-get install glib-2.0

Install libsoup:

    sudo apt-get install libsoup2.4-dev

## Installation:
Clone source from this repository:    

    git clone https://github.com/RomanBelokurov/mavlink2websocket

Then clone submodules

    git submodule update --init --recursive

## Build and run service

    mkdir build && cd build
    cmake ../
    make    
    ./uavClient -uri ws://127.0.0.1:3000 -serial /dev/serial0 -baud 57600
