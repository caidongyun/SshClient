/*
 * Connection.h
 *
 *  Created on: Mar 18, 2016
 *      Author: ecejnj
 */

#ifndef SSH_CONNECTION_H_
#define SSH_CONNECTION_H_

#include <libssh/libssh.h>
#include <string.h>

#include <Config/Configuration.h>

namespace ssh_cli {

class Connection {
    const Configuration &conf;
    ssh_session session;
    bool connected = false;

    int verify_knownhost() ;

    int authenticate_kbdint();
    int authenticate_kbdint_pwd();
    int authenticate_passwd();
    int authenticate( );


    /**
     * Release resources
     */
    void release(){
        if ( session){
            if ( connected){
                ssh_disconnect( session);
                connected = false;
            }
            ssh_free( session);
            session = nullptr;
        }
    }

public:
    explicit Connection(
            const Configuration &);
    virtual ~Connection();


    /**
     * Connect (and authenticate)
     *
     * @return true if connected
     */
    bool connect();


    /**
     * Execute provided command
     *  It should be in connected state
     *
     * @param coman
     *
     * @return string from execution
     */
    std::string execute( const std::string &) const;

    bool isConnected() const {
        return connected;
    }
};

} /* namespace ssh_cli */

#endif /* SSH_CONNECTION_H_ */
