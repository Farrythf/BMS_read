// BMS_read.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define Maxdatasize 1000
#define PortNum 502

void welcome_info();
int main()
{
#pragma region String_init

	string str[] = { "Battery Base Model: ", "Qulify Code: ", "Nameplate Charge Capacity(Ah): ", "Nameplate Energy Capacity(Wh): ", "Nameplate Max Charge Rate: ", "Namplate Max Discharge Rate: ",
					"Self Discharge Rate: ", "Nameplate Max SoC: ", "Nameplate Min SoC: ", "Max Reserve Percent: ", "Min Reserve Percent: ", "State of Charge: ", "Depth of Discharge: ",
					"State of Health: ", "Cycle Count: ", "Charge Status: ", "Control Mode: ", "Battery Heartbeat: ", "Controller Heartbeat: ", "Alarm Reset: ", "Battery Type: ", "State of the Battery Bank: ",
					"Pad: ", "Warranty Date: ", "Battery Event 1 Bitfield: ", "Battery Event 2 Bitfield: ", "Vendor Event Bitfield 1: ", "Vendor Event Bitfield 2: ", "External Battery Voltage: ",
					"Max Battery Voltage: ", "Min Battery Voltage: ", "Max Cell Voltage: ", "Max Cell Voltage String: ", "Max Cell Voltage Module: ", "Min Cell Voltage: ", "Min Cell Voltage String: ",
					"Min Cell Voltage Module: ", "Average Cell Voltage: ", "Total DC Current: ", "Max Charge Current: ", "Max Discharge Current: ", "Total Power: ", "Inverter State Request: ",
					"Battery Power Request: ", "Set Operation: ", "Set Inverter State: ", "Scale factor for charge capacity: ", "Scale factor for energy capacity: ", "Scale factor for maximum charge and discharge rate: ",
					"Scale factor for self discharge rate: ", "Scale factor for state of charge values: ", "Scale factor for depth of discharge: ", "Scale factor for state of health: ", "Scale factor for DC bus voltage: ",
					"Scale factor for cell voltage: ", "Scale factor for DC current: ", "Scale factor for instantationous DC charge/discharge current: ", "Scale factor for AC power request: "};
	
	/*String for every variety read from the device, used for file written*/

#pragma endregion

#pragma region Socketlib_init

	WSADATA wsaData;														//init the WSADATA for socket lib
	int iRet = 0;															//a global identifier for error detect
	iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);							//socket lib init
	if (iRet != 0)															//error catch
	{
		cout << "WSAStartup(MAKEWORD(2, 2), &wsaData) execute failed!";		//fatal error, end the programe
		return -1;
	}
	if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))		//WSADATA version check
	{
		WSACleanup();														//version not right, please check the lib install
		cout << "WSADATA version is not correct!";
		return -1;
	}

#pragma endregion

#pragma region Socket_init

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);							//socket init
	if (clientSocket == INVALID_SOCKET)												//fatal error, end the progame
	{
		cout << "clientSocket = socket(AF_INET, SOCK_STREAM, 0) execute failed!";
		return -1;
	}

	/*int iMode = 1;
	iRet = ioctlsocket(clientSocket, FIONBIO, (u_long FAR*) & iMode);
	if (iRet == SOCKET_ERROR)
	{
		cout << "ioctlsocket failed!\n";
		WSACleanup();
		return -1;
	}*/

#pragma endregion

#pragma region Welcome_info

	welcome_info();				//version information

#pragma endregion

#pragma region Server_setting

	char ip_addr[20];										//for ip address
	int port = PortNum;										//for prot number
	cout << "Server IP: ";
	cin >> ip_addr;
	cout << "Connecting...";
	SOCKADDR_IN srv_Addr;									//sturcture for ip address
	srv_Addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);		//use old version function "inet_addr",change it if you want
	srv_Addr.sin_family = AF_INET;							//stipulate the family format
	srv_Addr.sin_port = htons(port);						//define port number

#pragma endregion

