/**
 * @file simulatorServer.h
 *
 * @author Laurent Saint-Marcel
 *
 * Interface basee sur le socket pour se connecter sur le simulateur
 */

#ifndef __SIMULATOR_SERVER_H__
#define __SIMULATOR_SERVER_H__

#include "network.h"
#include "network.h"

class SimulatorRobot;
class SimulateurServer;
class SimulatorCOnnection;

/**
 * @class SimulatorConnection
 * Gere une connection au SimulatorServer
 */
class SimulatorConnection {
  public:
    SimulatorConnection(Socket& socket);
    virtual ~SimulatorConnection();

    SimulatorRobot* getRobot() { return robot_; }
    void disconnect();
 
    void parseReceivedData(SimuRequestType type, 
			   unsigned char* buffer);
    unsigned char* setBufferHeader(SimuRequestType type, 
                                   int dataLength);
    void           sendBuffer();
    unsigned char* recvBuffer(SimuRequestType& type);

 private:
    SimulatorRobot * robot_;
    Socket           socket_;
    unsigned char    buffer_[SIMU_BUFFER_LENGTH];


};

/**
 * @class SimulatorServer
 * Gere un systeme de connection au simulateur
 */
class SimulatorServer {
 public:
    SimulatorServer();
    virtual ~SimulatorServer(); 

    void            startReceiver();
    SimulatorRobot* getRobot(int connectionId);

 protected:
    static void registerConnection(Socket& socket);
    static void startConnectionThread(SimulatorConnection* connection);
    static void unregisterConnection(SimulatorConnection* connection);

#ifdef USE_FTHREAD
    friend void connectionThread(void* connection);
#else
    friend void* connectionThread(void* connection);
#endif
    friend void simulatorReceiveAnalyzer(Socket& socket);

 protected:
    static SimulatorConnection* connections_[SIMU_PORT_MAX_CONNECTIONS];
    
 
};

#endif // __SIMULATOR_SERVER_H__
