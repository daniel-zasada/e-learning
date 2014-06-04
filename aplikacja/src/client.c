// socknow.cpp : Defines the entry point for the console application.
//

#include <cstdlib>
#include <stdlib.h>
#include "stdafx.h"
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#define IDC_MAIN_BUTTON_CONNECTED 100 //ok button for connected
#define IDC_MAIN_BUTTON 101//ok button
#define IDC_MAIN_BUTTON_CANCEL 102//cancel button
#define IDC_EDIT_LOG 103//edit box for login
#define IDC_EDIT_PASS 104//edit box for password
#define IDC_EDIT_IN 105//edit box for typing messages
#define IDC_EDIT_OUT 106
#define WM_SOCKET 107
#define WM_SOCKET2 108

HWND logButt;
HWND passButt;
HWND hButtonConnected;
HGDIOBJ defFontOK_CONN;
int len, lenPass, len2;
int error;
int clientData;
int mode;
int nextRead = 0;
char textIDC;
char *buffs;
char *buffs2;
char *buffPass;
char clientBuff[128];
char ready[3] = "NO";
char Indx[3];
char specialBuff[] = "??";

LPSTR lpstr;
LPSTR passpstr;
LPSTR lpstr2;

HWND hwind, hwind2;
SOCKET Socket;

HWND hEditIn, hEditOut, hEditIn2;
HGDIOBJ defFont;

int PHASE = 0;
int valTest = 0;

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void mSock();
int wind();
int wind2();
void addEndText(HWND, LPCWSTR);

