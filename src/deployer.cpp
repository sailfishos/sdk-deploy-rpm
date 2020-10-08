/*
 Copyright (C) 2014 Jolla Oy
 Contact: Jarko Vihriälä <jarko.vihriala@jolla.com>
 All rights reserved.

 You may use this file under the terms of BSD license as follows:

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of the Jolla Ltd nor the
     names of its contributors may be used to endorse or promote products
     derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QCoreApplication>
#include <QTimer>
#include <QDBusConnection>
#include <cstdio>

#include "deployer.h"

static const char * const DBUS_SERVICE = "org.sailfishos.installationhandler";
static const char * const INSTALLATIONHANDLER_DBUS_PATH = "/org/sailfishos/installationhandler";
static const char * const DBUS_INTERFACE = "org.sailfishos.installationhandler";

Deployer::Deployer(QStringList rpms)
    : QObject(0)
    , rpms(rpms)
    , client(DBUS_SERVICE, INSTALLATIONHANDLER_DBUS_PATH, DBUS_INTERFACE)
    , watcher(DBUS_SERVICE, client.connection())
{
    connect(&client, SIGNAL(installFinished(bool,QString)), this, SLOT(onFinished(bool,QString)));
    connect(&client, SIGNAL(needConfirm()), this, SLOT(showConfirm()));
    connect(&watcher, &QDBusServiceWatcher::serviceUnregistered, this, &Deployer::onUnregistered);
}

Deployer::~Deployer()
{
}

QDBusReply<void>
Deployer::run()
{
    return client.call("installFiles", rpms);
}

void 
Deployer::onFinished(bool success, const QString &errorString)
{
    if (!success)
        fprintf(stderr, "%s\n", qPrintable(errorString));
    fprintf(stderr, "Installation %s.\n", success ? "successful" : "failed");
    disconnect(&client, SIGNAL(installFinished(bool)), this, SLOT(onFinished(bool)));
    disconnect(&watcher, &QDBusServiceWatcher::serviceUnregistered, this, &Deployer::onUnregistered);
    QCoreApplication::exit(success ? 0 : 1);
}

void
Deployer::onUnregistered(const QString &)
{
    fprintf(stderr, "Installation failure: service died.\n");
    QCoreApplication::exit(1);
}

void Deployer::showConfirm()
{
    fprintf(stderr, "Please confirm installation on device.\n");
}
