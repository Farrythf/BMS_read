#pragma once

#ifndef Send_Recv_h
#define Send_Recv_h

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string>
#include <BMS_read.h>

int Send_Recv(SOCKET clientSocket, char* Rev_data, unsigned char* Send_data);



#endif