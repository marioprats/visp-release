/****************************************************************************
 *
 * $Id: vpServer.cpp 3820 2012-06-27 13:13:29Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2012 by INRIA. All rights reserved.
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional 
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 * 
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Description:
 * TCP Server
 *
 * Authors:
 * Aurelien Yol
 *
 *****************************************************************************/

#include <visp/vpServer.h>

/*!
  Construct a server on the machine launching it.
*/
vpServer::vpServer( ) : vpNetwork(), started(false)
{
  int protocol = 0;
  emitter.socketFileDescriptorEmitter = socket(AF_INET, SOCK_STREAM, protocol);
  if (emitter.socketFileDescriptorEmitter < 0)
  {
    vpERROR_TRACE( "vpServer::vpServer(), cannot open socket." );
  }
  emitter.emitterAdress.sin_family = AF_INET;
  emitter.emitterAdress.sin_addr.s_addr = INADDR_ANY;
  emitter.emitterAdress.sin_port = 0; 
  
  adress = inet_ntoa(emitter.emitterAdress.sin_addr);
  port = emitter.emitterAdress.sin_port;
}

/*!
  Construct a server on the machine launching it, with a specified port.
  
  \param port_serv : server's port.
*/
vpServer::vpServer( const int &port_serv ) : vpNetwork(), started(false)
{
  int protocol = 0;
  emitter.socketFileDescriptorEmitter = socket(AF_INET, SOCK_STREAM, protocol);
  if (emitter.socketFileDescriptorEmitter < 0)
  {
    vpERROR_TRACE( "vpServer::vpServer(const int &port_serv), cannot open socket." );
  }
  emitter.emitterAdress.sin_family = AF_INET;
  emitter.emitterAdress.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");;
  emitter.emitterAdress.sin_port = htons( port_serv ); 
  
  adress = inet_ntoa(emitter.emitterAdress.sin_addr);
  port = port_serv;
}

/*!
  Construct a server on the machine at a given adress, with a specified port.
  
  \param adress_serv : server's adress.
  \param port_serv : server's port.
*/
vpServer::vpServer( const std::string &adress_serv,const int &port_serv ) : vpNetwork(), started(false)
{
  int protocol = 0;
  emitter.socketFileDescriptorEmitter = socket(AF_INET, SOCK_STREAM, protocol);
  if (emitter.socketFileDescriptorEmitter < 0)
  {
    vpERROR_TRACE( "vpServer::vpServer(const std::string &adress_serv,const int &port_serv), cannot open socket." );
  }
  emitter.emitterAdress.sin_family = AF_INET;
  emitter.emitterAdress.sin_addr.s_addr = inet_addr(adress_serv.c_str());
  emitter.emitterAdress.sin_port = htons( port_serv );
  
  adress = adress_serv;
  port = port_serv;
}

/*!
  Shutdown the server. 
*/
vpServer::~vpServer()
{
#ifdef UNIX
  close( emitter.socketFileDescriptorEmitter );
#else //Win32
  closesocket( emitter.socketFileDescriptorEmitter );
#endif

  for(unsigned int i = 0 ; i < receptor_list.size() ; i++)
#ifdef UNIX
    close( receptor_list[i].socketFileDescriptorReceptor );
#else //Win32
    closesocket( receptor_list[i].socketFileDescriptorReceptor );
#endif
     
}

/*!
  Enable the server to wait for clients (on the limit of the maximum limit). 
  
  \return True if the server has started, false otherwise.
*/
bool vpServer::start()
{  
  int serverStructLength = sizeof(emitter.emitterAdress);
  int bindResult = bind( emitter.socketFileDescriptorEmitter, (struct sockaddr *) &emitter.emitterAdress, serverStructLength );
  
  if( bindResult < 0 )
  {
    std::cout << "Error id : " << bindResult << std::endl;
    std::string errorMessage( "vpServer::vpServer(), cannot bind to port"  );
    char posrtNumberString[32];
    sprintf( posrtNumberString, "%d", port );
    errorMessage += " ";
    errorMessage += posrtNumberString;
    errorMessage += " The port may be already used.";
    vpERROR_TRACE( errorMessage.c_str() );
    return false;
  }
  
#ifdef SO_NOSIGPIPE
  // Mac OS X does not have the MSG_NOSIGNAL flag. It does have this
  // connections based version, however.
  if (emitter.socketFileDescriptorEmitter > 0) {
    int set_option = 1;
    if (0 == setsockopt(emitter.socketFileDescriptorEmitter, SOL_SOCKET, SO_NOSIGPIPE, &set_option, sizeof(set_option))) {
    } else {
      std::cout << "Failed to set socket signal option" << std::endl;
    }
  }
#endif // SO_NOSIGPIPE

  listen( emitter.socketFileDescriptorEmitter, max_clients );
  std::cout << "Server ready" << std::endl;
  
  started = true;
  
  return true;
}

/*!
  Check if a client has connected or deconnected the server
  
  \return True if a client connected or deconnected, false otherwise OR server not started yet.
*/
bool vpServer::checkForConnections()
{
  if(!started)
    if(!start()){
      return false;
    }
  
  tv.tv_sec = tv_sec;
  tv.tv_usec = tv_usec;
  
  FD_ZERO(&readFileDescriptor);         
  
  socketMax = emitter.socketFileDescriptorEmitter;
  FD_SET(emitter.socketFileDescriptorEmitter,&readFileDescriptor);

  for(unsigned int i=0; i<receptor_list.size(); i++){
    FD_SET(receptor_list[i].socketFileDescriptorReceptor,&readFileDescriptor);

    if(i == 0)
      socketMax = receptor_list[i].socketFileDescriptorReceptor;
   
    if(socketMax < receptor_list[i].socketFileDescriptorReceptor) socketMax = receptor_list[i].socketFileDescriptorReceptor; 
  }
  
  int value = select(socketMax+1,&readFileDescriptor,NULL,NULL,&tv);
  if(value == -1){
    //vpERROR_TRACE( "vpServer::run(), select()" );
    return false;
  }
  else if(value == 0){
    return false;
  }
  else{
    if(FD_ISSET(emitter.socketFileDescriptorEmitter,&readFileDescriptor)){
      vpNetwork::vpReceptor client;
      client.receptorAddressSize = sizeof(client.receptorAddress);
      client.socketFileDescriptorReceptor = accept(emitter.socketFileDescriptorEmitter,(struct sockaddr*) &client.receptorAddress, &client.receptorAddressSize);
      if((client.socketFileDescriptorReceptor) == -1)
        vpERROR_TRACE( "vpServer::run(), accept()" );
      
      client.receptorIP = inet_ntoa(client.receptorAddress.sin_addr);
      printf("New client connected : %s\n", inet_ntoa(client.receptorAddress.sin_addr));
      receptor_list.push_back(client);
      
      return true;
    }
    else{
      for(unsigned int i=0; i<receptor_list.size(); i++){
        if(FD_ISSET(receptor_list[i].socketFileDescriptorReceptor,&readFileDescriptor)){
          char deco;
          int numbytes = recv(receptor_list[i].socketFileDescriptorReceptor, &deco, 1, MSG_PEEK);
      
          if(numbytes == 0)
          {
            std::cout << "Disconnected : " << inet_ntoa(receptor_list[i].receptorAddress.sin_addr) << std::endl;
            receptor_list.erase(receptor_list.begin()+i);
            return 0;
          }
        }
      }
    }
  }
  
  return false;
}

/*!
  Print the connected clients. 
*/
void vpServer::print()
{
  vpNetwork::print("Client");
}