#pragma region Socket_connent

	while (true)
	{
		iRet = connect(clientSocket, (SOCKADDR*)& srv_Addr, sizeof(SOCKADDR));								//try to connect
		if (0 != iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAEINVAL)													//connection bolcked, wait and try again
			{
				Sleep(500);
				continue;
			}
			else if (err == WSAEISCONN)
			{
				break;																						//connect successful
			}
			else
			{
				cout << "connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR)) execute failed!";		//fatal error, end programe
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}

		}
	}
	cout << "OK\n";																							//connection information and time
	cout << "Connected!\n";
	for (int i = 0; i < 100; i++)
	{
		cout << ">";
	}
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	cout << "\n***************************************************************";
	cout << "\n******************Time and Information*************************\n*******************";
	cout << st.wMonth << "/" << st.wDay << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond;
	cout << "**************************\n**********";
	cout << "Server IP: " << "192.168.11.106" << " Port: " << "501" << "******************\n";
	cout << "***************************************************************\n";

#pragma endregion

#pragma region Send_Recv

	ofstream file;																								//file init
	file.open("res.out", ios::out | ios::trunc);																//every time recreate the file
	if (file.fail())
	{
		cout << "File open fail";																				//fatal error end the programe
		return -1;
	}

	char Rev_data[Maxdatasize];
	unsigned char Send_data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x05 , 0x01, 0x03, 0x27, 0x55, 0x00, 0x40 };	//Read all the regs from 802, 64 regs, 128 byte.
	while (true)
	{
		iRet = send(clientSocket, (const char*)Send_data, sizeof(Send_data), 0);								//try send the data
		if (SOCKET_ERROR == iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(500);																						//send blocked, wait and try again
				continue;
			}
			else
			{
				printf("send failed!\n");																		//fatal error end programe
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}
		}
		break;
	}
	//cout << "Sent\n";

	while (true)
	{
		ZeroMemory(Rev_data, Maxdatasize);																		//receive data buffer
		iRet = recv(clientSocket, Rev_data, sizeof(Rev_data), 0);												//non-blocking receive
		if (SOCKET_ERROR == iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);																						//receive blocked, wait and try again
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{
				printf("recv failed!\n");																		//fatal error, end programe
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}
			break;
		}
		break;
	}
#pragma endregion

#pragma region Data_format

	int Data_802[Maxdatasize];
	memset(Data_802, 0, sizeof(Data_802));

	char inter;
	int indx = 0;
	for (int i = 9; i < 37;)								//format the data read from the device
	{														//the read-back data is char array(normal order)
		inter = Rev_data[i];								//to convert the char array to int
		Rev_data[i] = Rev_data[i + 1];						//memcpy is used to realize the process
		Rev_data[i + 1] = inter;							//in Windows data storage is in abnormal order
		memcpy(Data_802 + indx, Rev_data + i, 2);			//the least significant bit is in the leftest
		indx++;												//thus we should adjust the order manually	
		i = i + 2;											//4 for is used for this
	}

	inter = Rev_data[37];
	Rev_data[37] = Rev_data[40];
	Rev_data[40] = inter;
	inter = Rev_data[38];
	Rev_data[38] = Rev_data[39];
	Rev_data[39] = inter;
	memcpy(Data_802 + indx, Rev_data + 37, 4);
	indx++;

	for (int i = 41; i < 57;)
	{
		inter = Rev_data[i];
		Rev_data[i] = Rev_data[i + 1];
		Rev_data[i + 1] = inter;
		memcpy(Data_802 + indx, Rev_data + i, 2);
		indx++;
		i = i + 2;
	}

	for (int i = 57; i < 77;)
	{
		inter = Rev_data[i];
		Rev_data[i] = Rev_data[i + 3];
		Rev_data[i + 3] = inter;
		inter = Rev_data[i + 1];
		Rev_data[i + 1] = Rev_data[i + 2];
		Rev_data[i + 2] = inter;
		memcpy(Data_802 + indx, Rev_data + i, 4);
		indx++;
		i = i + 4;
	}

	for (int i = 77; i < 137;)
	{
		inter = Rev_data[i];
		Rev_data[i] = Rev_data[i + 1];
		Rev_data[i + 1] = inter;
		memcpy(Data_802 + indx, Rev_data + i, 2);
		indx++;
		i = i + 2;
	}

#pragma endregion

#pragma region File_output

	for (int i = 0; i < 58; i++)
	{
		file << str[i] << Data_802[i] << "\n";		//normal file export 
	}
	file << "\n";
	file.close();

#pragma endregion

	closesocket(clientSocket);
	WSACleanup();
	system("pause");
	return 0;
}

void welcome_info()
{
	std::cout << "Engine statr\n";
	std::cout << "Version: 1.0.3\n";
	for (int i = 0; i < 100; i++)
	{
		cout << ">";
	}
	cout << "\n";
}




// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
