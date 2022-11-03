#include "ringBuffer.h"


void RB_setup(RING_BUFFER* buffer, size_t size, size_t typeSize){
	buffer->size = size;
    buffer->typeSize = typeSize;
	buffer->start = (uint8_t*) malloc(buffer->size * typeSize);
	buffer->write = buffer->start;
	buffer->read = buffer->start;
    buffer->status = RB_EMPTY;
}


uint8_t RB_write(RING_BUFFER* buffer, void* data){
    //check pointer overflow
	if(buffer->write == buffer->start + buffer->size * buffer->typeSize){

        //check if buffer is full
		if(buffer->read != buffer->start){
			buffer->write = buffer->start;
		}else{
			//buffer full
            buffer->status = RB_FULL;
			return RB_FULL;
		}
	}else{

        //check if buffer is full
		if((buffer->write + buffer->typeSize) != buffer->read){
			buffer->write += buffer->typeSize;
		}else{
			//buffer full
            buffer->status = RB_FULL;
			return RB_FULL;
		}
	}

    memcpy(buffer->write, data, buffer->typeSize);
    buffer->status = RB_READY;
	return 0;
}

uint8_t RB_read(RING_BUFFER* buffer, void* data){
    //check pointer overflow
	if(buffer->read == buffer->start + buffer->size * buffer->typeSize){

        //check if buffer is empty
		if(buffer->read != buffer->write){
			buffer->read = buffer->start;
		}else{
			//buffer empty
			buffer->status = RB_EMPTY;
			return RB_EMPTY;
		}
	}else{

        //check if buffer is empty
		if(buffer->read != buffer->write){
			buffer->read += buffer->typeSize;
		}else{
			//buffer empty
			buffer->status = RB_EMPTY;
			return RB_EMPTY;
		}
	}

    memcpy(data, buffer->read, buffer->typeSize);
	buffer->status = RB_READY;
	return 0;
}
