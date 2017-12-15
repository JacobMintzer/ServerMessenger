#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <Windows.h>
#include <iostream>

#define BUFFER_SIZE 1024


void handleMessage(char* buffer){
	std::ofstream outfile;

  	outfile.open("errorlog.txt", std::ios_base::app);
  	outfile << buffer;
  	outfile.close(); 
  	std::wstring name( L"[!SetVariable status spook NeneListener] [!UpdateMeter Background NeneListener] [!Redraw NeneListener]" );
	const wchar_t* bang = name.c_str();
  	HWND rmWnd = FindWindow("DummyRainWClass", NULL);
	if (rmWnd){
		COPYDATASTRUCT cds;
		cds.dwData = 1;
		cds.cbData = (DWORD)(wcslen(bang) + 1) * sizeof(WCHAR);
		cds.lpData = (void*)bang;

		// Send bang to the Rainmeter window
		SendMessage(rmWnd, WM_COPYDATA, 0, (LPARAM)&cds);
	}
	else{
		std::cerr<<"didn't work"<<std::endl;
	}

}
int main(){
	/* create TCP client socket (endpoint) */
	int sd = socket( PF_INET, SOCK_STREAM, 0 );

	if ( sd < 0 ){
		perror( "socket() failed" );
		exit( EXIT_FAILURE );
	}

	std::ifstream in_str("IP.txt");
	std::string IP;
	in_str>>IP;
	struct hostent * hp = gethostbyname( IP.c_str() );
	//struct hostnet *hp=gethostbyname("linux00.cs.rpi.edu");

	if ( hp == NULL ){
		fprintf( stderr, "ERROR: gethostbyname() failed\n" );
		return EXIT_FAILURE;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	memcpy( (void *)&server.sin_addr, (void *)hp->h_addr, hp->h_length );
	unsigned short port = 8123;
	server.sin_port = htons( port );

	printf( "server address is %s\n", inet_ntoa( server.sin_addr ) );


	printf( "connecting to server.....\n" );
	if ( connect( sd, (struct sockaddr *)&server, sizeof( server ) ) == -1 ){
		perror( "connect() failed" );
		return EXIT_FAILURE;
	}

	int n;
	while (true){
		char buffer[ BUFFER_SIZE ];
		n = read( sd, buffer, BUFFER_SIZE - 1 );    /* BLOCKING */

		if ( n == -1 ){
			perror( "read() failed" );
			return EXIT_FAILURE;
		}
		else if ( n == 0 ){
			printf( "Rcvd no data; also, server socket was closed, sleeping for 5 minutes\n" );
			usleep(600000);
		}
		else{
			buffer[n] = '\0';    /* assume we rcvd text-based data */
			printf( "Rcvd from server: %s\n", buffer );
			handleMessage(buffer);
		}
	}
close( sd );

return EXIT_SUCCESS;
}