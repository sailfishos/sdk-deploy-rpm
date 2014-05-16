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
#include <cstdio>

#include "deployer.h"


Deployer::Deployer(QStringList rpms)
    : QObject(0)
    , tx(0)
    , state(INITIAL)
    , rpms(rpms)
{
}

Deployer::~Deployer()
{
}

PackageKit::Transaction *
Deployer::transaction()
{
    tx = new PackageKit::Transaction();

    QObject::connect(tx, SIGNAL(changed()),
                   this, SLOT(onChanged()));
    QObject::connect(tx, SIGNAL(itemProgress(const QString &, PackageKit::Transaction::Status, uint)),
                   this, SLOT(onItemProgress(const QString &, PackageKit::Transaction::Status, uint)));
    QObject::connect(tx, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
                   this, SLOT(onFinished(PackageKit::Transaction::Exit, uint)));
    QObject::connect(tx, SIGNAL(message(PackageKit::Transaction::Message, const QString &)),
                   this, SLOT(onMessage(PackageKit::Transaction::Message, const QString &)));
    QObject::connect(tx, SIGNAL(errorCode(PackageKit::Transaction::Error, const QString &)),
                     this, SLOT(onErrorCode(PackageKit::Transaction::Error, const QString &)));
    return tx;
}

void
Deployer::run()
{
    switch (state) {
        case INITIAL:
            fprintf(stderr, "INSTALLING\n");
            transaction()->installFiles(rpms,
                    PackageKit::Transaction::TransactionFlagNone);
            state = INSTALLING;
            break;

        default:
            break;
    }
}

void
Deployer::onChanged()
{
    const char *action = "Working";

    switch (tx->status()) {
        case PackageKit::Transaction::StatusWait:
            action = "Waiting";
            break;
        case PackageKit::Transaction::StatusSetup:
            action = "Preparing";
            break;
        case PackageKit::Transaction::StatusRefreshCache:
            action = "Refreshing";
            break;
        case PackageKit::Transaction::StatusFinished:
            action = "Finished";
            break;
        case PackageKit::Transaction::StatusDownload:
            action = "Downloading";
            break;
        case PackageKit::Transaction::StatusInstall:
            action = "Installing";
            break;
        case PackageKit::Transaction::StatusUpdate:
            action = "Updating";
            break;
        case PackageKit::Transaction::StatusDepResolve:
            action = "Resolving";
            break;
        default:
            break;
    }

    fprintf(stderr, "%s", action);
    if (tx->percentage() != 101) {
        fprintf(stderr, ": %d%%", tx->percentage());
    }

    if (tx->remainingTime()) {
        fprintf(stderr, " (remaining: %ds)", tx->remainingTime());
    }

    fprintf(stderr, "\n");
}

void
Deployer::onItemProgress(const QString &itemID,
        PackageKit::Transaction::Status status,
        uint percentage)
{
    Q_UNUSED(status);
    QStringList id = itemID.split(';');
    if (id.size() < 2) {
        id << "";
    }
    fprintf(stderr, "%s %s: [%d %%]", qPrintable(id[0]), qPrintable(id[1]), percentage);
    fprintf(stderr, "\n");
}

void 
Deployer::onFinished(PackageKit::Transaction::Exit status,
        uint runtime)
{
    fprintf(stderr, "Finished transaction (status=%u, runtime=%dms)\n", status, runtime);
    tx->deleteLater();
    tx = NULL;

    state = DONE;
    fprintf(stderr, "FINISHING\n");
    QCoreApplication::exit((status == PackageKit::Transaction::ExitSuccess) ? 0 : 1);
}

void
Deployer::onMessage(PackageKit::Transaction::Message type,
        const QString &message)
{
    Q_UNUSED(type);
    fprintf(stderr, "\nMessage: %s\n", qPrintable(message));
}

void
Deployer::onErrorCode(PackageKit::Transaction::Error error,
                      const QString &details)
{
    Q_UNUSED(error);
    fprintf(stderr, "Error: %s\n", qPrintable(details));
}
