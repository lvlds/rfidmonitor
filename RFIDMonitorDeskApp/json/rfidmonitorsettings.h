/****************************************************************************
**
** www.celtab.org.br
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the RFIDMonitor project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

#ifndef RFIDMONITORSETTINGS_H
#define RFIDMONITORSETTINGS_H

#include <QDebug>

namespace json {

class Service
{
public:

    int serviceType() const;
    void setServiceType(int serviceType);

    QString serviceName() const;
    void setServiceName(const QString &serviceName);

private:
    int m_serviceType;
    QString m_serviceName;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class Module
{
public:
    explicit Module(const QString &name = "");
    QString moduleName() const;
    void setModuleName(const QString &moduleName);

    int version() const;
    void setVersion(int version);

    QList<Service> services() const;
    void setServices(const QList<Service> &services);

    bool operator==(const Module &module);

private:
    QString m_moduleName;
    int m_version;
    QList<Service> m_services;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class DefaultServices
{

public:
    QString reader() const;
    void setReader(const QString &reader);
    QString persister() const;
    void setPersister(const QString &persister);
    QString exporter() const;
    void setExporter(const QString &exporter);
    QString packager() const;
    void setPackager(const QString &packager);
    QString synchronizer() const;
    void setSynchronizer(const QString &synchronizer);

    QString communicator() const;
    void setCommunicator(const QString &communicator);

private:
    QString m_reader;
    QString m_persister;
    QString m_exporter;
    QString m_packager;
    QString m_synchronizer;
    QString m_communicator;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class Network
{
public:

    QString essid() const;
    void setEssid(const QString &essid);

    QString password() const;
    void setPassword(const QString &password);

private:
    QString m_essid;
    QString m_password;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

class RFIDMonitorSettings
{
public:
    RFIDMonitorSettings();

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    QList<Module> modules() const;
    void setModules(const QList<Module> &modules);

    DefaultServices defaultServices() const;
    void setDefaultServices(const DefaultServices &defaultServices);

    Network networkConfiguration() const;
    void setNetworkConfiguration(const Network &networkConfiguration);

    QString device() const;
    void setDevice(const QString &device);

    QString serverAddress() const;
    void setServerAddress(const QString &serverAddress);

    int serverPort() const;
    void setServerPort(const int &serverPort);

private:
    int m_id;
    int m_serverPort;
    QString m_name;
    QString m_macAddress;
    QString m_device;
    QString m_serverAddress;
    int m_port;

    QList<Module> m_modules;
    DefaultServices m_defaultServices;
    Network m_networkConfiguration;

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
};

}


#endif // RFIDMONITORSETTINGS_H
