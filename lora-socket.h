#ifndef __LORA_SOCKET_H__
#define __LORA_SOCKET_H__

#include <LoRa.h>
#include "vector.h"

#ifndef LORA_SOCKET_IP
#define LORA_SOCKET_IP "0.0.0.0"
#endif

#ifndef MAX_RECEIVE_STACK_SIZE
#define MAX_RECEIVE_STACK_SIZE 5
#endif

#ifndef MAX_SEND_STACK_SIZE
#define MAX_SEND_STACK_SIZE 5
#endif

#ifndef MAX_TCP_TRY_TIMES
#define MAX_TCP_TRY_TIMES 8
#endif


class LoRaSocket {
   public:
    LoRaSocket(){}
    static void core();
    static void ini();
    static void udp(const String& msg, const String& to = "0.0.0.0");
    static void tcp(const String& msg, const String& to);
    static void rtcp(const String& msg);

    static void onReceived(const String& msg, const String& from, const String& to, const String& type){

    };


   private:
    static Vector<String> tcp_sendingStack, tcp_receiveStack;
    static Vector<unsigned int> tcp_sendingTryTimes;
    /* LoRa Functions */
    static void LoRa_tx_mode();
    static void LoRa_rx_mode();
    static void send(const String& s);
    static const String& receiveMsg();
    /* Package Functions */
    inline static const String& getIPHeader(const String& to = "0.0.0.0"){
        return to + "|" + LORA_SOCKET_IP + "|";
    };
    inline static bool isGoodPackage(const String& s){
        String body = s.substring(0, s.length() - 1);
        if(s.substring(s.length() - 1, s.length()) != hash(body)){
            return false;
        }
        return true;
    };
    inline static const String& getToIP(const String& s){
        unsigned short left = s.indexOf('|');
        unsigned short right = s.indexOf('|', left + 1);
        return s.substring(left, right - 1);
    };
    inline static const String& getFromIP(const String& s){
        unsigned short left = s.indexOf('|', s.indexOf('|') + 1);
        unsigned short right = s.indexOf('|', left + 1);
        return s.substring(left, right - 1);
    };
    inline static const String& getType(const String& s){
        return s.substring(0, s.indexOf('|') - 1);
    };
    inline static const String& getContent(const String& s){
        unsigned short left = s.indexOf('|', s.indexOf('|', s.indexOf('|') + 1) + 1);
        unsigned short right = s.indexOf('|', left + 1);
        return decode(s.substring(left, right - 1));
    };
    inline static const String& getTcpKey(const String& s){
        unsigned short left = s.indexOf('|', s.indexOf('|', s.indexOf('|', s.indexOf('|') + 1) + 1) + 1);
        unsigned short right = s.indexOf('|', left + 1);
        return decode(s.substring(left, right - 1));
    };
    /* receive Functions */
    static void getMsg(const String& msg);
    /* tcp stack functions */
    static void checkSendStack(){
        for(unsigned int i = 0; i < tcp_sendingStack.Size(); i ++){
            send(tcp_sendingStack[i]);
            tcp_sendingTryTimes[i] += 1;

            if(tcp_sendingTryTimes[i] >= MAX_TCP_TRY_TIMES){
                tcp_sendingStack.Erase(i);
                tcp_sendingTryTimes.Erase(i);
            }
        }
    };
    inline static void receiveStackClassify(){
        if(tcp_receiveStack.Size() > MAX_RECEIVE_STACK_SIZE) tcp_receiveStack.Erase(0);
    };
    inline static void sendStackClassify(){
        if(tcp_sendingStack.Size() > MAX_SEND_STACK_SIZE) {
            tcp_sendingStack.Erase(0);
            tcp_sendingTryTimes.Erase(0);
        }
    };
    static void removeByKey(const String& key);
    /* tools */
    static const String& hash(const String& s);
    static const String& encode(const String& s){
        return s;
    };
    static const String& decode(const String& s){
        return s;
    }
    static const String& generateRandomKey(){
        String o = "";
        for(unsigned short i = 0; i < 4; i ++){
            o += char(LoRa.random() % 26 + 97);
        }
        return o;
    }

    /* timer */
    static void setInterval(void (*function)(void), const int delay){
        static unsigned long startTime = millis(); 

        if(millis() - startTime > delay){
            (*function)();
            startTime = millis();
        }
    }


};

void LoRaSocket::getMsg(const String& msg){
    if(!isGoodPackage(msg)) return;
    if(getToIP(msg) != LORA_SOCKET_IP && getToIP(msg) != "0.0.0.0") return;

    if(getType(msg) == "udp") onReceived(getContent(msg), getFromIP(msg), getToIP(msg), "udp");
    if(getType(msg) == "tcp"){
        if(tcp_receiveStack.Find(msg) != -1) return;
        onReceived(getContent(msg), getFromIP(msg), getToIP(msg), "tcp");
        rtcp(msg);
        receiveStackClassify();
    }
    if(getType(msg) == "rtcp"){
        removeByKey(getContent(msg));
    }

}

void LoRaSocket::udp(const String& msg, const String& to){
    String fin = "udp|" + getIPHeader(to) + encode(msg) + "|";
    fin += hash(fin);
    send(fin);
};


void LoRaSocket::tcp(const String& msg, const String& to){
    String fin = "tcp|" + getIPHeader(to) + encode(msg) + "|" + generateRandomKey() + "|";
    fin += hash(fin);
    tcp_sendingStack.PushBack(fin);
    tcp_sendingTryTimes.PushBack(0);
    sendStackClassify();
    send(fin);
};


void LoRaSocket::rtcp(const String& msg){
    tcp_receiveStack.PushBack(msg);
    String fin = "rtcp|" + getIPHeader(getFromIP(msg)) + getTcpKey(msg) + "|";
    fin += hash(fin);
    send(fin);
}

Vector<String> LoRaSocket::tcp_sendingStack, LoRaSocket::tcp_receiveStack;
Vector<unsigned int> LoRaSocket::tcp_sendingTryTimes;

void LoRaSocket::ini() {


}

void LoRaSocket::core() {

    /* Listen Msg */
    if(LoRa.parsePacket()){
        getMsg(receiveMsg());
    }

    /* check tcp stack */
    setInterval(checkSendStack, 1000);

}

void LoRaSocket::LoRa_tx_mode(){
    LoRa.idle();
}

void LoRaSocket::LoRa_rx_mode(){
    LoRa.receive();
}

void LoRaSocket::send(const String& s){
  LoRa_tx_mode();
  delay(200);
  LoRa.beginPacket();
  LoRa.print(s);
  LoRa.endPacket();
  delay(200);
  LoRa_rx_mode();
}


const String& LoRaSocket::receiveMsg(){
    String s = "";
    while (LoRa.available()) {
        s += (char)LoRa.read();
    }
    return s;
}

const String& LoRaSocket::hash(const String& s){
    unsigned char hashVal = 'k';
    for(unsigned short i = 0; i < s.length(); i ++){
        hashVal ^= s.charAt(i);
    }
    hashVal = hashVal % 26 + 97;
    return String(hashVal);
}


void LoRaSocket::removeByKey(const String& key){
    for(unsigned int i = 0; i < tcp_sendingStack.Size(); i++){
        if(getTcpKey(tcp_sendingStack[i]) == key) {
            tcp_sendingStack.Erase(i);
            return;
        }
    }
}

#endif //__LORA_SOCKET_H__