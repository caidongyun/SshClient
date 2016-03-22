//============================================================================
// Name        : SshClient.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


/*
#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <string.h>

#include <Config/Configuration.h>
#include <Util/Log.h>
*/
#include <Client.h>

/*
int verify_knownhost(ssh_session session)
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
      break;
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
      // fallback to SSH_SERVER_NOT_KNOWN behavior
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


int exec_remote_cmd(ssh_session session, const char *cmd)
{
  ssh_channel channel;
  int rc;
  char buffer[256];
  int nbytes;
  channel = ssh_channel_new(session);
  if (channel == NULL)
    return SSH_ERROR;
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK)
  {
    ssh_channel_free(channel);
    return rc;
  }

  INFO( "Executing '%s'\n", cmd);
  rc = ssh_channel_request_exec(channel, cmd);
  if (rc != SSH_OK)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return rc;
  }
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  while (nbytes > 0)
  {
    // writing to stdout
    if (write(1, buffer, nbytes) != (unsigned int) nbytes)
    {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }

  if (nbytes < 0)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_ERROR;
  }
  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
  return SSH_OK;
}


int authenticate_kbdint(ssh_session session)
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


int authenticate_kbdint_pwd(ssh_session session,  const ssh_cli::Configuration &conf)
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
      //const char *prompt;
      //char echo;
      //prompt = ssh_userauth_kbdint_getprompt(session, iprompt, &echo);

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



int authenticate_passwd(ssh_session session,  const ssh_cli::Configuration &conf){
    //char *password;
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



int authenticate( ssh_session session,
                  const ssh_cli::Configuration &conf){

    int rc = SSH_AUTH_ERROR;

    ssh_userauth_none(session, NULL); // withouot this ssh_userauth_list return 0
    int method = ssh_userauth_list(session, NULL);

    DEBUG( "Method: 0x%x Pwd:0x%x Kb:0x%x \n", method,
            SSH_AUTH_METHOD_PASSWORD,
            SSH_AUTH_METHOD_INTERACTIVE);

    if ( ( method & SSH_AUTH_METHOD_PASSWORD ) &&
         ( conf.isPwdSupport()) ){
        DEBUG( "Authenticating with password");
        rc = authenticate_passwd( session, conf);

    } else
    if ( ( method & SSH_AUTH_METHOD_INTERACTIVE ) &&
          conf.isKbSupport()  ){
        DEBUG( "Authenticating with kb interactive");
        if ( conf.getPassword().empty()){
            rc = authenticate_kbdint( session);
        } else {
            rc = authenticate_kbdint_pwd( session, conf);
        }
    }


    return rc;
}

int execute( const ssh_cli::Configuration &conf){
    ssh_session my_ssh_session;
    int rc;

    // Open session and set options
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
        exit(-1);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, conf.getHost().c_str());
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT_STR, conf.getPort().c_str());
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, conf.getUser().c_str());
    // Connect to server
    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
        ERROR( "Error connecting to '%s': %s\n",
                conf.getHost().c_str(),
                ssh_get_error(my_ssh_session));
        ssh_free(my_ssh_session);
        exit(-1);
    }
    // Verify the server's identity
    // For the source code of verify_knowhost(), check previous example
    if (verify_knownhost(my_ssh_session) < 0)
    {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        exit(-1);
    }

    // Authenticate ourselves
    rc = authenticate( my_ssh_session, conf);

    if (rc != SSH_AUTH_SUCCESS)
    {
        ERROR( "Error authenticating: %s\n",
                ssh_get_error(my_ssh_session));
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        exit(-1);
    }

    exec_remote_cmd( my_ssh_session, conf.getCmd().c_str());

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);

    return 0;
}
*/


int main(int argc, const char *argv[])
{

    /*int rc = 0;
    ssh_cli::Configuration config( argc, argv);

    if ( config.loadConfiguration()){
        rc = execute( config);

    } else {
        rc = 1;
    }

    return rc;
    */
    ssh_cli::Client client;
    return client.execute( argc, argv);



}
