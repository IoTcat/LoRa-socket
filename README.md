# LoRa-socket

![size](https://badge-size.herokuapp.com/iotcat/LoRa-socket/master/lora-socket.h)

[简体中文](./zh.md)

## Usage - with Callback Functions
```C++

#define LORA_SOCKET_IP "1.0.0.1" //need to be unique

#include "lora-socket.h"

LoRaSocket socket;

void setup(){
    socket.ini(); //initalization
    socket.onReceived(doIfRec); //Register a callback function, which is triggered when a new message is received
}

void doIfRec(String message, String fromIP, String toIP, String msgType){
    //Execute the this function when a new message is received
    
    socket.udp("Hi"); //Broadcast using udp
    socket.udp("Hi xx", "1.0.0.2"); //Use udp to send messages to 1.0.0.2
    socket.tcp("Hi xxx", "1.0.0.2");  //Use tcp to send messages reliably to 1.0.0.2
}

void loop(){
    //No delay() could be used in loop()
    socket.core(); //Socket service core
}


```
## 使用案例 - 使用条件触发
```C++

#define LORA_SOCKET_IP "1.0.0.1" //need to be unique

#include "lora-socket.h"

LoRaSocket socket;

void setup(){
    socket.ini(); //initalization
}

void loop(){
    //No delay() could be used in loop()
    if(socket.isNewMsg()){ //Determine whether there is new message
        Serial.println(socket.getNewMsg()); //Print out new messages directly [Method 1]
        
        String message, fromIP, toIP, msgType;
        socket.getNewMsg(message, fromIP, toIP, msgType); //Get content and information by reference [Method 2]
    }
    
    socket.core(); //Socket service core
}


```
