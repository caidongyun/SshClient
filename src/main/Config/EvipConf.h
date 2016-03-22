/*
 * EvipConf.h
 *
 *  Created on: Mar 22, 2016
 *      Author: ecejnj
 */

#ifndef MAIN_CONFIG_EVIPCONF_H_
#define MAIN_CONFIG_EVIPCONF_H_

#include <string>
#include <list>


namespace ssh_cli {

class EvipConf {
    std::list<std::string> lbeList;

    static const std::string VIP_SUFFIX;
public:
    EvipConf();
    virtual ~EvipConf();

    /**
     * Get command for retrieving LBE agents
     */
    static const std::string LBE_CMD;

    /**
     * Parse LBE from input
     */
    bool parseLBE( const std::string &);

    std::list<std::string> getLbeList() const {
        return lbeList;
    }
};

} /* namespace ssh_cli */

#endif /* MAIN_CONFIG_EVIPCONF_H_ */
