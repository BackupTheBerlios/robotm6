/**
 * @file log2html.cpp
 *
 * @author Laurent Saint-Marcel
 *
 * Outil de conversion d'un fichier de log en plusieurs fichier html
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <time.h>

#define SOUND_INFO
#define GONIO_INFO
#include "log.h"
#include "classConfig.h"
#include "robotFile.h"
#include "soundList.h"

static const int LOG_LINK_NBR = 5;
static int  linkNbr_=0;
static char link_    [LOG_LINK_NBR][255];
static char linkName_[LOG_LINK_NBR][255];

static int lineCounter_=0;
static bool filter_[LOG_TYPE_NBR];
static bool filterEnvDetector=true;

static const char* COLOR_COMMAND  = "FFFF00";
static const char* COLOR_ERROR    = "FF0000";
static const char* COLOR_WARNING  = "FFAA00";
static const char* COLOR_OK       = "00FF00";
static const char* COLOR_FUNCTION = "CC00FF";
static const char* COLOR_INFO     = "55AAFF";
static const char* COLOR_START    = "FFFF00";
static const char* COLOR_HLI      = "CCFFFF";
static const char* COLOR_LCD      = "0000FF";
static const char* COLOR_PERIODIC = "FF00FF";

// ---------------------------------------------------------------------------
// operator-(const timeval &t1 , const timeval &t2)
// ---------------------------------------------------------------------------
timeval operator-(const timeval &t1 , const timeval &t2)
{
  timeval t3;
  t3.tv_sec = t1.tv_sec  - t2.tv_sec;
  if (t1.tv_usec < t2.tv_usec) {
    t3.tv_usec= 1000000-t2.tv_usec+t1.tv_usec;
    t3.tv_sec--;
  } else {
    t3.tv_usec= t1.tv_usec - t2.tv_usec;
  }
  return t3;
}



// ---------------------------------------------------------------------------
// restorePacket
// ---------------------------------------------------------------------------
// Lire les donnees d'un fichier de log et les transformer en un packet 
// ---------------------------------------------------------------------------
bool restorePacket(File* file,
                   LogPacketHeader& packetHeader, 
                   Byte* data)
{
    if (file) {
        if (file->read((Byte*)(&packetHeader), 
		       sizeof(LogPacketHeader)) != sizeof(LogPacketHeader)) {
            return false;
	}
	assert(packetHeader.length<(int)LOG_DATA_MAX_LENGTH);
	//printf("%d %d\n", (int)packetHeader.type, packetHeader.length);
        if (file->read(data, packetHeader.length) != packetHeader.length) {
            return false;
	}
    }
 
    return true;
}

// ---------------------------------------------------------------------------
// initFilter
// ---------------------------------------------------------------------------
void initFilter()
{
    for(int i=0;i<LOG_TYPE_NBR;i++) {
        filter_[i] = false;
    }
    filter_[LOG_TYPE_SOUND]     = true;
    filter_[LOG_TYPE_POSITION]  = true;
    filter_[LOG_TYPE_MOTOR]     = true;
    filter_[LOG_TYPE_PING]      = true;

    filterEnvDetector=true;
}

// ---------------------------------------------------------------------------
// htmlLinks
// ---------------------------------------------------------------------------
void htmlLinks(FILE*       htmlFile)
{
    for(int i=0;i<linkNbr_;i++) {
        fprintf(htmlFile, 
                "  <A HREF=\"%s\">%s</A> \n", 
                link_[i], linkName_[i]);
    }
}

// ---------------------------------------------------------------------------
// htmlHeader
// ---------------------------------------------------------------------------
// Met a jour un fichier avec l'entete necessaire pour un fichier html 
// ---------------------------------------------------------------------------
void htmlHeader(FILE*       htmlFile,
                const char* title,
                const char* titleColor,
                const char* bgColor)
{
    fprintf(htmlFile, 
            "<HTML>\n"
            "<HEAD>\n"
            "  <TITLE>%s</TITLE>\n"
            "</HEAD>\n"
            "<BODY BGCOLOR=\"#%s\" TEXT=\"#FFFFFF\">"
            "<CENTER><H1><FONT COLOR=\"#%s\">%s</FONT></H1>\n",
            title, bgColor, titleColor, title);
    fprintf(htmlFile, 
            "<a href=\"#starting\">Begining of the match</a><br><br>\n");
    htmlLinks(htmlFile);
    fprintf(htmlFile, 
            "<TABLE BORDER=1>\n"
            "  <TR><TD><CENTER><b>ID</b></CENTER></TD>\n"
	    "      <TD><CENTER><b>Date</b></CENTER></TD>\n"
            "      <TD><CENTER><b>Match Time</b></CENTER></TD>\n"
            "      <TD><CENTER><b>File</b></CENTER>\n</TD>\n"
	    "      <TD><CENTER><b>Line</b></CENTER></TD>\n"
            "      <TD><CENTER><b>Type</b></CENTER></TD>\n"
            "      <TD><CENTER><b>Message</b></CENTER></TD>\n"
            "  </TR>\n"); 
    lineCounter_=0;
}

// ---------------------------------------------------------------------------
// htmlFooter
// ---------------------------------------------------------------------------
// Met a jour un fichier avec les donnees necessaires pour finir un fichier 
// html 
// ---------------------------------------------------------------------------
void htmlFooter(FILE*       htmlFile,
                const char* filename,
                const char* filedate)
{
    fprintf(htmlFile, "</TABLE>\n\n");
    htmlLinks(htmlFile);
    fprintf(htmlFile, 
            "</CENTER>\n<BR>\n"
            "Generated from %s on %s\n"
            "</BODY>\n"
            "</HTML>\n",
            filename, filedate);      
}

// ---------------------------------------------------------------------------
// htmlLine
// ---------------------------------------------------------------------------
// Ecrit une ligne dans un fichier html
// ---------------------------------------------------------------------------
void htmlLine(FILE*       htmlFile,
              struct timeval packetTime,
              struct timeval packetTimeFromStartMatch,
              const char* typeColor,
              const char* type,
              const char* filename,
              int         line,
              const char* message)
{
    fprintf(htmlFile, 
            "  <TR><TD ALIGN=\"RIGHT\">%4d </TD>"
	    "      <TD>%02d'%02d\"%03ums </TD><TD>%02d'%02d\"%03ums </TD>\n"
            "      <TD ALIGN=\"RIGHT\">%s </TD><TD>%4d </TD>"
            "      <TD><FONT COLOR=\"#%s\">%s </FONT></TD>\n"
            "      <TD><FONT COLOR=\"#%s\">%s </FONT></TD>\n"
            "  </TR>\n",
	    lineCounter_++,
            (int)(packetTime.tv_sec/60), (int)(packetTime.tv_sec%60), 
	    (unsigned int)(packetTime.tv_usec/1000),
            (int)(packetTimeFromStartMatch.tv_sec/60), 
	    (int)(packetTimeFromStartMatch.tv_sec%60), 
            (unsigned int)(packetTimeFromStartMatch.tv_usec/1000),
            filename, line,
            typeColor, type,
            typeColor, message);      
}

// ---------------------------------------------------------------------------
// packetToHtml
// ---------------------------------------------------------------------------
// Analyse un packet et l'ecrit dans un fichier html
// ---------------------------------------------------------------------------
void packetToHtml(FILE* htmlFile,
                  LogPacketHeader const& packetHeader, 
                  Byte* data,
                  bool first,
                  struct timeval &packetTime,
                  struct timeval &packetTimeFromStartMatch) 
{
    if (first) {
        packetTime = packetHeader.timeStamp;
        packetTimeFromStartMatch = packetTime;
    }
    LogPacketMessage* msg = (LogPacketMessage*)data;
    if (packetHeader.type == LOG_TYPE_START_MATCH) {
        packetTimeFromStartMatch = packetHeader.timeStamp;
    }
    if (filter_[(int)packetHeader.type]) return;

    switch (packetHeader.type) {
    case LOG_TYPE_PING:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_START, // type color
                 "PING",
                 "_",
                 0, 
                 "_");
        break;
    case LOG_TYPE_ROBOT_MODEL:
        {
            char txt[30];
            LogPacketEnumValue* model= (LogPacketEnumValue*)data;
            if (model->value == 0) {
                strcpy(txt, "<br><b>Robot Attack<b><br><br>");
            } else {
                strcpy(txt, "<br><b>Robot Defence<b><br><br>");
            }
            htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_START, // type color
                 "START",
                 "_",
                 0, 
                 txt);
        }
        break; 
    case LOG_TYPE_SIMU:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_START, // type color
                 "START",
                 "_",
                 0, 
                 "<br><blink><b>Mode Simulated<b></blink><br><br>");
        break; 
    case LOG_TYPE_REAL:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_START, // type color
                 "START",
                 "_",
                 0, 
                 "<br><blink><b>Mode Real<b></blink><br><br>");
        break; 
    case LOG_LEVEL_COMMAND:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_COMMAND, // type color
                 "COMMAND",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 msg->mesg);
        break;
    case LOG_LEVEL_ERROR:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_ERROR, // type color
                 "ERROR",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 msg->mesg);
        break;
    case LOG_LEVEL_WARNING:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_WARNING, // type color
                 "WARNING",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 msg->mesg);
        break;
    case LOG_LEVEL_INFO:
        if (filterEnvDetector) {
            if (strstr(msg->mesg, "EVENTS_ENV_DETECTOR_") != NULL) return;
        }
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_INFO, // type color
                 "INFO",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 msg->mesg);
        break;
    case LOG_LEVEL_OK:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_OK, // type color
                 "OK",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 msg->mesg);
        break;
    case LOG_LEVEL_FUNCTION:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_FUNCTION, // type color
                 "FUNCTION",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 msg->mesg);
        break;
    case LOG_TYPE_START_MATCH:
        packetTimeFromStartMatch = packetHeader.timeStamp;
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_START, // type color
                 "START",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 "<a NAME=\"starting\"></a>"
                 "<br><blink><b>The match is starting</b><blink><br><br>");
        break;
    case LOG_TYPE_END_MATCH:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_START, // type color
                 "START",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 "<br><blink><b>The match is finished</b><blink><br><br>");
        break;
    case LOG_TYPE_START_STRATEGY:
        htmlLine(htmlFile,
                 packetHeader.timeStamp-packetTime,
                 packetHeader.timeStamp-packetTimeFromStartMatch,
                 COLOR_START, // type color
                 "START",
                 (strlen(msg->file)==0) ? "nofile":msg->file,
                 msg->line, 
                 "<br><blink><b>The strategy is starting<b></blink><br><br>");
        break; 

    case LOG_TYPE_POSITION:
        {
            char txt[255];
            LogPosition* pos= (LogPosition*)data;
            sprintf(txt, "x=%d, y=%d, theta=%d", 
                    pos->x, pos->y, pos->t);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_PERIODIC, // type color
                     "Position",
                     "_",
                     0, 
                     txt);
        } 
        break;
    case LOG_TYPE_MOTOR:
        {
            char txt[255];
            LogPacketMotorInfo* motor= (LogPacketMotorInfo*)data;
            sprintf(txt, "Coder Left=%d Right=%d -- PWM Left=%d, Right=%d", 
                    (int)motor->posLeft, (int)motor->posRight,
                    (int)motor->pwmLeft, (int)motor->pwmRight);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_PERIODIC, // type color
                     "Motor",
                     "_",
                     0, 
                     txt);
        }
        break;
    case LOG_TYPE_FIRE_BALLS:
        {
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_HLI, // type color
                     "HLI",
                     "_",
                     0, 
                     "Fire! balls launched!");
        }
        break;
    case LOG_TYPE_SOUND:
        {
            char txt[255];
            LogPacketEnumValue* sound= (LogPacketEnumValue*)data;
            sprintf(txt, "Sound: %d, %s", 
                    sound->value, 
                    soundList[sound->value].filename);
            // need #include sound.h
	    // if (sound-> value>= SOUND_NBR) return;
	    // sprintf(txt, "Sound: %s", soundList[sound->value].filename);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_HLI, // type color
                     "Sound",
                     "_",
                     0, 
                     txt);
        }
        break;
    case LOG_TYPE_OBSTACLE:
        {
            char txt[255];
            LogPacketObstacle* obstacle= ((LogPacketObstacle*)data);
            char detectorName[255];
            sprintf(detectorName, "detector %d", (int)obstacle->envDetectorId);
	    // need "envDetectorMapping.h"
	    /*strcpy(detectorName,
                   (obstacle->envDetectorId >= 0)?
                   envDetectorMapping_[(int)obstacle->envDetectorId].name:"bumper");*/
                    
            switch(obstacle->type) {
	    case ENV_OBSTACLE:
		sprintf(txt, "ENV: Obstacle envDetector detected by %s, "
                        "pt(%d %d)", 
                        detectorName, 
			(int)obstacle->x, (int)obstacle->y);
		break;
	    case BUMPER_OBSTACLE:
		sprintf(txt, "BUMPER: Obstacle Bumper pt(%d %d)", 
			(int)obstacle->x, (int)obstacle->y);
		break;
	    case ALL_OBSTACLES:
	    default:
		sprintf(txt, "ANY: Obstacle de type inconnu detected by %s, "
                        "pt(%d %d)",
                        detectorName,  
			(int)obstacle->x, (int)obstacle->y);
		break;
	    }
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_HLI, // type color
                     "OBSTACLE",
                     "-",
                     0, 
                     txt);
        }
        break;
    case LOG_TYPE_SKITTLE:
        {
            char txt[255];
            LogPt3D* skittle= ((LogPt3D*)data);
            sprintf(txt, "Skittle detected: pt(%d %d)", 
			(int)skittle->x, (int)skittle->y);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_HLI, // type color
                     "SKITTLE",
                     "-",
                     0, 
                     txt);
        }
        break; 
    case LOG_TYPE_SUPPORT:
        {
            char txt[255];
            LogSupport* support= ((LogSupport*)data);
            sprintf(txt, "Support detected: pt1(%d %d) pt2(%d %d)", 
			(int)support->x1, (int)support->y1,
			(int)support->x2, (int)support->y2);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_HLI, // type color
                     "SUPPORT",
                     "-",
                     0, 
                     txt);
        }
        break; 
     case LOG_TYPE_BRIDGE:
        {
            char txt[255];
            LogPacketEnumValue* bridge= ((LogPacketEnumValue*)data);
            sprintf(txt, "Bridge %d detected: y=%d cm", 
		    bridge->value, 
		    22+bridge->value*15);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_HLI, // type color
                     "SUPPORT",
                     "-",
                     0, 
                     txt);
        }
        break; 
    case LOG_TYPE_TRAJECTORY:
        {
            char txt[255];
            LogPacketTrajectory* traj= ((LogPacketTrajectory*)data);
            for(int w=0;w<traj->nbrPt;w++) {
                sprintf(txt, "pt[%d]=(%d %d)", w,
                        (int)traj->ptx[w], (int)traj->pty[w]);
                htmlLine(htmlFile,
                         packetHeader.timeStamp-packetTime,
                         packetHeader.timeStamp-packetTimeFromStartMatch,
                         COLOR_HLI, // type color
                         "HLI",
                         "_",
                         0, 
                         txt);
            }
        }
    case LOG_TYPE_JACKIN:
        {
            LogPacketBoolean* jackin= ((LogPacketBoolean*)data);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_START, // type color
                     "HLI",
                     "_",
                     0, 
                     (jackin->value) ? "START JACK INSERTED": "START JACK EJECTED" );
        }
     case LOG_TYPE_EMERGENCY_STOP:
        {
            LogPacketBoolean* esp= ((LogPacketBoolean*)data);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_START, // type color
                     "HLI",
                     "_",
                     0, 
                     (esp->value) ? "EMERGENCY STOP PRESSED": "EMERGENCY STOP RELEASED" );
        }
    case LOG_TYPE_LCD:
        {
            LogPacketLcd* lcd= ((LogPacketLcd*)data);
            htmlLine(htmlFile,
                     packetHeader.timeStamp-packetTime,
                     packetHeader.timeStamp-packetTimeFromStartMatch,
                     COLOR_LCD, // type color
                     "LCD",
                     "lcd.cpp",
                     0, 
                     strlen(lcd->txt)>0?lcd->txt:"<i>lcdClear()</i>");
        }
    default:
        break;		
    }
}

