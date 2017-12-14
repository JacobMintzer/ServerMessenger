#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

std::string checkError(char* errlog){
	errlog++;
	std::ifstream in_str("../../Twitter/Statistics/twitter_errlog.txt");

	std::string errorMessage="";
	if (!in_str.good()){
		std::cerr<<"can't open file"<<errlog<<std::endl;
	}
	else{
		std::stringstream buffer;
		buffer<<in_str.rdbuf();
		errorMessage=buffer.str();
		in_str.close();
		std::ofstream ofs;
		ofs.open(errlog, std::ofstream::out | std::ofstream::trunc);
		ofs.close();
		if (errorMessage.length()>3){
			return errorMessage;
		}
	}
	return "";
}

int main(int argc, char* argv[]){
	/*std::string ip;
	std::string port;
	std::ifstream readIP("ip.txt");
	readIP>>port;
	readIP.close();
*/
	
	int sd = socket( PF_INET, SOCK_STREAM, 0 );
	struct sockaddr_in server;
	server.sin_family = PF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	unsigned short port = 8123;
	server.sin_port = htons( port );
	int len = sizeof( server );
	if ( bind( sd, (struct sockaddr *)&server, len ) == -1 ){
		std::cerr<<"bind() failed"<<std::endl;
	}
	if ( listen( sd, 5 ) == -1 ){
		std::cerr<<"listen() failed"<<std::endl;
		return EXIT_FAILURE;
	}
	printf( "SERVER: Listener bound to port %d\n", port );
	struct sockaddr_in client;
	int fromlen = sizeof( client );
	int n;
	
	
	std::string error;
	int newsd;
	while (true){
		error=checkError(argv[1]);
		if(error.empty()){
			usleep(300000);
		}
		else{
			newsd = accept( sd, (struct sockaddr *)&client, (socklen_t *)&fromlen );
			n=send(newsd,error.c_str(),error.length(),0);
			if(n==42069){
				std::cout<<"ayyyy"<<std::endl;
			}
		}
	}
	return 0;
}