/**
 * @file logger.cpp
 *
 * @author Laurent Saint-Marcel
 *
 * Programme qui recoit des donnees de log sur une socket et qui les 
 * enregistre sur le disque en les compressant
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "robotFile.h"		/* pour les fichiers zippe */
#include <time.h>
#include <dirent.h>

#define LOG_SOCKET_INFO
#define LOG_DIRECTORY_INFO
#include "log.h"
#include "classConfig.h"

/* data for interrupt */
int sock_=0;
File* file_=NULL;

static const int LOG_LIMIT_DEFAULT=40;

char logDirectory_[255];
int  logLimit_=LOG_LIMIT_DEFAULT;
static const char* logBaseName_="robot_";
static const char* logDateName_="robotLog_";

// --------------------------------------------------------------------
// testDirent
// --------------------------------------------------------------------
int testDirent(const struct dirent* p) 
{  
  if ( strstr(p->d_name,"robot_") == NULL ) {
    return 0;
  }
  return 1;
}

// --------------------------------------------------------------------
// Cree un fichier qui donne la correpondance fichier de log, date
// --------------------------------------------------------------------
void updateCorrepondanceFile(int &num)
{
    num=0;
    bool numFound=false;
    FILE* configFile=NULL;
    FILE* f2=NULL;
    char configName[255];
    int i=-1;
    char srcName[LOG_LIMIT_DEFAULT][255];
    char destName[LOG_LIMIT_DEFAULT][255];
    sprintf(configName, "%s/date.txt", logDirectory_);
    configFile = fopen(configName, "r");
    if (configFile != NULL) {
	if (fscanf(configFile, "Last=%d\n", &num) == 1) {
	    num=(num+1)%logLimit_;
	    for(i=0;i<=logLimit_; i++) {
		if (fscanf(configFile, "%s %s\n", srcName[i], destName[i]) != 2)
		    break;
		char filename[255];
		sprintf(filename, "%s/%s", logDirectory_, srcName[i]);
		if ((f2=fopen(filename,"r"))!=NULL) {
		    fclose(f2);
		    int numI=-1;
		    if (sscanf(srcName[i], "robot_%d.log.gz", &numI) == 1) {
			if (numI == num) { 
			    numFound=true;
			    printf("%d found:%s found\n", num, srcName[i]);
			}
		    }
		} else {
		    printf("%s not found\n", srcName[i]);
		    i--;
		}
	    }
        }
        fclose(configFile);
    }
    			
    time_t tloc;
    time(&tloc);
    struct tm *ltime=localtime(&tloc);
    sprintf(srcName[num],"%s%02d.log.gz", logBaseName_, num);
    sprintf(destName[num], "%s%02d%02d%02d_%02dh%02dm%02ds.gz", 
            logDateName_,
            ltime->tm_year%100,
            ltime->tm_mon+1,
            ltime->tm_mday,
            ltime->tm_hour,
            ltime->tm_min,
            ltime->tm_sec);
    
    configFile = fopen(configName, "w");
    if (configFile != NULL) {
	fprintf(configFile, "Last=%d\n", num);
        for(int j=0;j<i; j++) {
            fprintf(configFile, "%s %s\n", srcName[j], destName[j]);
        }
	if (!numFound) {
	    fprintf(configFile, "%s %s\n", srcName[num], destName[num]);
	}
        fclose(configFile);
    }
}

// --------------------------------------------------------------------
// ouvre un fichier gzip pour archiver
// --------------------------------------------------------------------
int initFile(File* file) 
{
  char archiveName[255];
  int num_last=0;

  updateCorrepondanceFile(num_last);
  sprintf(archiveName,"%s/%s%02d.log", 
	  logDirectory_, 
	  logBaseName_, 
	  num_last);

  ::printf("Create new log file --> %s.gz\n  mv %s.gz ../log\n", 
	   archiveName, archiveName);
  if (file) file->open(archiveName, FILE_MODE_WRITE);
  
  return EXIT_SUCCESS;
}


// --------------------------------------------------------------------
// traitementInterrupt
// --------------------------------------------------------------------
void traitementInterrupt(int signum) 
{
  switch (signum) {
  case SIGINT:
    printf("SIGINT Closing socket and file\n");	
    close(sock_);
    if (file_) file_->close();
    unlink(LOG_SOCKET_PATH);
    exit(EXIT_SUCCESS);		
    break;
    
  case SIGSEGV:	
    printf("Aargh !! Segmentation Fault\n");
    close(sock_);
    shutdown(sock_, SHUT_RDWR);
    if (file_) file_->close();
    unlink(LOG_SOCKET_PATH);
    exit(EXIT_SUCCESS);		
    break;
    
  case SIGTERM:	
    printf("SIGTERM Closing socket and file\n");		
    close(sock_);
    if (file_) file_->close();
    unlink(LOG_SOCKET_PATH);
    exit(EXIT_SUCCESS);
    break;
  }
}