// ---------------------------------------------------------------------------
// log2html
// ---------------------------------------------------------------------------
// Convertir un log en fichier html
// ---------------------------------------------------------------------------
void log2html(const char* filename,
              const char* htmlFilename,
              const char* title)
{
    ZFile file;
    if (!file.open(filename, FILE_MODE_READ)) {
        printf("Cannot open log file: %s\n", filename);
        exit(-1);
    }
    LogPacketHeader packetHeader((LogType)0); 
    Byte data[LOG_DATA_MAX_LENGTH];
    struct timeval packetTime;
    struct timeval packetTimeFromStartMatch;
    bool first=true;
    FILE* htmlFile=fopen(htmlFilename, "wb");
    if (htmlFile==NULL) {
        printf("Cannot open htmlFile: %s\n", htmlFilename);
        exit(-1);
    }
    htmlHeader(htmlFile, (title!=NULL) ? title:filename, "FF0000", "000000");

    while(restorePacket(&file, packetHeader, data)) {
        packetToHtml(htmlFile,
                     packetHeader, 
                     data,
                     first,
                     packetTime,
                     packetTimeFromStartMatch);
        first=false;
    }

    htmlFooter(htmlFile, filename, "...");

    fclose(htmlFile);
    file.close();
    printf("Html log generated in: %s\n", htmlFilename);
}
	
// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char*argv[]) 
{    
    if (argc<2) {
        printf("Usage: %s logFileName {logTitle} {targetDirectory=.} "
               "{targetName=logFileName}\n", argv[0]);
        return -1;
    }
    // get the log file name and source directory
    char baseName[255];
    char baseDirectory[255];
    char htmlDirectory[255];
    char htmlFilename[255];
    char htmlTitle[255];
    char* tmp=strrchr(argv[1], '/');
    if (tmp) {
        strcpy(baseName, tmp+1);
        strncpy(baseDirectory, argv[1], strlen(argv[1])-strlen(baseName));
    } else {
        strcpy(baseName, argv[1]);
        strncpy(baseDirectory,"./", 255);
    }
    // remove the file name extension
    tmp=strchr(baseName, '.');
    if (tmp) { strcpy(tmp, ""); }
    
    // get target filename
    if (argc>=5) {
        strncpy(baseName, argv[4], 255);
        // remove the file name extension
        tmp=strrchr(baseName, '.');
        if (tmp) { tmp[0]=0; }
    }

    // get target directory
    if (argc>=4) {
        strncpy(htmlDirectory, argv[3], 255);
        if (htmlDirectory[strlen(htmlDirectory)-1] != '/')
            strcat(htmlDirectory, "/");
    } else {
        htmlDirectory[0]='.';
        htmlDirectory[1]='/';
        htmlDirectory[2]=0;
    }
   
    linkNbr_=4;
    assert(linkNbr_ <= LOG_LINK_NBR);
    sprintf(link_[0], "%s.html",    baseName);
    sprintf(link_[1], "%sEvt.html", baseName);
    sprintf(link_[2], "%sErr.html", baseName);
    sprintf(link_[3], "%sAll.html", baseName);
    strcpy(linkName_[0], "Standard");
    strcpy(linkName_[1], "All Events");
    strcpy(linkName_[2], "Err/Warn/OK");
    strcpy(linkName_[3], "All");

    for(int i=0;i<linkNbr_;i++) {
        initFilter();
        switch(i) {
        case 3: // display all info and function
           for(int j=0;j<LOG_TYPE_NBR;j++) filter_[j]=false;
           filterEnvDetector=false;
           break;
        case 2: // do not display info and function
           filter_[LOG_LEVEL_INFO]=true;
           filter_[LOG_LEVEL_FUNCTION]=true;
           break;
        case 1: // do not filter envdetector
           filterEnvDetector=false;
           break;
        case 0: // default
        default:
            break;
        }
        sprintf(htmlFilename, "%s%s", htmlDirectory, link_[i]);
        sprintf(htmlTitle, "%s - %s", argc>=3?argv[2]:baseName, linkName_[i]);
        log2html(argv[1], htmlFilename, htmlTitle);
    }
    
    return(EXIT_SUCCESS);
}
