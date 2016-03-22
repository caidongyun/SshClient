/*
 * Client.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: ecejnj
 */

#include <Client.h>

#include <Config/Configuration.h>
#include <Config/EvipConf.h>
#include <ssh/Connection.h>

#include <Util/Log.h>

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

            std::string result = conn.execute( EvipConf::LBE_CMD);

            if ( !result.empty()){
                rc = 0;
                std::cout << result;
                EvipConf evipConf;

                evipConf.parseLBE( result);
                for ( auto &lbe: evipConf.getLbeList()){
                    INFO( "Evip LBE:%s", lbe.c_str());
                }
            }
        }
    }

    return rc;
}

} /* namespace ssh_cli */
