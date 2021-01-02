
# mavlink2websocket
Service for streaming MavLink telemetry via websocket based backend service. 
Two way communication with callbacks. Feel free to fork and modify as you wish.

mavlink2websocket is a tiny RPi service writen in C++. 

Please make sure that all necessary libraries are installed before running.

## Dependencies:
Install Boost libraries:

    sudo apt-get install libboost-all-dev

Install Websocket++ from sources:

    git clone https://github.com/zaphoyd/websocketpp
    cd websocketpp
    mkdir build && cd build
    cmake ../
    cmake install

## Installation:
Clone source from this repository:    

    git clone https://github.com/RomanBelokurov/mavlink2websocket

Then clone submodules

    git submodule update --init --recursive

## Build and run service

    mkdir build && cd build
    cmake ../
    make
    ./uavService "ws://[YOUR_SERVER]:[PORT]


## Used libraries:

websocketpp: https://github.com/zaphoyd/websocketpp
