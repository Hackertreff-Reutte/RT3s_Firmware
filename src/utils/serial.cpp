#include "serial.h"


void print(char* s){

    for(unsigned int i = 0; i < strlen(s) && tx_buffer.status != RB_FULL; i++){
        RB_write(&tx_buffer, &s[i]);
    }

    sendTxBuffer();
}

void println(char* s){
    for(unsigned int i = 0; i < strlen(s) && tx_buffer.status != RB_FULL; i++){
        RB_write(&tx_buffer, &s[i]);
    }
    char newline = '\n';
    RB_write(&tx_buffer, &newline);

    sendTxBuffer();
}

std::string readAll(){
    std::string s = "";
    char data;
    while(rx_buffer.status != RB_EMPTY){
        if(RB_read(&rx_buffer, &data) != RB_EMPTY){
            s += data;
        }
    }
    return s;
}

std::string readln(){
    std::string s = "";
    char data;
    while(rx_buffer.status != RB_EMPTY){
        if(RB_read(&rx_buffer, &data) != RB_EMPTY){
            s += data;
            if(data == '\n'){
                break;
            }
        }
    }
    return s;
}

char getChar(){
    char data;
    if(RB_read(&rx_buffer, &data) != RB_EMPTY){
        return data;
    }else{
        return -1;
    }
}