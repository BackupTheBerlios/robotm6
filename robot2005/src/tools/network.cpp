/*
 * @file : network.cpp
 *
 * Description :
 * Public interface for receiving telecommand orders throught a socket
 * This is the robot part
 *
 * @date    01/11/02
 * @author  Laurent Saint-Marcel
 */

#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close */

#include <stdio.h>
#include <unistd.h> // usleep
#include <assert.h>

#include "log.h"

#include "network.h"

// ----------------------------------------------------------------------------
// Socket::Socket
// ----------------------------------------------------------------------------
Socket::Socket(int socketId) :
    RobotBase("Socket", CLASS_SOCKET), socketID_(socketId)
{

}

// ----------------------------------------------------------------------------
// Socket::~Socket
// ----------------------------------------------------------------------------
Socket::~Socket()
{

}

// ----------------------------------------------------------------------------
// Socket::validate
// ----------------------------------------------------------------------------
bool Socket::validate()
{
    return false;
}

// ----------------------------------------------------------------------------
// Socket::openReceiver
// ----------------------------------------------------------------------------
bool Socket::openReceiver(const int port, 
                          const int maxConnexions)
{
  struct sockaddr_in servAddr;

  /* create socket */
  socketID_ = socket(AF_INET, SOCK_STREAM, 0);
  if(socketID_<0) {
     LOG_ERROR("Cannot open socket\n");
     return false;
  }
  
  /* bind server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(port);
  
  if (bind(socketID_, 
	   (struct sockaddr *) &servAddr, 
	   sizeof(servAddr))<0) {
    /* if can't bind this port, try another one */
    LOG_ERROR("Cannot bind port %d\n", port);
    close();
    return false;
  }

  /* start listening */
  listen(socketID_, maxConnexions);
  LOG_INFO("Socket %d listening on port %d\n", socketID_, port);
  return true;
}

// ----------------------------------------------------------------------------
// Socket::close
// ----------------------------------------------------------------------------
bool Socket::close()
{
    if (socketID_>=0) {
        ::close(socketID_);
        LOG_INFO("Connection closed on socket %d\n", socketID_);
        socketID_ = -1;
    }
    return true;
}

// ----------------------------------------------------------------------------
// Socket::receiveData
// ----------------------------------------------------------------------------
bool Socket::receiveData(ReceiveAnalyzer userReceptor)
{
  int newSd=0;
  socklen_t cliLen;
  struct sockaddr_in cliAddr;

  while(1) {
    LOG_INFO("Socket %d is waiting for connection\n", socketID_);
    cliLen = sizeof(cliAddr);
    newSd = accept(socketID_, (struct sockaddr *) &cliAddr, &cliLen);
    if(newSd<0) {
      LOG_ERROR("Cannot accept connection\n");
      return false;
    }
    LOG_INFO("Accept connection on socket%d\n", newSd);
    if (userReceptor) {
        Socket newSock(newSd);
        userReceptor(newSock);
    }
  } /* while (1) */
  return true;
}

// ----------------------------------------------------------------------------
// Socket::read
// ----------------------------------------------------------------------------
int Socket::read(unsigned char *buf, 
                 const int length) 
{
  int N=length;
  int n=0;
  int i=0;
  LOG_DEBUG("Wait %d bytes on socket %d\n", length, socketID_);
  memset(buf,0x0,N); /* init buffer */
  while(N>0) {
      /* read data from socket */
      usleep(20);
      n = recv(socketID_, buf+i, N, 0); /* wait for data */
      if (n<0) {
	LOG_ERROR("Cannot receive data\n");
	close();
	return i;
      } else if (n==0) {
	LOG_INFO("Connection closed by client\n");
	close();
	return i;
      } 
      LOG_DEBUG("Received %d bytes\n", n);
      N-=n;
      i+=n; 
  }
  return i;
}

// ----------------------------------------------------------------------------
// Socket::openSender
// ----------------------------------------------------------------------------
bool Socket::openSender(const int port,
                        const char* hostname)
{
  int rc;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h=NULL;
  
  h = gethostbyname(hostname);
  if(h==NULL) {
    LOG_ERROR("Unknown host '%s'\n", hostname);
    return false;
  }

  servAddr.sin_family = h->h_addrtype;
  memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons(port);

  /* create socket */
  socketID_ = socket(AF_INET, SOCK_STREAM, 0);
  if(socketID_<0) {
    LOG_ERROR("Cannot open socket\n");
    return false;
  }

  /* bind any port number */
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);
  
  rc = bind(socketID_, (struct sockaddr *) &localAddr, sizeof(localAddr));
  if(rc<0) {
    LOG_ERROR("Cannot bind port TCP %u\n",port);
    close();
    return false;
  }
				
  /* connect to server */
  rc = connect(socketID_, (struct sockaddr *) &servAddr, sizeof(servAddr));
  if(rc<0) {
    LOG_ERROR("Cannot connect \n");
    close();
    return false;
  }

  LOG_OK("Connected to %s on port %d, socket=%d\n", hostname, port, socketID_);
  /* return socket ID */
  return true;
}

// ----------------------------------------------------------------------------
// Socket::write
// ----------------------------------------------------------------------------
int Socket::write(unsigned char* buf, 
                  const int length)
{
  if (send(socketID_, buf, length, 0)<0) {
    LOG_ERROR("Cannot send data\n");
    return -1;
  }
  //fflush((FILE*)socketID);
  return length;
}
