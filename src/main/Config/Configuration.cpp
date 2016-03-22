/*
 * Configuration.cpp
 *
 *  Created on: Feb 5, 2016
 *      Author: ecejnj
 */

#include <iostream>

#include "Configuration.h"
#include <Util/Log.h>

#include <sstream>

namespace po = boost::program_options;

namespace ssh_cli {


const std::string Configuration::PWD_SUPPORT = "pwd_auth";
const std::string Configuration::KB_SUPPORT = "kb_auth";
const std::string Configuration::PWD = "password";
const std::string Configuration::HOST_STR = "host";
const std::string Configuration::PORT_STR = "port";
const std::string Configuration::USR_STR = "user";
const std::string Configuration::CMD_STR = "cmd";


Configuration::Configuration(int argc, const char *argv[]):
    argc( argc),
    argv( argv),
    pwd_support( false),
    kb_support( false){


}

Configuration::~Configuration() {
    // TODO Auto-generated destructor stub
}

template<typename T>
void
Configuration::parseOption(
        const std::string &option,
        T &param,
        const po::variables_map &vm){

    if (vm.count( option)) {
        param = vm[ option].as<T>();
        std::stringstream ss; ss << param;
        INFO( "%s read was set to '%s'\n", PWD_SUPPORT.c_str(),
                ss.str().c_str());

    }
}

bool
Configuration::parse_specific_option(){
    po::options_description desc;
    desc.add_options()
                             (PWD_SUPPORT.c_str(), po::value<bool>( ))
                             (KB_SUPPORT.c_str(), po::value<bool>( ))
                             (PWD.c_str(), po::value<std::string>( ))
                             (HOST_STR.c_str(), po::value<std::string>( ))
                             (PORT_STR.c_str(), po::value<std::string>( ))
                             (USR_STR.c_str(), po::value<std::string>( ))
                             (CMD_STR.c_str(), po::value<std::string>( ));

    po::variables_map vm;
    po::store(po::parse_config_file<char>( fileName.c_str(),
            desc, true), vm);
    po::notify(vm);

    parseOption<bool>( PWD_SUPPORT, pwd_support, vm);
    parseOption<bool>( KB_SUPPORT, kb_support, vm);
    parseOption<std::string>( PWD, password, vm);
    parseOption<std::string>( HOST_STR, host, vm);
    parseOption<std::string>( PORT_STR, port, vm);
    parseOption<std::string>( USR_STR, user, vm);
    parseOption<std::string>( CMD_STR, cmd, vm);

    return true;
}

bool
Configuration::parse_options(){
    bool rc;
    try {

        po::options_description desc("Allowed options");
        desc.add_options()
                        ("help,h", "produce help message")
                        ("file,f", po::value<std::string>(), "set input file")
                        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            // This is not really a failure but we need to exit
            return false;
        }

        if (vm.count("file")) {
            fileName = vm["file"].as<std::string>();
            INFO ("Input file dir was set to: '%s'\n ",
                    fileName.c_str());
        } else {
            std::cerr << "Usage\n" << desc << "\n";
            return false;
        }

        rc = parse_specific_option();

    }
    catch( std::exception& e) {
        ERROR( "%s Excecption error:%s", __PRETTY_FUNCTION__, e.what());
        return false;
    }
    catch(...) {
        ERROR( "%s Exception of unknown type", __PRETTY_FUNCTION__);
        return false;
    }
    return rc;


}

bool
Configuration::loadConfiguration(){

    if ( !parse_options()){
        return false;
    }


    return true;
}



} /* namespace ssh_cli */
