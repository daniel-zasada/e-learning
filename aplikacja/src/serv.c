#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "stdafx.h"
#include <conio.h>
#include <string>
#include <WinSock2.h>
#include <windows.h>
#include <cstdlib>

/* uncomment for applications that use vectors */
#include <vector>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#define EXAMPLE_HOST "localhost"
#define EXAMPLE_USER "root"
#define EXAMPLE_PASS "sqlroot.9194"
#define EXAMPLE_DB "world"
#define CORRECT_DATA

#pragma comment(lib, "ws2_32.lib")
#define WM_SOCKET 100
#define IDC_EDIT_IN 102
#define IDC_EDIT_OUT 104

enum REMOVE_FLAG {SET, NOT_SET};
enum REMOVE_FLAG remFlag = NOT_SET;

const int maxClients = 25;

int Client = 0;
int dcClient = 0;
int res;
int resasync;
int indicator = 0;
int indeKS = 0;
static int j = 0;
static int i = 0;

char strings[128];
char passStrings[128];
SOCKET SocketM[maxClients];
SOCKET ServSock = NULL;
SOCKET Temp[2];
sockaddr sockAddrClient;
u_long mode;

HINSTANCE hInst, hPrevInstance;
HWND hWnd, hEditIn, hEditOut;

HGDIOBJ defFont;

//SOCKADDR_IN Sockadr;

int wind();
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int connectorplus(void);
void addEndText(HWND hEdit, LPCWSTR newText);
void removeText(HWND, LPCWSTR);
int userChecking(std::string );
int userChecking(char *userName);
void print_user();

//vectors
std::vector<int> myvectr; //vector containing indexes of disconnected client
std::vector<std::string> vectrLogged; //vector containing names of currently logged users
std::vector<int> indxvectr; //indexes of logged clients

using namespace std;

void add_user(string name, string pass)
{
	int ict=0;

	sql::Driver *driver = get_driver_instance();
	std::auto_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3307", "root", "sqlroot.9194"));
	con->setSchema("mybase");
	std::auto_ptr<sql::Statement> stmt(con->createStatement());

	//how many objects
	std::auto_ptr<sql::ResultSet> result(stmt->executeQuery("SELECT name FROM muser"));
	while( result->next() )
	{
		ict++;
	}
	cout << "Exist " << ict << "Elements " << endl;

	//check if user already exist
	std::auto_ptr<sql::ResultSet> resultcheck(stmt->executeQuery("SELECT name FROM muser"));
	while( resultcheck->next() )
	{
		string pName = resultcheck->getString("name");
		if( pName == name )
		{
			cout << "User already exist " << endl;
			return;
		}
	}
	std::auto_ptr<sql::PreparedStatement> prestate(con->prepareStatement("INSERT INTO muser(id_key, name, pass) VALUES(?, ?, ?)"));

	prestate->setInt(1, ict);
	prestate->setString(2, name);
	prestate->setString(3, pass);
	prestate->executeUpdate();
	ict++;
}

