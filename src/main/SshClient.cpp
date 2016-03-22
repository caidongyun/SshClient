//============================================================================
// Name        : SshClient.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================



#include <Client.h>



int main(int argc, const char *argv[])
{


    ssh_cli::Client client;
    return client.execute( argc, argv);



}
