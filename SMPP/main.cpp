#include <QCoreApplication>
#include <QSettings>

#include "ESMETransceiver.h"


namespace {
    const QString CONFIG_FILE_NAME = "config.ini";

    const QString DEFAULT_HOSTNAME = "xml55.smstec.ru";
    const quint16 DEFAULT_PORT = 3333;
    const QString DEFAULT_LOGIN = "Test";
    const QString DEFAULT_PASSWORD = "Test";
    const QString DEFAULT_SYSTEM_TYPE = "WWW";
    const quint8 DEFAULT_SMPP_VERSION = 34;

    void writeDefaultConfigIfNeeded()
    {
        QSettings setting(CONFIG_FILE_NAME, QSettings::IniFormat);
        if (setting.allKeys().isEmpty()) {
            setting.setValue("hostname", DEFAULT_HOSTNAME);
            setting.setValue("port", DEFAULT_PORT);
            setting.setValue("login", DEFAULT_LOGIN);
            setting.setValue("password", DEFAULT_PASSWORD);
            setting.setValue("systemType", DEFAULT_SYSTEM_TYPE);
            setting.setValue("smmpVersion", DEFAULT_SMPP_VERSION);
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    writeDefaultConfigIfNeeded();

    QSettings setting(CONFIG_FILE_NAME, QSettings::IniFormat);

    QString hostname = setting.value("hostname", DEFAULT_HOSTNAME).toString();
    bool portParsed = false;
    quint16 port = setting.value("port", DEFAULT_PORT).toInt(&portParsed);
    if (!portParsed) port = DEFAULT_PORT;
    QString login = setting.value("login", DEFAULT_LOGIN).toString();
    QString password = setting.value("password", DEFAULT_PASSWORD).toString();
    QString systemType = setting.value("systemType", DEFAULT_SYSTEM_TYPE).toString();
    bool smmpVersionParsed = false;
    quint8 smmpVersion = setting.value("smmpVersion", DEFAULT_SMPP_VERSION).toInt(&smmpVersionParsed);
    if (!smmpVersionParsed) smmpVersion = DEFAULT_SMPP_VERSION;

    ESMETransceiver esme(hostname, port, login, password, systemType, smmpVersion);
    QObject::connect(&esme, &ESMETransceiver::closed, [&a] {
        a.exit();
    });

    return a.exec();
}

