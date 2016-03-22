/*
 * Client.h
 *
 *  Created on: Mar 18, 2016
 *      Author: ecejnj
 */

#ifndef MAIN_CLIENT_H_
#define MAIN_CLIENT_H_



namespace ssh_cli {

class Client {
public:
    explicit Client( );
    virtual ~Client();

    int execute( int , const char* argv[]);
};

} /* namespace ssh_cli */

#endif /* MAIN_CLIENT_H_ */
