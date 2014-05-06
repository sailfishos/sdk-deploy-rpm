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
#include <QStringList>
#include <QFileInfo>

#include <cstdio>

#include "deployer.h"

int
main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = QCoreApplication::arguments();
    args.pop_front();

    QStringList rpms;
    foreach (const QString &arg, args) {
        QFileInfo info(arg);
        if (!info.exists()) {
            fprintf(stderr, "File does not exist: %s\n", qPrintable(arg));
            return 2;
        }

        rpms << info.absoluteFilePath();
    }

    if (QCoreApplication::arguments().size() == 1) {
        fprintf(stderr, "Usage: %s filename.rpm [...]\n", argv[0]);
        return 1;
    }

    Deployer deployer(rpms);
    deployer.run();

    return app.exec();
}
