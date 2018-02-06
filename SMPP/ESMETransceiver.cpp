#include "ESMETransceiver.h"

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

#include <QDebug>


namespace {
    struct PDUHeader {
        quint32 command_length;
        quint32 command_id;
        quint32 command_status;
        quint32 sequence_number;
    };

    QByteArray createBindTransceiverPDU(quint32 sequenceNumber,
                                        const QString& systemId,
                                        const QString& password,
                                        const QString& systemType,
                                        quint8 interfaceVersion)
    {
        QByteArray result;

        PDUHeader header {0, htonl(0x00000009), 0, htonl(sequenceNumber)};
        result.append(systemId.toLatin1(), systemId.size())
              .append('\0')
              .append(password.toLatin1(), password.size())
              .append('\0')
              .append(systemType.toLatin1(), systemType.size())
              .append('\0')
              .append(interfaceVersion)
              .append(char(0))
              .append(char(0))
              .append('\0');
        header.command_length = htonl(sizeof(header) + result.size());
        result.insert(0, reinterpret_cast<char*>(&header), sizeof(header));

        return result;
    }

    bool hostnameToIp(const char* hostname , in_addr* ip)
    {
        bool result = false;

        hostent* he;
        if ((he = gethostbyname(hostname)) != NULL) {
            in_addr** addr_list = (in_addr **)he->h_addr_list;
            for(int i = 0; addr_list[i] != NULL; i++) {
                memcpy(ip, addr_list[i], he->h_length);
                result = true;
            }
            if (!result) qWarning() << QObject::tr("No ip was found for host");
        }
        else qWarning() << QObject::tr("It's impossible to get ip by host name");

        return result;
    }
}


ESMETransceiver::ESMETransceiver(const QString& hostname,
                                 quint16 port,
                                 const QString& login,
                                 const QString& password,
                                 const QString& systemType,
                                 quint8 smmpVersion,
                                 QObject* parent)
    : QObject(parent)
    , m_socket(-1)
    , m_hostname(hostname)
    , m_port(port)
    , m_login(login)
    , m_password(password)
    , m_systemType(systemType)
    , m_smmpVersion(smmpVersion)
{
    if (initSocket()) {
        m_transmitterThread = std::thread(&ESMETransceiver::transmitBindPDU, this);
        m_receiverThread = std::thread(&ESMETransceiver::waitForBindPDUResponse, this);
    }
    else emit close();
}

ESMETransceiver::~ESMETransceiver()
{
    if (m_receiverThread.joinable()) m_receiverThread.join();
    if (m_transmitterThread.joinable()) m_transmitterThread.join();
    if (m_socket != -1) {
        shutdown(m_socket, SHUT_RDWR);
        ::close(m_socket);
        m_socket = -1;
    }
}

bool ESMETransceiver::initSocket()
{
    bool result = false;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket != -1) {
        sockaddr_in serverAddr;
        serverAddr.sin_family  = AF_INET;
        serverAddr.sin_port = htons(m_port);
        if (hostnameToIp(m_hostname.toLatin1(), (in_addr*)&serverAddr.sin_addr.s_addr)) {
            qInfo() << tr("Connecting to %1").arg(inet_ntoa(*(in_addr*)&serverAddr.sin_addr.s_addr));
            result = (::connect(m_socket, (sockaddr*) &serverAddr, sizeof(serverAddr)) == 0);
            if (!result) {
                qWarning() << tr("It's impossible to connect to the server %1:%2 (%3)")
                              .arg(m_hostname, QString::number(m_port), tr(std::strerror(errno)));
            }
            else qInfo() << tr("Connected");
        }
    }
    else qWarning() << tr("It's impossible to create socket");

    return result;
}

void ESMETransceiver::transmitBindPDU()
{
    QByteArray pdu = createBindTransceiverPDU(0,
                                              m_login,
                                              m_password,
                                              m_systemType,
                                              m_smmpVersion);
    // т.к. операции с сокетом блокирующие, то программа никогда не завершится,
    // если здесь произойдёт ошибка
    if (send(m_socket, pdu.data(), pdu.size(), MSG_NOSIGNAL) != pdu.size()) {
        qWarning() << tr("Transmission error");
    }
}

void ESMETransceiver::waitForBindPDUResponse()
{
    PDUHeader header;
    if (recv(m_socket, (char*)&header, sizeof(PDUHeader), MSG_NOSIGNAL) == sizeof(PDUHeader)) {
        if (ntohl(header.sequence_number) == 0) handleCommandStatus(ntohl(header.command_status));
        else {
            qWarning() << tr("Unexpected sequence number: %1")
                          .arg(QString::number(ntohl(header.sequence_number)));
        }
    }
    else qWarning() << tr("It's impossible to read message header");

    emit close();
}

void ESMETransceiver::handleCommandStatus(int status)
{
    if (status == 0) qInfo() << tr("SMPP connection established successfully.");
    else tr("SMPP connection error: %1").arg(QString::number(status));
}