// --------------------------------------------------------------------
// initSocket
// --------------------------------------------------------------------
int initSocket(int &sock) 
{
  /* Create socket from which to read. */
#ifdef LOG_DATAGRAM
  sock = socket(AF_UNIX, SOCK_DGRAM, 0);
#else
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
#endif
  if (sock < 0) {
    perror("opening datagram socket\n");
    exit(1);
  }
  /* Create name. */
  struct sockaddr_un name;
  name.sun_family = AF_UNIX;
  strcpy(name.sun_path, LOG_SOCKET_PATH);
  
  /* Bind the UNIX domain address to the created socket */
    
  if (bind(sock, (struct sockaddr *) &name, sizeof(struct sockaddr_un))) {
    perror("binding name to datagram socket\n");
    exit(1);
  }
  printf("Socket --> %s\n", LOG_SOCKET_PATH);
  return(EXIT_SUCCESS);
}

// --------------------------------------------------------------------
// displayPacket
// --------------------------------------------------------------------
void displayPacket(LogPacketHeader const& packetHeader, 
                   Byte* data) 
{
  char head[30];
  LogPacketMessage* msg = (LogPacketMessage*)data;	
  switch (packetHeader.type) {
  case LOG_LEVEL_ERROR:
    sprintf(head,"%sError:%s",KB_ERROR,KB_RESTORE);
    ::printf("[%15.15s:%4d] %s %s", 
             (strlen(msg->file)==0) ? "nofile":msg->file, 
             msg->line, head, msg->mesg);
    break;
  case LOG_LEVEL_WARNING:
    sprintf(head,"%sWarning:%s",KB_WARNING,KB_RESTORE);
    ::printf("[%15.15s:%4d] %s %s", 
             (strlen(msg->file)==0) ? "nofile":msg->file, 
             msg->line, head, msg->mesg);
    break;
  case LOG_LEVEL_INFO:
    sprintf(head,"%sInfo:%s",KB_INFO,KB_RESTORE);
    ::printf("[%15.15s:%4d] %s %s", 
             (strlen(msg->file)==0) ? "nofile":msg->file, 
             msg->line, head, msg->mesg);
    break;
  case LOG_LEVEL_OK:
    sprintf(head,"%sOk:%s",KB_OK,KB_RESTORE);
    ::printf("[%15.15s:%4d] %s %s", 
             (strlen(msg->file)==0) ? "nofile":msg->file, 
             msg->line, head, msg->mesg);
    break;
  case LOG_LEVEL_FUNCTION:
    sprintf(head,"%sFunction:%s",KB_FUNCTION,KB_RESTORE);
    ::printf("[%15.15s:%4d] %s %s\n", 
             (strlen(msg->file)==0) ? "nofile":msg->file, 
             msg->line, head, msg->mesg);
    break;
  case LOG_TYPE_START_MATCH:
    ::printf("%s==== The match is starting ====%s\n",KB_OK,KB_RESTORE);
    break;
  case LOG_TYPE_START_STRATEGY:
    ::printf("%s==== A strategy is selected ====%s\n",KB_OK,KB_RESTORE);
    break;
  default:
    ::printf("Unknown Type:%d\n", packetHeader.type);
    break;		
  }
}

// --------------------------------------------------------------------
// storePacket
// --------------------------------------------------------------------
bool storePacket(File* file,
                 LogPacketHeader const& packetHeader, 
                 Byte* data)
{
    if (file) {
        if (file->write((Byte*)(&packetHeader), 
                        sizeof(LogPacketHeader)) != sizeof(LogPacketHeader))
            return false;
        if (file->write(data, packetHeader.length) != packetHeader.length)
            return false;
    }
    return true;
}

// --------------------------------------------------------------------
// restorePacket
// --------------------------------------------------------------------
bool restorePacket(File* file,
                   LogPacketHeader& packetHeader, 
                   Byte* data)
{
    if (file) {
        if (file->read((Byte*)(&packetHeader), 
                   sizeof(LogPacketHeader)) != sizeof(LogPacketHeader))
            return false;
        if (file->read(data, packetHeader.length) != packetHeader.length)
            return false;
    }
    return true;
}	

// --------------------------------------------------------------------
// peekFromSocket
// --------------------------------------------------------------------
int peekFromSocket(int sock, void* data, int size) 
{
    int nbRead=0;
    int nbTotal=0;
    do {
        nbRead = recv(sock, &(((unsigned char*)data)[nbTotal]), 
                      size-nbTotal, MSG_PEEK);
        nbTotal+=nbRead;
        if (nbRead==0) {
            return -2;
        }
    } while ((nbTotal != size) && (nbRead > 0));


    if (nbTotal != size) {
        return -1;
    } else {
        return 0;
    }
}

