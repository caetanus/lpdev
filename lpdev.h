/*
 *  Cross Platform paralel port comunication
 *  (c) 2012 TekSof
 *  author: Marcelo Aires Caetano - marcelo dot caetano at teksof dot com dot br
 *
 */

/**
  * \abstract This module provides a transparent interface to paralel port communication in windows and linux.
  * in windows, it uses inpout32.dll loaded dinamically, in linux it uses ppdev and /dev/parport*
  * \note in linux the user must be a member of lp group and this module doesn't requires root
  * \note this module doesn't works on bsd or other unixes yet
  **/

#ifndef LPDEV_H
#define LPDEV_H



#include <Qt/QtCore>
#include <QObject>
#include <QMap>
#include <stdio.h>

#ifdef linux

#include <fcntl.h> /* open() */
#include <sys/types.h> /* open() */
#include <sys/stat.h> /* open() */
#include <sys/ioctl.h>
#include <linux/parport.h>
#include <linux/ppdev.h>

#elif defined(WIN32)

#   include <windows.h>
typedef short  (*Inp32Ptr)(short PortAddress);
void (*Out32Ptr) (short PortAddress, short data);
Inp32Ptr Inp = 0;
Out32Ptr Out = 0;
HINSTANCE InpOut32DLL;

typedef struct{
    int portOut;
    int portStatus;
    int portControl;
} IOPortNumber;

IOPortNumber IOPorts[3];

void StartInpout(){
    static bool started = false;
    if (started)
        return;
    if(!(InpOut32DLL = LoadLibrary("inpout32.dll"))) {
        MessageBox(0, "Couldn't Find inpout32.dll, exiting\nPlease put this dll in your path.", "error", MB_ICONERROR| MB_OK);
        exit(1);
    }
    Inp = (Inp32Ptr)GetProcAddress(InpOut32DLL, "Inp32");
    Out = (Out32Ptr)GetProcAddress(InpOut32DLL, "Out32");

    // starting the addressment of the ports
    IOPorts[0].portOut = 0x378;
    IOPorts[0].portStatus = 0x379;
    IOPorts[0].portControl = 0x37A;

    IOPorts[1].portOut = 0x278;
    IOPorts[1].portStatus = 0x279;
    IOPorts[1].portControl = 0x27A;

    IOPorts[2].portOut = 0x3BC;
    IOPorts[2].portStatus = 0x3BD,
    IOPoera[2].portControl = 0x3BE;

    started = true;
}


#endif

class LpDev : public QObject {
Q_OBJECT

public:
   static LpDev* open(int port = 0);

   ~LpDev();
   void write(uchar data);
   uchar read();
   void writeStatus(uchar data);
   uchar readStatus();
   void writeControl(uchar data);
   uchar readControl();
private:
   void close();
   int m_fd;
   uchar m_data;
   LpDev(int fd);
   static QMap<int,LpDev*> m_ports;		// singleton instances
};
#endif // LPDEV_H



