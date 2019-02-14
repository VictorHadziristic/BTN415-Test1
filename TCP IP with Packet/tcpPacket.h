#pragma once
#include <memory>
#include <iostream>
#include <fstream>

class Packet
{
public:
	struct Header
	{
		//char srcAddr : 4;
		//char destAddr : 4;
		char numOfBytes;
		
	} Head;

	char * Data;

	Packet() {
		Data = nullptr;
		//Head.srcAddr = 1;
		//Head.destAddr = 0;
		Head.numOfBytes = 0;
	}
};
