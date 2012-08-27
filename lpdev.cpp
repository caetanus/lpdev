#include "lpdev.h"

#if defined(WIN32)

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




LpDev::LpDev(int fd, int port)
    : QObject(0)
{
#ifdef WIN32
    StartInpout();
#endif
    m_fd = fd;
    m_port = port;
    qDebug() << "lpt port" << port << "openned with fd" << fd;

}

LpDev* LpDev::open(int port)
{
    int fd = internalOpen(port);
    switch(fd) {
        case -1:
            return NULL;
            break;
        case -255:
            return m_ports[port];
            break;
        default:
            LpDev* lp = new LpDev(fd, port);
            m_ports[port] = lp;
            lp->write(0);
            return lp;
    }

}

int LpDev::internalOpen(int port)
{

#ifdef WIN32
    Q_ASSERT(port < 3 and port >= 0);
    if(port > 2 and port < 0){
        return -1;
    }
#endif
    if (m_ports.keys().contains(port)) {
        return -255;
    }
    else {
#ifdef linux
        int fd;
        QString device = QString("/dev/parport%1").arg(QString::number(port));
        if ((fd = ::open(device.toAscii(), O_RDWR)) < 0) {
            return -1;
        }
        if(ioctl(fd, PPCLAIM)) {
            ::close(fd);
            return -1;
        }

        return fd;
#elif WIN32
        return port;
#endif
    }
}

/**
 * Close and releases the paralel port
 *
 */
void LpDev::close()
{
#ifdef linux
    ioctl(m_fd, PPRELEASE);
    ::close(m_fd);
#endif
}

void LpDev::write(uint udata)
{
    uchar data = (uchar)udata;
    internalOpen(m_port);
    m_data = data;
#ifdef linux
    ioctl(m_fd, PPWDATA, &data);
#elif defined(WIN32)
    Out(IOPorts[m_fd].portOut, data);
#endif
}

uchar LpDev::readStatus()
{
#ifdef WIN32
    return In(IOPorts[m_fd].portStatus);
#elif defined(linux)
    int status;
    ioctl(m_fd, PPRSTATUS, &status);
    return (uchar)status;
#endif

}

uchar LpDev::readControl()
{
    internalOpen(m_port);
#ifdef WIN32
    return In(IOPorts[m_fd].portControl);
#elif defined(linux)
    int control;
    ioctl(m_fd, PPRCONTROL, &control);
    return control;
#endif
}


void LpDev::writeControl(uint udata)
{
    uchar data = (uchar)udata;
    internalOpen(m_port);
#ifdef WIN32
    Out(IOPorts[m_port].portControl, data);
#elif defined(linux)
    ioctl(m_fd, PPWCONTROL, &data);
#endif
}

void LpDev::writeStatus(uint udata)
{
    uchar data = (uchar)udata;
    internalOpen(m_port);
#ifdef WIN32
    Out(IOPorts[m_fd].portStatus, data);
#elif defined(linux)
#define PPWSTATUS2	_IOW(PP_IOCTL, 0x82, unsigned char)

    ioctl(m_fd, PPWSTATUS2, &data);
#endif
}


uchar LpDev::read()
{

    return m_data;
}

LpDev::~LpDev(){
    close();
    delete m_ports[m_port];
}
