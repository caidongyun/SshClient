/*
 * Connection.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: ecejnj
 */

#include "Connection.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <Util/Log.h>

namespace ssh_cli {

Connection::Connection(
        const Configuration &conf):
    conf( conf),
    session( nullptr){


}

Connection::~Connection() {
    release();
}

int
Connection::verify_knownhost()
{
  int state, hlen;
  unsigned char *hash = NULL;
  char *hexa;
  char buf[10];
  state = ssh_is_server_known(session);
  hlen = ssh_get_pubkey_hash(session, &hash);
  if (hlen < 0)
    return -1;

  switch (state)
  {
    case SSH_SERVER_KNOWN_OK:
      fprintf( stdout, "Host key:\n");
      ssh_print_hexa("Public key hash", hash, hlen);
      break; /* ok */
    case SSH_SERVER_KNOWN_CHANGED:
      ERROR( "Host key for server changed: it is now:\n");
      ssh_print_hexa("Public key hash", hash, hlen);
      break;
    case SSH_SERVER_FOUND_OTHER:
      ERROR( "The host key for this server was not found but an other"
        "type of key exists.\n");
      ERROR( "An attacker might change the default server key to"
        "confuse your client into thinking the key does not exist\n");
      free(hash);
      return -1;
    case SSH_SERVER_FILE_NOT_FOUND:
      ERROR( "Could not find known host file.\n");
      ERROR( "If you accept the host key here, the file will be"
       "automatically created.\n");
      /* fallback to SSH_SERVER_NOT_KNOWN behavior */
    case SSH_SERVER_NOT_KNOWN:
      hexa = ssh_get_hexa(hash, hlen);
      ERROR("The server is unknown. Do you trust the host key?\n");
      ERROR( "Public key hash: %s\n", hexa);
      free(hexa);
      if (fgets(buf, sizeof(buf), stdin) == NULL)
      {
        free(hash);
        return -1;
      }
      if (strncasecmp(buf, "yes", 3) != 0)
      {
        free(hash);
        return -1;
      }
      if (ssh_write_knownhost(session) < 0)
      {
        ERROR( "Error %s\n", strerror(errno));
        free(hash);
        return -1;
      }
      break;
    case SSH_SERVER_ERROR:
      ERROR( "Error %s", ssh_get_error(session));
      free(hash);
      return -1;
  }
  free(hash);
  return 0;
}


int
Connection::authenticate_kbdint( )
{
  int rc;
  rc = ssh_userauth_kbdint(session, NULL, NULL);

  while (rc == SSH_AUTH_INFO)
  {
    const char *name, *instruction;
    int nprompts, iprompt;
    name = ssh_userauth_kbdint_getname(session);
    instruction = ssh_userauth_kbdint_getinstruction(session);
    nprompts = ssh_userauth_kbdint_getnprompts(session);
    if (strlen(name) > 0)
      printf("%s\n", name);
    if (strlen(instruction) > 0)
      printf("%s\n", instruction);
    for (iprompt = 0; iprompt < nprompts; iprompt++)
    {
      const char *prompt;
      char echo;
      prompt = ssh_userauth_kbdint_getprompt(session, iprompt, &echo);
      DEBUG( "prompt '%s' echo %d \n", prompt, echo);
      if (echo)
      {
        char buffer[128], *ptr;
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
          return SSH_AUTH_ERROR;
        buffer[sizeof(buffer) - 1] = '\0';
        if ((ptr = strchr(buffer, '\n')) != NULL)
          *ptr = '\0';
        if (ssh_userauth_kbdint_setanswer(session, iprompt, buffer) < 0)
          return SSH_AUTH_ERROR;
        memset(buffer, 0, strlen(buffer));
      }
      else
      {
        char *ptr;
        ptr = getpass(prompt);
        if (ssh_userauth_kbdint_setanswer(session, iprompt, ptr) < 0)
          return SSH_AUTH_ERROR;
      }
    }
    rc = ssh_userauth_kbdint(session, NULL, NULL);
  }
  return rc;
}


int
Connection::authenticate_kbdint_pwd( )
{
  int rc;
  rc = ssh_userauth_kbdint(session, NULL, NULL);
  while (rc == SSH_AUTH_INFO)
  {
    const char *name, *instruction;
    int nprompts, iprompt;
    name = ssh_userauth_kbdint_getname(session);
    instruction = ssh_userauth_kbdint_getinstruction(session);
    nprompts = ssh_userauth_kbdint_getnprompts(session);
    if (strlen(name) > 0)
      printf("%s\n", name);
    if (strlen(instruction) > 0)
      printf("%s\n", instruction);
    for (iprompt = 0; iprompt < nprompts; iprompt++)
    {

      // TODO: check asking for pwd
      if (ssh_userauth_kbdint_setanswer(session, iprompt,
                                          conf.getPassword().c_str()) < 0){
          return SSH_AUTH_ERROR;
      }
    }
    rc = ssh_userauth_kbdint(session, NULL, NULL);
  }
  return rc;
}



int
Connection::authenticate_passwd( ){

    int rc;
    // Authenticate ourselves
    std::string pwd = conf.getPassword();

    if ( pwd.empty()){
        pwd = getpass("Password: ");
    }
    rc = ssh_userauth_password( session, NULL, pwd.c_str());
    if (rc != SSH_AUTH_SUCCESS)
    {
        ERROR( "Error authenticating with password: %s\n",
                ssh_get_error( session));

    }

    return rc;
}



int
Connection::authenticate( ){

    int rc = SSH_AUTH_ERROR;

    ssh_userauth_none(session, NULL); // withouot this ssh_userauth_list return 0
    int method = ssh_userauth_list(session, NULL);

    DEBUG( "Method: 0x%x Pwd:0x%x Kb:0x%x \n", method,
            SSH_AUTH_METHOD_PASSWORD,
            SSH_AUTH_METHOD_INTERACTIVE);

    if ( ( method & SSH_AUTH_METHOD_PASSWORD ) &&
         ( conf.isPwdSupport()) ){
        DEBUG( "Authenticating with password");
        rc = authenticate_passwd( );

    } else
    if ( ( method & SSH_AUTH_METHOD_INTERACTIVE ) &&
          conf.isKbSupport()  ){
        DEBUG( "Authenticating with kb interactive");
        if ( conf.getPassword().empty()){
            rc = authenticate_kbdint( );
        } else {
            rc = authenticate_kbdint_pwd( );
        }
    }


    return rc;
}


bool
Connection::connect(){

    int rc;

    // Open session and set options
    session = ssh_new();
    if ( session == NULL)
        return false;

    ssh_options_set( session, SSH_OPTIONS_HOST, conf.getHost().c_str());
    ssh_options_set( session, SSH_OPTIONS_PORT_STR, conf.getPort().c_str());
    ssh_options_set( session, SSH_OPTIONS_USER, conf.getUser().c_str());

    // Connect to server
    rc = ssh_connect( session);
    if (rc != SSH_OK)
    {
        ERROR( "Error connecting to '%s': %s\n",
                conf.getHost().c_str(),
                ssh_get_error( session));
        release( );
        return false;
    }

    connected = true;

    // Verify the server's identity
    // For the source code of verify_knowhost(), check previous example
    if (verify_knownhost() < 0)
    {
        release();
        return false;
    }

    // Authenticate ourselves
    rc = authenticate( );

    if (rc != SSH_AUTH_SUCCESS)
    {
        ERROR( "Error authenticating: %s\n",
                ssh_get_error( session));
        release();
        return false;
    }

    return connected;
}

std::string
Connection::execute( const std::string &cmd) const{


    std::string output;

    if ( !connected){
        ERROR( "Connection not established");
        return output;
    }

    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;

    channel = ssh_channel_new( session);
    if (channel == NULL)
        return output;

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return output;
    }

    INFO( "Executing '%s'\n", cmd.c_str());
    rc = ssh_channel_request_exec(channel, cmd.c_str());

    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return output;
    }

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0)
    {
        output += std::string( buffer, nbytes);

        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    if (nbytes < 0)
    {
        ERROR( "Incomplete cmd");
        // Return what was retrieved so far
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return output;
}




} /* namespace ssh_cli */