int _tmain(int argc, _TCHAR* argv[])
{
	/*HINSTANCE hInst;
	hInst = 0;
	//WNDCLASSEX temp;

	//window registration
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
	wndClass.cbClsExtra = NULL;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = NULL;
	wndClass.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = NULL;
	wndClass.hIconSm = NULL;
	wndClass.hInstance = hInst;
	wndClass.lpfnWndProc = (WNDPROC)WinProc;
	wndClass.lpszClassName = L"WndClass";
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	//registration wndClass
	if(!RegisterClassEx(&wndClass))
	{
		int res = GetLastError();
		MessageBox(NULL,
			L"Window class creation failed",
			L"Window Class Failed",
			MB_ICONERROR);
		std::cout << "ERROR CODE: " << res << std::endl;

		getchar();
		return 1;
	}
	//create window basics on wndClass
	hwind = CreateWindowEx(NULL,
					L"WndClass",
					L"Win async client...",
					WS_SIZEBOX,// | WS_SYSMENU,//WS_OVERLAPPEDWINDOW,
					400,//x
					200,//y
					300,//width
					150,//height
					NULL,
					NULL,
					hInst,
					NULL);


	if(!hwind)
	{
		int err_result = GetLastError();

		MessageBox(NULL,
				L"Window create failed...",
				L"Window fail...",
				MB_ICONERROR);

		std::cout << "ERROR: " << err_result;
	}

	ShowWindow(hwind, 9);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/
	wind();
	getchar();
	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			/*case IDC_MAIN_BUTTON_CONNECTED:
				std::cout << "\n\tCONNECTED OK WAS PRESSED " << std::endl;
				char log2Buff[3];
				ZeroMemory(log2Buff, sizeof(log2Buff));
				//char specialBuff[] = "??";
				sprintf(log2Buff,"%d", len); //log len to log2Buff
				send(Socket, log2Buff, sizeof(log2Buff), 0);
				send(Socket, buffs, len, 0);
				//special character
				send(Socket, specialBuff, sizeof(specialBuff), 0);
				send(Socket, "HELLO SERVER!", 13, 0);
			break;*/

			case IDC_MAIN_BUTTON: //ok pressed
				std::cout << "\t\tOK WAS PRESSED\n" << std::endl;
				/*char buffer[256];
				SendMessage(logButt,
								WM_GETTEXT,
								sizeof(buffer)/sizeof(buffer[0]),
								reinterpret_cast<LPARAM>(buffer));*/

				len = SendMessage(logButt, WM_GETTEXTLENGTH, 0, 0);
				lenPass = SendMessage(passButt, WM_GETTEXTLENGTH, 0, 0);

				if(len > 21)
				{
					std::cout << "\nLOG TOO LONG ";
					return 0;
				}

				std::cout << "LengthLogg: " << len << std::endl;
				std::cout << "LengthPass: " << lenPass << std::endl;
				//LPSTR text[len];
				lpstr = (LPSTR)malloc(len+1);
				passpstr = (LPSTR)malloc(lenPass+1);

				buffPass = new char[lenPass];
				buffs = new char[len];

				SendMessage(logButt, WM_GETTEXT, (WPARAM)len+1,
								(LPARAM)lpstr);
				SendMessage(passButt, WM_GETTEXT, (WPARAM)lenPass+1,
								(LPARAM)passpstr);
				//char strs[128];
				//GetDlgItemText(hWnd, IDC_EDIT_LOG, (LPWSTR)strs, len);
				int i;
				int j;
				j=0;

				ZeroMemory(buffs, sizeof(buffs));
				for(i = 0; i < len; i++)
				{
					buffs[i] = lpstr[j];
					j = j+2;
				}
				i=0;
				//log
				std::cout << "\nLOGIN: ";
				while(i < len)
				{
					std::cout << buffs[i];
					i++;
				}

				//for pass
				ZeroMemory(buffPass, sizeof(buffPass));
				j=0;
				for(i = 0; i < lenPass; i++)
				{
					buffPass[i] = passpstr[j];
					j = j+2;
				}
				i=0;
				std::cout << "\nPASS: ";
				while(i < lenPass)
				{
					std::cout << buffPass[i];
					i++;
				}

				mSock();

			//DestroyWindow(hWnd);
			break;

		case IDC_MAIN_BUTTON_CANCEL://cancel pressed
				//static int occur = 0;
				std::cout << "\t\t\nCANCEL WAS PRESSED" << std::endl;

					//first send index of client to server
					//send(Socket, Indx, sizeof(Indx), 0);
					//send(Socket, Indx, sizeof(Indx),0);
					shutdown(Socket, SD_SEND);
					closesocket(Socket);
					DestroyWindow(hWnd);
				break;
		}

		break;
	case WM_SOCKET:
				switch(WSAGETSELECTEVENT(lParam))
				{
				case FD_READ:
					char cbuff[2];
					//at first read index of client
					if(nextRead == 0)
					{
						ZeroMemory(Indx, sizeof(Indx));
						//ZeroMemory(cbuff, 2);
						//ZeroMemory(ready, 2);
						MessageBox(hwind, L"CLIENT RECEIVED FD_READ MSG", L"CLIENT", MB_ICONINFORMATION);

						int indx_error = recv(wParam, Indx, sizeof(Indx), 0);
						if(indx_error == SOCKET_ERROR)
						{
							indx_error = WSAGetLastError();
							std::cout << "\n\tINDX ERROR: " << indx_error << std::endl;
							break;
						}
						//Sleep(500);
						std::cout << "\nRECEIVED INDX-CLIENT " << Indx << std::endl;
						nextRead = 1;
					}
					//if you got index receive info about pass and login
					else if(nextRead == 1)
					{	//Sleep(500);
						MessageBox(hwind, L"CLIENT RECEIVED PASS INFO", L"CLIENT", MB_ICONINFORMATION);
						recv(Socket, cbuff, 2, 0);

						error = WSAGetLastError();
						if(error != WSAEWOULDBLOCK && error != 0)
						{
							std::cout << "ERROR code: " << error;
							//shutdown sock
							shutdown(Socket, SD_SEND);
							closesocket(Socket);
							break;
						}

						if(cbuff[0] == 'N' && cbuff[1] == 'O')
						{
							std::cout << "\nPASSWORD NOT CORRECT" << std::endl;
							std::cout << "\nINDEX: " << Indx;
							int indx = atoi(Indx);
							std::cout << "\nAFTER CONVERTING: " << indx << std::endl;
							send(Socket, Indx, 2, 0);
							//shutdown(Socket, SD_SEND);
							//send(Socket, Indx, 2, 0);
							nextRead = 0;
							Sleep(250);
							closesocket(Socket);
						}
						else if(cbuff[0] == 'Y' && cbuff[1] == 'E')
						{
							std::cout << "\nPASSWORD CORRECT" << std::endl;
							std::cout << "\nINDEX: " << Indx;
							int indx = atoi(Indx);
							std::cout << "\nAFTER CONVERTING: " << indx << std::endl;
							send(Socket, Indx, 2, 0);
							//close previous windows and create another one
							//Sleep(1000);
							DestroyWindow(hwind);
							/*char log2Buff[3];
							ZeroMemory(log2Buff, sizeof(log2Buff));
							//char specialBuff[] = "??";
							sprintf(log2Buff,"%d", len); //log len to log2Buff
							send(Socket, log2Buff, sizeof(log2Buff), 0);
							send(Socket, buffs, len, 0);
							//special character
							send(Socket, specialBuff, sizeof(specialBuff), 0);
							send(Socket, "HELLO SERVER!", 13, 0);*/
							//CloseWindow(hWnd);
							//set phase 2...
							//PHASE = 2;
							//nextRead = 2; //set for handling messages for connected client

							wind2();
						}
						else if(cbuff[0] == 'E' && cbuff[1] == 'X') //user already exist
						{
							send(Socket, Indx, 2, 0);
							//closing socket cause this user is already logged in
							//wait for sure send was correctly passed
							Sleep(250);
							closesocket(Socket);
							nextRead = 0;
						}
					}
					/*else if(nextRead == 2)
					{
						int recres;
						char buffsor[3];
						ZeroMemory(buffsor, 3);
						recres = recv(Socket, buffsor, 2, 0);
						if(recres == SOCKET_ERROR)
						{
							int recErr;
							recErr = WSAGetLastError();
							std::cout << "\nrecERROR: " << recErr << std::endl;
						}
						std::cout << "\n\tMESSAGE: " << buffsor << std::endl;
					}*/
					break;
				}
		break;

	case WM_CREATE:
		if(PHASE == 0)
		{
		 HWND hButton = CreateWindowEx(NULL,
								L"BUTTON",
								L"OK",
								WS_VISIBLE|WS_CHILD|WS_TABSTOP,
								75,//x
								75,//y
								65,//weidth
								25,//height
								hWnd,
								(HMENU)IDC_MAIN_BUTTON,
								GetModuleHandle(NULL),
								NULL);

		 HGDIOBJ defFontOK = GetStockObject(DEFAULT_GUI_FONT);
		 SendMessage(hButton,
						WM_SETFONT,
						(WPARAM)defFontOK,
						NULL);//MAKELPARAM(FALSE,0));

		HWND hButtonC = CreateWindowEx(NULL,
						L"BUTTON",
						L"CANCEL",
						WS_VISIBLE|WS_CHILD|WS_TABSTOP,
						145,
						75,
						70,
						25,
						hWnd,
						(HMENU)IDC_MAIN_BUTTON_CANCEL,
						GetModuleHandle(NULL),
						NULL);

		 HGDIOBJ defFontCANC = GetStockObject(DEFAULT_GUI_FONT);
		 SendMessage(hButtonC,
						WM_SETFONT,
						(WPARAM)defFontCANC,
						NULL);//MAKELPARAM(FALSE,0));

		 logButt = CreateWindowEx(WS_EX_CLIENTEDGE,
								L"EDIT",
								L"Log-in",
								WS_VISIBLE|WS_CHILD,
								75,
								20,
								140,
								20,
								hWnd,
								(HMENU)IDC_EDIT_LOG,//(HMENU)IDC_MAIN_BUTTON_CANCEL,
								GetModuleHandle(NULL),
								NULL);
		 //default font
		 HGDIOBJ defFont = GetStockObject(DEFAULT_GUI_FONT);
		 SendMessage(logButt,
						WM_SETFONT,
						(WPARAM)defFont,
						NULL);//MAKELPARAM(FALSE,0));

		//SendMessage(h2Button, WM_SETTEXT, NULL, (LPARAM)L"TEXT");

		passButt = CreateWindowEx(WS_EX_CLIENTEDGE,
								L"EDIT",
								L"Password",
								WS_VISIBLE|WS_CHILD,
								75, //x
								40, //y
								140,//length
								20,//height
								hWnd,
								(HMENU)IDC_EDIT_PASS,//(HMENU)IDC_MAIN_BUTTON_CANCEL,
								GetModuleHandle(NULL),
								NULL);

		 HGDIOBJ defFontP = GetStockObject(DEFAULT_GUI_FONT);
		 SendMessage(passButt,
						WM_SETFONT,
						(WPARAM)defFontP,
						NULL);//MAKELPARAM(FALSE,0));
		}
		/*else if(PHASE == 2)
		{
			//proper window for client
			//incoming box
			HWND hButtonConnected = CreateWindowEx(NULL,
								L"BUTTON",
								L"OK",
								WS_VISIBLE|WS_CHILD|WS_TABSTOP,
								75,//x
								75,//y
								65,//weidth
								25,//height
								hWnd,
								(HMENU)IDC_MAIN_BUTTON_CONNECTED,
								GetModuleHandle(NULL),
								NULL);
				HGDIOBJ defFontOK_CONN = GetStockObject(DEFAULT_GUI_FONT);
				SendMessage(hButtonConnected,
						WM_SETFONT,
						(WPARAM)defFontOK_CONN,
						NULL);//MAKELPARAM(FALSE,0));

				hEditIn=CreateWindowEx(WS_EX_CLIENTEDGE,
				L"EDIT",
				L"",
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|
				ES_AUTOVSCROLL|ES_AUTOHSCROLL,
				50,
				120,
				400,
				200,
				hWnd,
				(HMENU)IDC_EDIT_IN,
				GetModuleHandle(NULL),
				NULL);
			if(!hEditIn)
			{
				MessageBox(hWnd,
					L"Could not create incoming edit box.",
					L"Error",
					MB_OK|MB_ICONERROR);
			}
			defFont = GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEditIn,
					WM_SETFONT,
					(WPARAM)defFont,
					MAKELPARAM(FALSE,0));
		}*/
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WinProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int asresult = WSAAsyncSelect(Socket, hWnd/*hwind2*/, WM_SOCKET2, FD_READ | FD_CLOSE);
	//char specialBuff2[] = "!!";
	switch(msg)
	{
		case WM_DESTROY:
				std::cout << "\nWINDOW RECEIVED DESTROY MESSAGE " << std::endl;
				WSAAsyncSelect(Socket, hWnd, WM_SOCKET2, 0);

				char log2Buff[3];
				char messagelen[3];
				ZeroMemory(log2Buff, sizeof(log2Buff));
				ZeroMemory(messagelen, sizeof(messagelen));
				//char specialBuff[] = "??";
				sprintf(log2Buff,"%d", len); //log len to log2Buff
				sprintf(messagelen,"%d", len2);
				send(Socket, log2Buff, sizeof(log2Buff), 0);
				send(Socket, buffs, len, 0);
				//special character
				send(Socket, "!!", sizeof(specialBuff), 0);					//send length of message
				send(Socket, Indx, 3, 0);
				//send(Socket, buffs2, len2, 0);
				WSAAsyncSelect(Socket, hWnd, WM_SOCKET, FD_READ);
				//close socket entirely
				Sleep(500);
				closesocket(Socket);

			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_MAIN_BUTTON_CONNECTED:
					/*len2 = SendMessage(hEditIn2, WM_GETTEXTLENGTH, 0, 0);
					std::cout << "Length From edit: " << len2 << std::endl;
					//LPSTR text[len];
					lpstr2 = (LPSTR)malloc(len2 + 1);
					buffs2 = new char[len2];

					SendMessage(hEditIn2, WM_GETTEXT, (WPARAM)len2,
									(LPARAM)lpstr2);

					//GetWindowText(hEditIn2, lpstr2, len2);

					int ic;
					int jc;
					jc=0;
					//free(lpstr2);
					/*ZeroMemory(buffs2, sizeof(buffs2));
					for(ic = 0; ic < len2; ic++)
					{
						buffs2[ic] = lpstr2[jc];
						jc = jc + 1;
					}
					ic=0;
					//log
					std::cout << "\nFROM EDIT: ";
					while(ic < len2)
					{
						std::cout << buffs2[ic];
						ic++;
					}*/

					len2 = GetWindowTextLength(hEditIn2);
					std::cout << "Length Edit: " << len2 << std::endl;
					//allocating memory for buffer
					lpstr2 = (LPSTR) VirtualAlloc((LPVOID)  NULL,
						(DWORD) (len2 + 1), MEM_COMMIT,
						PAGE_READWRITE);
					GetWindowText(hEditIn2, (LPWSTR)lpstr2, len2 + 1);

					buffs2 = new char[len2];

					int ic;
					int jc;
					jc=0;

					ZeroMemory(buffs2, sizeof(buffs2));
					for(ic = 0; ic < len2; ic++)
					{
						buffs2[ic] = lpstr2[jc];
						jc = jc + 2;
					}
					ic=0;
					//login
					std::cout << "\nFROM EDIT: ";
					while(ic < len2)
					{
						std::cout << buffs2[ic];
						ic++;
					}
					//free memory
					VirtualFree(lpstr2, 0, MEM_RELEASE);

					WSAAsyncSelect(Socket, hWnd, WM_SOCKET2, 0);
					std::cout << "\n\tCONNECTED OK WAS PRESSED " << std::endl;
					char log2Buff[3];
					char messagelen[3];
					ZeroMemory(log2Buff, sizeof(log2Buff));
					ZeroMemory(messagelen, sizeof(messagelen));
					//char specialBuff[] = "??";
					sprintf(log2Buff,"%d", len); //log len to log2Buff
					sprintf(messagelen,"%d", len2);
					send(Socket, log2Buff, sizeof(log2Buff), 0);
					send(Socket, buffs, len, 0);
					//special character
					send(Socket, specialBuff, sizeof(specialBuff), 0);
					//send length of message
					send(Socket, messagelen, 3, 0);
					send(Socket, buffs2, len2, 0);
					WSAAsyncSelect(Socket, hWnd, WM_SOCKET2, FD_READ | FD_CLOSE);
				break;
			}
			break;

		case WM_CREATE:
			//proper window for client
			//incoming box
			hButtonConnected = CreateWindowEx(NULL,
								L"BUTTON",
								L"OK",
								WS_VISIBLE|WS_CHILD|WS_TABSTOP,
								300,//x
								380,//y
								65,//weidth
								25,//height
								hWnd,
								(HMENU)IDC_MAIN_BUTTON_CONNECTED,
								GetModuleHandle(NULL),
								NULL);
				defFontOK_CONN = GetStockObject(DEFAULT_GUI_FONT);
				SendMessage(hButtonConnected,
						WM_SETFONT,
						(WPARAM)defFontOK_CONN,
						NULL);//MAKELPARAM(FALSE,0));

				hEditIn=CreateWindowEx(WS_EX_CLIENTEDGE,
				L"EDIT",
				L"",
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|
				ES_AUTOVSCROLL|ES_AUTOHSCROLL,
				115,
				45,
				250,
				280,
				hWnd,
				(HMENU)IDC_EDIT_IN,
				GetModuleHandle(NULL),
				NULL);
			if(!hEditIn)
			{
				MessageBox(hWnd,
					L"Could not create incoming edit box.",
					L"Error",
					MB_OK|MB_ICONERROR);
			}
			defFont = GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEditIn,
					WM_SETFONT,
					(WPARAM)defFont,
					MAKELPARAM(FALSE,0));

			hEditIn2=CreateWindowEx(WS_EX_CLIENTEDGE,
				L"EDIT",
				L"",
				WS_CHILD|WS_VISIBLE|ES_MULTILINE|
				ES_AUTOVSCROLL|ES_AUTOHSCROLL,
				115,
				340,
				250,
				30,
				hWnd,
				(HMENU)IDC_EDIT_IN,
				GetModuleHandle(NULL),
				NULL);
			if(!hEditIn2)
			{
				MessageBox(hWnd,
					L"Could not create incoming edit box.",
					L"Error",
					MB_OK|MB_ICONERROR);
			}
			defFont = GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEditIn2,
					WM_SETFONT,
					(WPARAM)defFont,
					MAKELPARAM(FALSE,0));
			break;

		case WM_SOCKET2:
				switch(WSAGETSELECTEVENT(lParam))
				{
					case FD_READ:
						//std::cout << "\nREAD MESSAGE FOR CONNECTED CLIENT! " << std::endl;
						int asresult = WSAAsyncSelect(wParam, hWnd, WM_SOCKET2, 0);
						ioctlsocket(wParam,FIONBIO , 0);
						char mesLen[3];
						char buffproper[128];
						ZeroMemory(buffproper, sizeof(buffproper));
						ZeroMemory(mesLen, sizeof(mesLen));
						//std::cout << "\nPROPER WINDOW REC SOMETHING " << std::endl;
						recv(Socket, mesLen, 3, 0);
						Sleep(500);
						int lmsg = atoi(mesLen);
						recv(Socket, buffproper, lmsg, 0);
						std::cout << buffproper << std::endl;
						int wsasync = WSAAsyncSelect(wParam, hWnd, WM_SOCKET, FD_READ);

						size_t sizeBuff = strlen(buffproper)+1;
						wchar_t* wchar = new wchar_t[sizeBuff];
						mbstowcs(wchar, buffproper, sizeBuff);

						addEndText(hEditIn, (LPCWSTR)wchar);
						addEndText(hEditIn, (LPCWSTR)L"\r\n");
					break;
				}
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void mSock()
{
	WSADATA windata;
	WSAStartup(MAKEWORD(2, 2), &windata);//use wisock vers 2.2

	//create socket
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// ip adress for host
	struct hostent *host;
	host = gethostbyname("localhost");
	if(host == NULL)
	{
		int getHostErr = WSAGetLastError();
		std::cout << "ERROR: " << getHostErr;
		return;
	}
	//address structure
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(8888);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *(( unsigned long*)host->h_addr);

	//connect socket to server
	int conErr = connect( Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr) );
	if(conErr == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		std::cout << "\nCONNECTION ERROR: " << err << std::endl;
		return;
	}
	//Sleep(500);
	//set for handling socket massages
	//int asres = WSAAsyncSelect(Socket, hwind, WM_SOCKET, FD_READ | FD_CLOSE);
	//int asres2 = WSAAsyncSelect(Socket, hwind, WM_SOCKET, 0);

	char logBuff[3];
	char passBuff[3];
	char testBuff[3];

	ZeroMemory(logBuff, 3);
	ZeroMemory(passBuff, 3);
	sprintf(logBuff,"%d", len); //log len to logBuff
	sprintf(passBuff, "%d", lenPass);//pass len to passBuff
	//set block mode
	WSAAsyncSelect(Socket, hwind, WM_SOCKET, 0);
	ioctlsocket(Socket,FIONBIO , 0);
	//send login length to serv
	int bufHead = send(Socket, logBuff, sizeof(logBuff), 0);
	//Sleep(200);
	//sending login via socket
	int ldata2 = send(Socket, buffs, len, 0);

	//sending buffhead with length of passw
	int bufHeadP = send(Socket, passBuff, sizeof(passBuff), 0);
	//send password through socket
	int passData = send(Socket, buffPass, lenPass, 0);
	if(ldata2 == SOCKET_ERROR)
	{
		std::cout << "\n\n\tERROR OCCURED:(" << std::endl;
		int error = WSAGetLastError();
		if(error != WSAEWOULDBLOCK && error != 0)
		{
			std::cout << "\n\nERROR code: " << error;
			//shutdown sock
			shutdown(Socket, SD_SEND);
			closesocket(Socket);
			return;
		}
	}
	WSAAsyncSelect(Socket, hwind, WM_SOCKET, FD_READ | FD_CLOSE);
	//return 0
}

