# LoRa-socket

## 使用案例 - 使用回调函数
```C++

#define LORA_SOCKET_IP "1.0.0.1" //需唯一

#include "lora-socket.h"

LoRaSocket socket;

void setup(){
    socket.ini(); //初始化
    socket.onReceived(doIfRec); //注册回调函数，当接收到新消息时触发
}

void doIfRec(String message, String fromIP, String toIP, String msgType){
    //当收到新消息时执行这个函数中的内容
    
    socket.udp("你好"); //使用udp广播
    socket.udp("xx你好", "1.0.0.2"); //使用udp向1.0.0.2发送消息
    socket.tcp("xxx你好", "1.0.0.2");  //使用tcp向1.0.0.2可靠地发送消息
}

void loop(){
    //注意loop()全局不得有delay，否则会导致接收不稳定
    socket.core(); //循环组件
}


```
## 使用案例 - 使用条件触发
```C++

#define LORA_SOCKET_IP "1.0.0.1" //需唯一

#include "lora-socket.h"

LoRaSocket socket;

void setup(){
    socket.ini(); //初始化
}

void loop(){
    //注意loop()全局不得有delay，否则会导致接收不稳定
    if(socket.isNewMsg()){ //判断是否有新消息
        Serial.println(socket.getNewMsg()); //直接打印出新消息(法一)
        
        String message, fromIP, toIP, msgType;
        socket.getNewMsg(message, fromIP, toIP, msgType); //通过引用获取消息内容，发信ip，收信ip，消息类型
    }
    
    socket.core(); //循环组件
}


```
