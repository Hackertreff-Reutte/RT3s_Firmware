#include "serial.h"


void print(const char* s){
    print((char *) s);
}

void print(char* s){

    taskENTER_CRITICAL();
    for(unsigned int i = 0; i < strlen(s) && tx_buffer.status != RB_FULL; i++){
        RB_write(&tx_buffer, &s[i]);
    }
    taskEXIT_CRITICAL();
}

void println(const char* s){
    println((char *) s);
}

void println(char* s){
    taskENTER_CRITICAL();
    for(unsigned int i = 0; i < strlen(s) && tx_buffer.status != RB_FULL; i++){
        RB_write(&tx_buffer, &s[i]);
    }
    char carriageReturn = '\r';
    RB_write(&tx_buffer, &carriageReturn);
    char newline = '\n';
    RB_write(&tx_buffer, &newline);
    taskEXIT_CRITICAL();
}

std::string readAll(){
    std::string s = "";
    char data;
    taskENTER_CRITICAL();
    while(rx_buffer.status != RB_EMPTY){
        if(RB_read(&rx_buffer, &data) != RB_EMPTY){
            s += data;
        }
    }
    taskEXIT_CRITICAL();
    return s;
}

std::string readln(){
    std::string s = "";
    char data;
    taskENTER_CRITICAL();
    while(rx_buffer.status != RB_EMPTY){
        if(RB_read(&rx_buffer, &data) != RB_EMPTY){
            s += data;
            if(data == '\n'){
                break;
            }
        }
    }
    taskEXIT_CRITICAL();
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