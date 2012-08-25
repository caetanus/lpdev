#include "lpdev.h"

LpDev::LpDev(int fd)
    : QObject(0)
{
#ifdef WIN32
    StartInpout();
#endif
    m_fd = fd;
    write(0);
}

LpDev* LpDev::open(int port)
{

#ifdef WIN32
    Q_ASSERT(port < 3 and port >= 0);
#endif
    if (m_ports.keys().contains(port)) {
        return m_ports[port];
    }
    else {
#ifdef linux
        int fd;
        QString device = QString("/dev/parport%1").arg(QString::number(port));
        if ((fd = ::open(device.toStdString().c_str(), O_RDWR)) < 0) {
            qDebug() << "unable to open '" << device << "'\n";
            return 0;
        }
        if(ioctl(fd, PPCLAIM)) {
            qDebug() << "unable to claim interface of device '" << device << "'\n";
            ::close(fd);
            return 0;
        }
        LpDev* r =  new LpDev(fd);
        m_ports[port] = r;
        return r;
#elif WIN32
        LpDev* r =  new LpDev(port);
        m_ports[port] = r;
        return r;
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

void LpDev::write(uchar data)
{
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
#ifdef WIN32
    return In(IOPorts[m_fd].portControl);
#elif defined(linux)
    int control;
    ioctl(m_fd, PPRCONTROL, &control);
    return control;
#endif
}


void LpDev::writeControl(uchar data)
{
#ifdef WIN32
    Out(IOPorts[m_fd].portControl, data);
#elif defined(linux)
    ioctl(m_fd, PPWCONTROL, &data);
#endif
}

void LpDev::writeStatus(uchar data)
{
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
    delete m_ports[m_fd];
}
