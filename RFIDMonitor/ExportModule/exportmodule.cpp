/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the FishMonitoring project
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
#include <servicemanager.h>
#include <QObject>

#include "logger.h"
#include "exportmodule.h"
#include "export/exportlocaldata.h"

static QThread *m_exportThread = 0;
static QThread *m_daemonThread = 0;
static ExportLocalData *m_exporter = 0;

/*!
 * \brief start_daemon starts Threads to detect new devices to export data into it and a thread to export the data.
 */
void start_daemon(){
    /*
     * Creates the object of the classes and then move these objects to execute as thread.
     * Make the connection between threads. Starts both threads.
     */
    m_exportThread = new QThread();
    m_exporter = new ExportLocalData();

    m_exporter->moveToThread(m_exportThread);
    QObject::connect(m_exportThread, SIGNAL(started()), m_exporter, SLOT(startExport()));

    m_daemonThread = new QThread();

    DeviceThread::instance()->moveToThread(m_daemonThread);
    QObject::connect(m_daemonThread, SIGNAL(started()), DeviceThread::instance(), SLOT(startListening()));
    QObject::connect(DeviceThread::instance(), SIGNAL(exportToDevice(QString)), m_exporter, SLOT(exportAction(QString)));
    QObject::connect(DeviceThread::instance(), SIGNAL(redLedOff()), m_exporter, SLOT(turnOffLed()));

    m_exportThread->start();
    m_daemonThread->start();
}

ExportModule::ExportModule(QObject *parent) :
    CoreModule(parent), m_exportThread(0)
{

}

ExportModule::~ExportModule()
{
    /* First stop the thread. For second delete the object running on the thread.
     * And then delete the thread.
     */
    m_exportThread->exit(0);
    m_exporter->deleteLater();
    m_exportThread->deleteLater();

    m_daemonThread->exit(0);
    m_device->deleteLater();
    m_daemonThread->deleteLater();
}

void ExportModule::init()
{
    // register the service to start a thread to detect devices
    ServiceManager::instance()->register_function("export.start_daemon", std::function< void()>(start_daemon));
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ExportModule, ExportModule)
#endif // QT_VERSION < 0x050000
