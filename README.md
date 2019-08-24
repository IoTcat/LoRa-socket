# LoRa-socket

## 使用案例
```C++

#define LORA_SOCKET_IP "1.0.0.1" //需唯一

#include "lora-socket.h"

LoRaSocket socket;

void setup(){
    socket.ini();
}

void doIfRec(String message, String fromIP, String toIP, String msgType){
    //当收到新消息时执行这个函数中的内容
    
    socket.udp("你好"); //使用udp广播
    socket.udp("xx你好", "1.0.0.2"); //使用udp向1.0.0.2发送消息
    socket.tcp("xxx你好", "1.0.0.2");  //使用tcp向1.0.0.2可靠地发送消息
}

void loop(){
    socket.core();
}


```
