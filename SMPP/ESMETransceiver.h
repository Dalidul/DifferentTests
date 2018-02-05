#pragma once

#include <QObject>

// стандартная библиотека, как заказывали
#include <thread>


class ESMETransceiver: public QObject
{
    Q_OBJECT

public:
    ESMETransceiver(const QString& hostname,
                    quint16 port,
                    const QString& login,
                    const QString& password,
                    const QString& systemType,
                    quint8 smmpVersion,
                    QObject* parent = nullptr);

signals:
    void closed();

public:
    void close();

private:
    bool initSocket();
    void transmitBindPDU();
    void waitForBindPDUResponse();
    void handleCommandStatus(int status);

private:
    int m_socket;  // QTcpSocket можно использовать только с QThread

    std::thread m_transmitterThread;
    std::thread m_receiverThread;

    QString m_hostname;
    quint16 m_port;
    QString m_login;
    QString m_password;
    QString m_systemType;
    quint8  m_smmpVersion;
};
