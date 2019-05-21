#include<Send_Recv.h>

int Send_Recv(SOCKET clientSocket, char* Rev_data, unsigned char* Send_data)
{
	int iRet;
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
}