// --------------------------------------------------------------------
// readFromSocket
// --------------------------------------------------------------------
int readFromSocket(int sock, void* data, int size) 
{
    int nbRead=0;
    int nbTotal=0;
    if (size<=0) return 0;
    do {
        nbRead = recv(sock, &(((unsigned char*)data)[nbTotal]), 
                      size-nbTotal, 0);
        nbTotal+=nbRead;
	if (nbRead==0) {
            return -2;
        }
    } while ((nbTotal != size) && (nbRead > 0));
    if (nbTotal != size) {
        return -1;
    } else {
        return 0;
    }
}

// --------------------------------------------------------------------
// processPacket
// --------------------------------------------------------------------
bool processPacket(int sock, 
                   File* outFile, 
                   bool display,
		   bool &close) 
{
    LogPacketHeader packetHeader((LogType)0);        
    struct tm *timeInfo=NULL;
    bool magicOK = false;
    int magic=0;
    static Byte data[LOG_DATA_MAX_LENGTH];
    close=false;
    // look for magic
    while (!magicOK) {
        Byte byte=0;
	while(1) {
	  if (peekFromSocket(sock, &byte, 1)==0) {
	    if (byte==0xa0) break;
	    else readFromSocket(sock, &byte, 1);
	  }
	}
        if (peekFromSocket(sock, &magic, sizeof(int))==0) {
            if (magic != LOG_MAGIC_NBR) {
                printf ("*** Packet corrupted try to resynchronize (magic "
                        ": 0x%x != 0x%x) !!!\n", magic, LOG_MAGIC_NBR);
                readFromSocket(sock, &magic, sizeof(int));
            } else {
                break;
            }
        } else {
            return false;
        }
    }
    if (readFromSocket(sock, &packetHeader, sizeof(LogPacketHeader))==0) {
        if (display) {
            printf("    timeStamp = ");
            timeInfo = localtime(&packetHeader.timeStamp.tv_sec);
            printf("%02d/%02d/%02d %02d:%02d:%02d,%06d\n",
                   timeInfo->tm_mday,
                   timeInfo->tm_mon+1,
                   timeInfo->tm_year+1900,
                   timeInfo->tm_hour,
                   timeInfo->tm_min,
                   timeInfo->tm_sec,
                   (int)packetHeader.timeStamp.tv_usec);
        }
	if (packetHeader.type == LOG_TYPE_CLOSE) {
	    printf("Closing log\n");
	    close=true;
	    return false;
	}
	//printf("read length=%d\n", packetHeader.length);
        if (readFromSocket(sock, data, packetHeader.length)==0) {
            if (display) {
	        displayPacket(packetHeader, data);
	    } 
        }
        storePacket(outFile, packetHeader, data); 
        return true;
    } else {
      printf("read error\n");
        return false;
    }
}

// --------------------------------------------------------------------
// parseArgs
// --------------------------------------------------------------------
void parseArgs(int argc, char*argv[])
{
    strcpy(logDirectory_, LOG_DIRECTORY_NAME);
    for(int i=1; i<argc; i++) {
        if (strcmp(argv[i], "-l")==0 && ++i<argc) logLimit_=atoi(argv[i]);
        else strcpy(logDirectory_, argv[i]);
    }
    if (logLimit_<3) 
        logLimit_ = 0;
    else if (logLimit_> LOG_LIMIT_DEFAULT) 
        logLimit_ =  LOG_LIMIT_DEFAULT;
    printf("Usage: %s [logDirectory=%s] [-l logLimit(%d)]\n"
           "  logDirectory: repertoire ou sont stockés les logs\n"
           "  logLimit: nombre maximum de fichier logs dans logDirectory. "
           "Si la limite est atteinte, le plus vieux log est efface\n",
           argv[0], logDirectory_, logLimit_);
}

// --------------------------------------------------------------------
// main
// --------------------------------------------------------------------
int main(int argc, char*argv[]) 
{    
    parseArgs(argc, argv);

    struct sigaction a;
    a.sa_handler = traitementInterrupt;      /* fonction à lancer */
    sigemptyset(&a.sa_mask);    /* rien à masquer */
    
    sigaction(SIGTSTP, &a, NULL);       /* pause contrôle-Z */
    sigaction(SIGINT,  &a, NULL);       /* fin contrôle-C */
    sigaction(SIGTERM, &a, NULL);       /* arrêt */
    sigaction(SIGSEGV, &a, NULL);       /* segmentation fault ! */

    int sock=0;
    initSocket(sock);
    sock_=sock;
    
    ZFile file;
    initFile(&file);
    file_=&file;
    
    while(1) {
	bool close=false;
        processPacket(sock, &file, false, close);
	if (close) {
	    file.close();
	    initFile(&file);
	    file_=&file;
	}
    }
    return(EXIT_SUCCESS);
}
