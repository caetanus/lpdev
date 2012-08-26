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
#include <QHash>
#include <QMetaType>
#include <stdio.h>

#ifdef linux

#include <fcntl.h> /* open() */
#include <sys/types.h> /* open() */
#include <sys/stat.h> /* open() */
#include <sys/ioctl.h>
#include <linux/parport.h>
#include <linux/ppdev.h>

#elif defined(WIN32)

#include <windows.h>

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

};

static QHash<int,LpDev*> m_ports;		// singleton instances


#endif // LPDEV_H