int wind()
{
	HINSTANCE hInst;
	hInst = 0;
	//WNDCLASSEX temp;

	//window registration
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
	wndClass.cbClsExtra = NULL;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = NULL;
	wndClass.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = NULL;
	wndClass.hIconSm = NULL;
	wndClass.hInstance = hInst;
	wndClass.lpfnWndProc = (WNDPROC)WinProc;
	wndClass.lpszClassName = L"WndClass";
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	//registration wndClass
	if(!RegisterClassEx(&wndClass))
	{
		int res = GetLastError();
		MessageBox(NULL,
			L"Window class creation failed",
			L"Window Class Failed",
			MB_ICONERROR);
		std::cout << "ERROR CODE: " << res << std::endl;

		getchar();
		return 1;
	}
	//create window based on wndClass
	hwind = CreateWindowEx(NULL,
					L"WndClass",
					L"Win async client...",
					WS_SIZEBOX,// | WS_SYSMENU,//WS_OVERLAPPEDWINDOW,
					200,//x
					200,//y
					300,//width
					150,//height
					NULL,
					NULL,
					hInst,
					NULL);


	if(!hwind)
	{
		int err_result = GetLastError();

		MessageBox(NULL,
				L"Window create failed...",
				L"Window fail...",
				MB_ICONERROR);

		std::cout << "ERROR: " << err_result;
	}

	ShowWindow(hwind, 9);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

int wind2()
{
	HINSTANCE hInst2 = 0;
//	WNDCLASSEX temp;

	//window registration
	WNDCLASSEX wndClass2;
	ZeroMemory(&wndClass2, sizeof(WNDCLASSEX));
	wndClass2.cbClsExtra = NULL;
	wndClass2.cbSize = sizeof(WNDCLASSEX);
	wndClass2.cbWndExtra = NULL;
	wndClass2.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wndClass2.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass2.hIcon = NULL;
	wndClass2.hIconSm = NULL;
	wndClass2.hInstance = hInst2;
	wndClass2.lpfnWndProc = (WNDPROC)WinProc2;
	wndClass2.lpszClassName = L"WndClass2";
	wndClass2.lpszMenuName = NULL;
	wndClass2.style = CS_HREDRAW | CS_VREDRAW;

	if(!RegisterClassEx(&wndClass2))
	{
		int res = GetLastError();
		MessageBox(NULL,
			L"Window class creation failed",
			L"Window Class Failed",
			MB_ICONERROR);
		std::cout << "ERROR CODE: " << res << std::endl;

		getchar();
		return 1;
	}

	hwind2 = CreateWindowEx(NULL,
					L"WndClass2",
					L"Win async serv...",
					WS_OVERLAPPEDWINDOW,
					200,
					200,
					500,
					450,
					NULL,
					NULL,
					hInst2,
					NULL);

	if(!hwind2)
	{
		int err_result = GetLastError();

		MessageBox(NULL,
				L"Window create failed...",
				L"Window fail...",
				MB_ICONERROR);

		std::cout << "ERROR: " << err_result;
	}
	ShowWindow(hwind2, 9);
	//int asresult = WSAAsyncSelect(Socket, hwind2, WM_SOCKET2, FD_READ | FD_CLOSE);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void addEndText(HWND hEdit, LPCWSTR newText)
{
	int TextLen = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(hEdit, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)newText);
}
