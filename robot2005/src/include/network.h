/**
 * @file : network.h
 *
 * @brief Standard functions to open read write on socket
 *
 * Description :
 * High level interface for the network communication
 *
 * @date    02/12/02
 * @author  Laurent Saint-Marcel
 */

#ifndef __ROBOT_NETWORK_H__
#define __ROBOT_NETWORK_H__

#include "robotBase.h"

class Socket;

/** Function that take a socketID as parameter */
typedef void(*ReceiveAnalyzer)(Socket&);

/**
 * @class Socket
 * @brief Gestion d'une connection reseau
 */
class Socket : public RobotBase {
 public :
     Socket(int socketId=-1);
     virtual ~Socket();
     bool validate();
/**
 * @brief This function opens a socket for receiving connections on port
 *        'port'
 * 
 * @return the socketID on success or <0 on error
 */
     bool openReceiver(const int port, 
                       const int maxConnexions=1);
/**
 * @brief This function opens a socket for sending data to hostname/port
 * 
 * @return the socketID on success or <0 on error
 */
     bool openSender(const int port, 
                     const char* hostname);
/**
 * @brief This function closes a connection
 */ 
     bool close();

/**
 * @brief This function waits for connections and run userReceiver for 
 *        each one
 * 
 * socket_openReceiver must have been run first. userReceiver should use 
 * readBytes to read data
 */ 
     bool receiveData(ReceiveAnalyzer userReceiver);

/**
 * @brief This function receive length bytes from the socket. It returns 
 *        only if it received length bytes or the connection was closed. 
 * 
 * @return <0 on error, length on success
 */
     int read(unsigned char *buf, 
	      const int length);

/**
 * @brief This function receive length bytes from the socket. It returns 
 *        only if it received length bytes or the connection was closed. 
 * 
 * @return <0 on error, length on success
 */
     int write(unsigned char *buf, 
	       const int length);

     bool isOpened() { return socketID_>=0; }

 private:
     int socketID_;
};

#endif
