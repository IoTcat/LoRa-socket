



#include <LoRa.h>

#define LORA_SOCKET_IP "2.2.2.1"

#include "lora-socket.h"


LoRaSocket socket;

void setup(){

    socket.ini();
    socket.tcp("lalalla", "10.2.3.1");
    socket.udp("ll");
}


void loop(){

    socket.core();
}