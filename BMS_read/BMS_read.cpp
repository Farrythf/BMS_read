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
					"Battery Power Request: ", "Set Operation: ", "Set Inverter State: " };

#pragma endregion

#pragma region Socketlib_init

	WSADATA wsaData;
	int iRet = 0;
	iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iRet != 0)
	{
		cout << "WSAStartup(MAKEWORD(2, 2), &wsaData) execute failed!";
		return -1;
	}
	if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))
	{
		WSACleanup();
		cout << "WSADATA version is not correct!";
		return -1;
	}

#pragma endregion

#pragma region Socket_init

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
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

	welcome_info();

#pragma endregion

#pragma region Server_setting

	char ip_addr[20];
	int port = PortNum;
	cout << "Server IP: ";
	cin >> ip_addr;
	cout << "Connecting...";
	SOCKADDR_IN srv_Addr;
	srv_Addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);
	srv_Addr.sin_family = AF_INET;
	srv_Addr.sin_port = htons(port);

#pragma endregion

#pragma region Socket_connent

	while (true)
	{
		iRet = connect(clientSocket, (SOCKADDR*)& srv_Addr, sizeof(SOCKADDR));
		if (0 != iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAEINVAL)
			{
				Sleep(500);
				continue;
			}
			else if (err == WSAEISCONN)
			{
				break;
			}
			else
			{
				cout << "connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR)) execute failed!";
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}

		}
	}
	cout << "OK\n";
	cout << "Connected!\n";
	for (int i = 0; i < 100; i++)
	{
		cout << ">";
	}
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	cout << "\n\n" << st.wMonth << "/" << st.wDay << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "\n";
	cout << "Server IP: " << ip_addr << " Port: " << port << "\n";

#pragma endregion

#pragma region Send_Recv

	ofstream file;
	file.open("res.out", ios::out | ios::trunc);
	if (file.fail())
	{
		cout << "File open fail";
		return -1;
	}

	char Rev_data[Maxdatasize];
	unsigned char Send_data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x05 , 0x01, 0x03, 0x27, 0x55, 0x00, 0x40 };	//Read all the regs from 802, 64 regs, 128 byte.
	while (true)
	{
		iRet = send(clientSocket, (const char*)Send_data, sizeof(Send_data), 0);
		if (SOCKET_ERROR == iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(500);
				continue;
			}
			else
			{
				printf("send failed!\n");
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
		ZeroMemory(Rev_data, Maxdatasize);
		iRet = recv(clientSocket, Rev_data, sizeof(Rev_data), 0);
		if (SOCKET_ERROR == iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{
				printf("recv failed!\n");
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

	int datawr[Maxdatasize];
	memset(datawr, 0, sizeof(datawr));

	char inter;
	int indx = 0;
	for (int i = 9; i < 37;)
	{
		inter = Rev_data[i];
		Rev_data[i] = Rev_data[i + 1];
		Rev_data[i + 1] = inter;
		memcpy(datawr + indx, Rev_data + i, 2);
		indx++;
		i = i + 2;
	}

	inter = Rev_data[37];
	Rev_data[37] = Rev_data[40];
	Rev_data[40] = inter;
	inter = Rev_data[38];
	Rev_data[38] = Rev_data[39];
	Rev_data[39] = inter;
	memcpy(datawr + indx, Rev_data + 37, 4);
	indx++;

	for (int i = 41; i < 57;)
	{
		inter = Rev_data[i];
		Rev_data[i] = Rev_data[i + 1];
		Rev_data[i + 1] = inter;
		memcpy(datawr + indx, Rev_data + i, 2);
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
		memcpy(datawr + indx, Rev_data + i, 4);
		indx++;
		i = i + 4;
	}

	for (int i = 77; i < 137;)
	{
		inter = Rev_data[i];
		Rev_data[i] = Rev_data[i + 1];
		Rev_data[i + 1] = inter;
		memcpy(datawr + indx, Rev_data + i, 2);
		indx++;
		i = i + 2;
	}

#pragma endregion

#pragma region File_output

	for (int i = 0; i < 46; i++)
	{
		file << str[i] << datawr[i] << "\n";
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
	std::cout << "Version: 1.0.1\n";
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
