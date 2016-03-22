/*
 * EvipConf.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: ecejnj
 */

#include <Config/EvipConf.h>

namespace ssh_cli {

const std::string EvipConf::VIP_SUFFIX = "%evip_macvlan0'";
const std::string EvipConf::LBE_CMD =
        "{ echo \"show agents alb_tr\"; sleep 1;} "
        "| telnet $(/opt/vip/bin/getactivecontrol ) 25190 "
        "| sed -n '/^+.*LBE.*$/,/^+.*FE.*$/p' "
        "| grep \"ACTIVE\" | awk '{print $2}'";

EvipConf::EvipConf() {
    // TODO Auto-generated constructor stub

}

EvipConf::~EvipConf() {
    // TODO Auto-generated destructor stub
}

bool
EvipConf::parseLBE( const std::string &lbes){

    if ( lbes.empty()) return false;

    size_t pos = 0;

    do{
        size_t cur = lbes.find( '\n', pos);
        if ( cur == std::string::npos) cur = lbes.size();

        lbeList.push_back( lbes.substr( pos, cur - pos) + VIP_SUFFIX);

        pos = cur + 1;
    } while ( pos < lbes.size());

    return true;
}

} /* namespace ssh_cli */
