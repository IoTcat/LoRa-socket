



#include <LoRa.h>

#define LORA_SOCKET_IP "2.2.2.1"

#include "lora-socket.h"


LoRaSocket socket;

void setup(){

    Serial.begin(115200);
   if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

    socket.ini();
    //socket.udp("lalalla");
    socket.tcp("tomato is rabbish", "2.2.2.2");
}


void loop(){

    socket.core();
}