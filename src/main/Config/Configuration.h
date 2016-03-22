/*
 * Configuration.h
 *
 *  Created on: Feb 5, 2016
 *      Author: ecejnj
 */

#ifndef CONFIGURATION_CONFIGURATION_H_
#define CONFIGURATION_CONFIGURATION_H_

#include <string>
#include <memory>
#include <list>


#include <boost/program_options.hpp>


namespace ssh_cli {

class Configuration {
    std::string fileName;
    int argc;
    const char **argv;

    /**
     * Parse generic options
     */
    bool parse_options();

    /**
     * Parse specific options
     */
    bool parse_specific_option();


    bool pwd_support, kb_support;
    std::string password, host, port, user, cmd;

    /** Variables name **/
    static const std::string PWD_SUPPORT;
    static const std::string KB_SUPPORT;
    static const std::string PWD;

    static const std::string HOST_STR;
    static const std::string PORT_STR;
    static const std::string USR_STR;
    static const std::string CMD_STR;


    template<typename T> void  parseOption(
            const std::string &,
            T &,
            const boost::program_options::variables_map &);

public:
    Configuration(int argc, const char *argv[]);
    virtual ~Configuration();

    std::string getFileName( ) const {
        return fileName;
    }


    /**
     * Load and apply configuration
     * (This method must be explicitly invoked)
     * It just exist to allow test run without a configuration file
     *
     * @return true if OK
     */
    bool loadConfiguration();

    bool isKbSupport() const {
        return kb_support;
    }

    const std::string& getPassword() const {
        return password;
    }

    bool isPwdSupport() const {
        return pwd_support;
    }

    const std::string& getCmd() const {
        return cmd;
    }

    const std::string& getHost() const {
        return host;
    }

    const std::string& getPort() const {
        return port;
    }

    const std::string& getUser() const {
        return user;
    }
};

} /* namespace ssh_cli */

#endif /* CONFIGURATION_CONFIGURATION_H_ */
