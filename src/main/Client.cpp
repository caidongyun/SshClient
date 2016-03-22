/*
 * Client.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: ecejnj
 */

#include <Client.h>

#include <Config/Configuration.h>
#include <ssh/Connection.h>

namespace ssh_cli {

Client::Client(){


}

Client::~Client() {

}

int
Client::execute( int argc, const char*argv[]){
    Configuration config( argc, argv);
    int rc = 1;

    if ( config.loadConfiguration()){
        Connection conn( config);

        if ( conn.connect()){

            std::string result = conn.execute( config.getCmd());

            if ( !result.empty()){
                rc = 0;
                std::cout << result;
            }

        }
    }

    return rc;
}

} /* namespace ssh_cli */
