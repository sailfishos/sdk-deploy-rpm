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
