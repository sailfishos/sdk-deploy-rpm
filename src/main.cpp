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

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>
#include <QStringList>
#include <QFileInfo>
#include <QDebug>
#include <QDBusReply>
#include <QUrl>

#include "deployer.h"

int
main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    auto tr = [](const char *message) {
        return QCoreApplication::translate("main", message);
    };

    QCommandLineParser parser;
    QCommandLineOption undeployOption("undeploy", tr("Undeploy previously deployed packages"));

    parser.addHelpOption();
    parser.addOptions({undeployOption});
    parser.addPositionalArgument("package",
            tr("Package file when deploying, package name when undeploying"), "package...");

    parser.process(app);

    if (parser.positionalArguments().isEmpty())
        parser.showHelp(1);

    if (parser.isSet(undeployOption)) {
        Undeployer undeployer(parser.positionalArguments());

        QDBusReply<void> result = undeployer.run();
        if (!result.isValid()) {
            qCritical() << "Failed to request package removal:" << result.error().message();
            return 1;
        }

        return app.exec();
    } else {
        for (const QString &file : parser.positionalArguments()) {
            if (!QFileInfo(file).exists()) {
                qerr() << tr("No such file: %1").arg(file) << endl;
                return 1;
            }
        }

        Deployer deployer(parser.positionalArguments());

        QDBusReply<void> result = deployer.run();
        if (!result.isValid()) {
            qCritical() << "Failed to request file installation:" << result.error().message();
            return 1;
        }

        return app.exec();
    }
}