int main(int argc, const char **argv)
{
	wind();
//	int action;

  cout << "Done." << endl;

  _getch();
  return EXIT_SUCCESS;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DESTROY:
		{
			/*PostQuitMessage(0);
			shutdown(ServSock, SD_BOTH);
			closesocket(ServSock);
			WSACleanup();*/
			DestroyWindow(hWnd);
			return 0;
		}
		break;

		case WM_CREATE:
				//incoming box
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

			WSADATA wsData;
			//chose vers
			res = WSAStartup(MAKEWORD(2,2), &wsData);
			if(res != 0) //initialization failed error code in res
			{
				std::cout << "Init failed: " << res << std::endl;
				WSACleanup();
				getchar();
				return 1;
			}
			//create this socket
			ServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(ServSock == INVALID_SOCKET)
			{
				int errorCode = WSAGetLastError();
				std::cout << "Creationfailed with err code: " << errorCode;
				WSACleanup();
				getchar();
				return 1;
			}

			//set what to listening
			SOCKADDR_IN Sockadr;
			Sockadr.sin_family = AF_INET;
			Sockadr.sin_addr.s_addr = htonl(INADDR_ANY);
			Sockadr.sin_port = htons(8888);

			//bind Sockadr with socket that was created earlier
			//bind(ServSock, (SOCKADDR*)(&Sockadr), sizeof(Sockadr));
			if( ::bind(ServSock, (SOCKADDR*)(&Sockadr), sizeof(Sockadr)) == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				WSACleanup();
				getchar();
				return 1;
			}//set for handling messages
			resasync = WSAAsyncSelect(ServSock,
									hWnd,
									WM_SOCKET,
									(FD_CLOSE|FD_ACCEPT|FD_READ) ); //previous was also FD_WRITE
			if(resasync)
			{
				int aserr = WSAGetLastError();
				std::cout << "Async failed " << std::endl;
				std::cout << "ERROR: " << aserr;
				MessageBox(hWnd,
							L"WSAAsync ERROR",
							L"CRITIC ERROR",
							MB_ICONERROR);

				return 1;
			}
			//nonblockmode
			//mode = 1;
			//ioctlsocket(ServSock, FIONBIO, &mode);

			//listen on socket for incoming connection
			if(listen(ServSock, SOMAXCONN)==SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				std::cout << "APPLICATION CLOSED WITH ERROR: " << error << std::endl;
				return 1;
			}

			break;

		case WM_COMMAND:
			break;

		case WM_SOCKET:
			switch(WSAGETSELECTEVENT(lParam))
			{
				case FD_CLOSE:
					MessageBox(hWnd,
							L"Client disconnected",
							L"Conn closed",
							MB_ICONINFORMATION|MB_OK);
					//closesocket(wParam);
					std::cout << "\nFD_CLOSE: indeKS = " << indeKS << std::endl;
					//myvectr.clear();
					myvectr.push_back(indeKS);
					Client = Client -1;
					//send index of dc client to vector
					std::cout << "\nEL FROM INDEX DISCONNECTED: " << indeKS << std::endl;
					//set flag indicating something was removed
					remFlag = SET;
					SendMessage(hEditIn, WM_SETTEXT,NULL, (LPARAM)L" ");
					//call removeText()
					removeText(hEditIn, L"Connected\r\n");
					break;

				case FD_ACCEPT:

					if( Client < (maxClients/* + addition*/) )
					{
						if(Client < 1 && remFlag == NOT_SET)
						{
							int size = sizeof(sockaddr);
							SocketM[Client] = accept(wParam, &sockAddrClient, &size);
							addEndText(hEditIn, L"Connected\r\n");
							//send actual socket index to proper client
							//first conert value from int to char
							char IndxBuff[3];
					        sprintf(IndxBuff, "%d", Client);
							std::cout << "\n\nCLIENT INDEX " << IndxBuff << std::endl;
							send(SocketM[Client], IndxBuff, sizeof(IndxBuff), 0);

							remFlag = NOT_SET;
							Client++;
						}
						//check remFlag
						else if(remFlag == SET /*&& Client >= 1*/)//someone disconnected
						{
							std::cout << "\n\nVECINDX: " << myvectr[0];
							std::cout << "\nCLIENT: " << Client;
							int vecIndx = myvectr[0];
							myvectr.clear();//clearing vector
							//remFlag = NOT_SET;
							int size = sizeof(sockaddr);
							SocketM[/*Client*/vecIndx] = accept(wParam, &sockAddrClient, &size);
							addEndText(hEditIn, L"Connected\r\n");

							char IndxBuff[3];
							//ZeroMemory(IndxBuff, 3);
							sprintf(IndxBuff, "%d", vecIndx);
					        //sprintf(IndxBuff, "%d", Client);
							std::cout << "\n\nCLIENT INDEX " << IndxBuff << std::endl;

							//send index to client who own it
							send(SocketM[/*Client*/vecIndx], IndxBuff, sizeof(IndxBuff), 0);
							Client++;//increment client number
							remFlag = NOT_SET;
						}
						else if(remFlag == NOT_SET && Client >= 1)
						{
							int size = sizeof(sockaddr);
							SocketM[Client] = accept(wParam, &sockAddrClient, &size);
							if(SocketM[Client] == INVALID_SOCKET)
							{
								MessageBox(hWnd, L"CANT ACCEPT", L"WRR", MB_ICONINFORMATION);
								int res = WSAGetLastError();
								WSACleanup();
							}//add end text
							addEndText(hEditIn, L"Connected\r\n");

							std::cout << "CLIENT CONNECTED" << std::endl;
							char IndxBuff[3];
					        sprintf(IndxBuff, "%d", Client);
							//sending index of connected client
							send(SocketM[Client], IndxBuff, sizeof(IndxBuff), 0);
							std::cout << "\n\nSEND CLIENT INDEX " << IndxBuff << std::endl;
							Client++;
						}
					}
					break;

				case FD_READ:

					if(indicator == 1)
					{	//receiving index of client who disconnected
						WSAAsyncSelect(wParam, hWnd, WM_SOCKET, 0);
						char indexBuff[3];
						ZeroMemory(indexBuff, sizeof(indexBuff));
						recv(wParam, indexBuff, 3, 0);
						indeKS = atoi(indexBuff);
						//std::cout << "ELEMENT OF INDEX: " << indeKS << std::endl;
						std::cout << "INDEX CHARR: " << indexBuff[0] << std::endl;
						WSAAsyncSelect(wParam, hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);

						indicator = 0;
					 }
					else if(indicator == -1)
					{
							//index of dc client- clicked on X
							WSAAsyncSelect(wParam, hWnd, WM_SOCKET, 0);
							char indexBuff[3];
							ZeroMemory(indexBuff, sizeof(indexBuff));
							recv(wParam, indexBuff, 3, 0);
							indeKS = atoi(indexBuff);
							indxvectr.push_back(indeKS);
							//std::cout << "ELEMENT OF INDEX: " << indeKS << std::endl;
							std::cout << "INDEX CONNECTED!: " << indexBuff[0] << std::endl;
							WSAAsyncSelect(wParam, hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);

							indicator = 0;
					}
					else if(indicator == 0)
					{
						//MessageBox(hWnd, L"SERV REC", L"SV", MB_OK);
						//setting back to blocking mode
						//disable by using WSAAsyncSelect//[Client-1]
						int wsasync = WSAAsyncSelect(wParam/*SocketM[Client-1]*/, hWnd, WM_SOCKET, 0);
						ioctlsocket(wParam,FIONBIO , 0);
						if(wsasync == SOCKET_ERROR)
						{
							std::cout << "SETTING BACK TO BLOCK MODE FAILED! " << std::endl;
							break;
						}
						char incbuf[3];
						char incbufPass[3];

						//int bufr = SOCKET_ERROR;
						ZeroMemory(incbuf, sizeof(incbuf));

						int buffHead = recv(wParam/*SocketM[Client-1]*/, incbuf, sizeof(incbuf), 0);

						//convert val from buf to int
						int atoiRes = atoi(incbuf);

						ZeroMemory(strings, sizeof(strings));
						//bufr == SOCKET_ERROR;
						//Sleep(500);//!
						int dt = recv(wParam/*SocketM[Client-1]*/,
									strings,
									atoiRes,
									0);
						if(dt != 0)
						{
							int errsock = WSAGetLastError();
							if(errsock != WSAEWOULDBLOCK && errsock != 0)
							{
								std::cout << "\n\tERR CODE " << errsock;
								//shutdown(SocketM[Client]);
								closesocket(SocketM[Client-1]);
								break;
							}
						}
						//receiving password header
						ZeroMemory(incbufPass, sizeof(incbufPass));
						int buffHead2 = recv(wParam/*SocketM[Client-1]*/, incbufPass, sizeof(incbufPass), 0);
						//checking if special character was received
						if(incbufPass[0] == '?')
						{
							int asresult = WSAAsyncSelect(wParam, hWnd, WM_SOCKET, 0);
							char connrec[128];
							char messagelen[4];

							ZeroMemory(connrec, sizeof(connrec));
							ZeroMemory(messagelen, sizeof(messagelen));
							recv(wParam, messagelen, 3, 0);
							Sleep(250);
							int msgLen = atoi(messagelen);
							recv(wParam, connrec, msgLen, 0);
							std::cout << "\nRECV: " << connrec << std::endl;
							std::cout << "CLLENT: " << Client << std::endl;
							//show index of all connected
							for(int icnt=0; icnt<indxvectr.size(); icnt++)
							{
								std::cout << "VECTR: " << indxvectr[icnt] << std::endl;
							}
							//resend this to all clients
							//int loops;
							for(int loops=0; loops< indxvectr.size(); /*Client;*/ loops++)
							{	//sending length of message to every client
								int conix = indxvectr[loops];
								int result = send(SocketM[conix], messagelen, 3, 0);
								int res = send(SocketM[conix], connrec, msgLen, 0);
								if(res == SOCKET_ERROR)
								{
									int serr = WSAGetLastError();
									std::cout << "\nSPECIAL ERROR: " << serr << std::endl;
								}
							}
							//do something
							std::cout << "\nSPECIAL CHARACTER RECEIVED!" << std::endl;
							int wsasync = WSAAsyncSelect(wParam, hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
							//indicator = 0;
						}
						else if(incbufPass[0] == '!')
						{
							std::cout << "\nSOMEONE WANT TO DISCONNECT! " << std::endl;
							//retrieve index of client
							WSAAsyncSelect(wParam, hWnd, WM_SOCKET, 0);
							//set block mode
							ioctlsocket(wParam,FIONBIO , 0);
							char indexBuff[3];
							ZeroMemory(indexBuff, sizeof(indexBuff));
							recv(wParam, indexBuff, 3, 0);
							indeKS = atoi(indexBuff);
							std::cout << "ELEMENT OF INDEX: " << indeKS << std::endl;
							std::cout << "INDEX CHARR: " << indexBuff[0] << std::endl;

							//check indxvectr
							for(int iv=0; iv<indxvectr.size(); iv++)
							{
								int inum = 0; // iv - 1;
								if(indxvectr[iv] == indeKS)
								{
									indxvectr.erase(indxvectr.begin() + iv );
									//vectrLogged.erase(vectrLogged.begin() + iv);
								}
							}
							WSAAsyncSelect(wParam, hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);

							indicator = 0;
						}
						else{
						int atoiResPass = atoi(incbufPass);
						//password
						ZeroMemory(passStrings, sizeof(passStrings));
						//Sleep(500);//!!
						int passdt = recv(wParam/*SocketM[Client-1]*/,
											passStrings,
											atoiResPass,
											0);
						//std::cout << strings << std::endl;
						//std::cout << passStrings << std::endl;
						if(strlen(strings)>=0) // && strlen(passStrings)>0)
						{
							std::cout << "\n\nLENGTH > 0\n";
							std::cout << strings << std::endl;
							std::cout << passStrings << std::endl;

								int connectRes = connectorplus();
								if(connectRes == -1)
								{
									char bufforz[2];
									ZeroMemory(bufforz, 2);
									//shutdown(SocketM[Client-1], SD_RECEIVE);
									indicator = 1;
									//set again for handling message
									int wsasync = WSAAsyncSelect(wParam/*SocketM[Client-1]*/, hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
									send(wParam/*SocketM[Client-1]*/, "NO", 2, 0);
								}
								else if(connectRes == 0)
								{
									indicator = 1;
									int wsasync = WSAAsyncSelect(SocketM[Client-1], hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
									send(wParam/*SocketM[Client-1]*/, "NO", 2, 0);
								}
								else if(connectRes == 1)
								{
									indicator = -1; //for fetching index;
									print_user();
									//indxvectr.push_back(indeKS);
									int wsasync = WSAAsyncSelect(wParam/*SocketM[Client-1]*/, hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
									send(wParam/*(SocketM[Client-1]*/, "YE", 2, 0);
								}
								else if(connectRes == 2)
								{
									indicator = 1;
									int wsasync = WSAAsyncSelect(wParam /*SocketM[Client-1]*/, hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
									send(wParam/*SocketM[Client-1]*/, "EX", 2, 0);
								}
						}
					}
				}
					break;
			}
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int wind()
{
	hInst = 0;
//	WNDCLASSEX temp;

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

	HWND hWnd = CreateWindowEx(NULL,
					L"WndClass",
					L"Win async serv...",
					WS_OVERLAPPEDWINDOW,
					200,
					200,
					640,
					480,
					NULL,
					NULL,
					hInst,
					NULL);

	if(!hWnd)
	{
		int err_result = GetLastError();

		MessageBox(NULL,
				L"Window create failed...",
				L"Window fail...",
				MB_ICONERROR);

		std::cout << "ERROR: " << err_result;
	}
	ShowWindow(hWnd, 9);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}


int connectorplus(void)
{
	//using namespace std;
	//int action;
	std::cout << "Connector/C++ tutorial framework..." << std::endl;
	std::cout << std::endl;

  try {
	  /* INSERT TUTORIAL CODE HERE!*/
	  sql::Driver *driver = get_driver_instance();
	  std::auto_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3307", "root", "sqlroot.9194"));
	  con->setSchema("mybase");

	  std::auto_ptr<sql::Statement> stmt(con->createStatement());

	  //stmt->execute("CREATE TABLE IF NOT EXISTS mcountry(id_key INT, name VARCHAR(15))");
	  stmt->execute("CREATE TABLE IF NOT EXISTS muser(id_key INT, name VARCHAR(15), pass VARCHAR(15))");
	  //std::auto_ptr<sql::PreparedStatement> prestate(con->prepareStatement("INSERT INTO muser(id_key, name, pass) VALUES (?, ?, ?)"));
	  //std::auto_ptr<sql::ResultSet> *result;

	 /*for(int j = 0; j < 4; j++)
	 {
	  cout << "1: Add ";

	  cin >> action;
	  switch(action)
	  {
		case 1:
			string uName;
			string uPass;
			cout << "User name: ";
			cin >> uName;
			cout << "\nUser pass: ";
			cin >> uPass;
			add_user(uName, uPass);
			break;
	  }
	 }*/
	  std::string name;
	  string pass;

	  int cter = 0;
	  int occur = -1; //-1 mean's that user doesnt exist
	  std::auto_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM muser"));

	  while(res->next())
	  {
		name = res->getString("name");

		//cout << name << endl;
		if(name == strings)
		{
			std::cout << "El found!" << std::endl;
			//checking elements from vector for logins
			int logName = userChecking(name);
			if(logName == 1)//user already exist
			{
				//send(SocketM[Client-1], "EX", 2, 0);
				return 2;//user already exist
			}
			//check password
			std::auto_ptr<sql::PreparedStatement> prestate(con->prepareStatement("SELECT * FROM muser WHERE id_key=(?)"));
			prestate->setInt(1, cter);
			std::auto_ptr<sql::ResultSet> resPass(prestate->executeQuery());
			while(resPass->next())
			{
				pass = resPass->getString("pass");
				std::cout << "\n PASS!! " << pass;
				std::cout << "\n PASSTRINGS: " << passStrings;
				if(pass == passStrings)
				{
					//int counter;
					std::cout << "\nCORRECT LOGIN AND PASSWORD WELCOME!" << std::endl;
					vectrLogged.push_back(name);//sending login to vector
					/*for(counter=0; counter<vectrLogged.size(); counter++)
					{
						std::cout << "\nVECTR ELEMENTS: " << vectrLogged[counter] << std::endl;
					}*/
					//send(SocketM[Client-1], "YE", 2, 0);
					occur = 1;
					return 1;//correct pass and login
				}
				else
				{
					std::cout << "\nINCORRECT PASSWORD" << std::endl;
					//indicator = 1;
					//send(SocketM[Client-1], "NO", 2, 0);
					//indicator = 1;
					occur = 0;
					return 0; //user passed incorrect password only
				}
			}
		}
		cter++;
	  }
	  if(occur == -1)
	  {
		  char exinfo[] = "USERD";
		  std::cout << "\nUSER DOESNT EXIST" << std::endl;
		  //indicator = 1;
		  //send(SocketM[Client-1], "NO", 2, 0);
		  return -1; //-1 means incorrect login and password
	  }

	  /*do{
		res.reset(stmt->getResultSet());
		while(res->next())
		{
			cout << "Name: " << res->getString(1) << endl;
		}
	  } while(stmt->getMoreResults());*/

  } catch (sql::SQLException &e) {
    /*
      MySQL Connector/C++ throws three different exceptions:

      - sql::MethodNotImplementedException (derived from sql::SQLException)
      - sql::InvalidArgumentException (derived from sql::SQLException)
      - sql::SQLException (derived from std::runtime_error)*/

    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    /* what() (derived from std::runtime_error) fetches error message*/
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;

    //return 0;//EXIT_FAILURE;
  }

  cout << "\n\nDone." << endl;

}

void addEndText(HWND hEdit, LPCWSTR newText)
{
	int TextLen = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(hEdit, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)newText);
}

void removeText(HWND hEdit, LPCWSTR newText)
{
	int rClient = Client;
	//rClient = Client - addition;
	int loop;
	for(loop=0; loop<rClient; loop++)
	{
		addEndText(hEdit, newText);
	}

}

void incBox(HWND hEdit)
{

}

void print_user()
{
	for(int i=0; i<vectrLogged.size(); i++)
	{
		std::cout << "\nVECTR ELEMENTS: " << vectrLogged[i] << std::endl;
	}
}

int userChecking(std::string userName)
{
	int counter;
	//int sz = vectrLogged.size();
	for(counter=0; counter < vectrLogged.size(); counter++)
	{
		std::cout << "\nVECTR ELEMENTS: " << vectrLogged[counter] << std::endl;
		if(vectrLogged[counter] == userName)
		{
			std::cout << "\nUSER ALREADY LOGGED" << std::endl;
			return 1;
		}
		/*else
		{
			return 0; //user doesnt exist
		}*/
	}
	return 0;
}

int userChecking(char *userName)
{
	int counter;
	//int sz = vectrLogged.size();
	for(counter=0; counter < vectrLogged.size(); counter++)
	{
		std::cout << "\nVECTR ELEMENTS: " << vectrLogged[counter] << std::endl;
		if(vectrLogged[counter] == userName)
		{
			std::cout << "\nUSER ALREADY LOGGED" << std::endl;
			return 1;
		}
		/*else
		{
			return 0; //user doesnt exist
		}*/
	}
	return 0;
}



