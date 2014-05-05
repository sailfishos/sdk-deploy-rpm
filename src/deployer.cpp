#include <QCoreApplication>
#include <QTimer>
#include <QStringList>

#include <Transaction>

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
    QStringList id = itemID.split(';');
    if (id.size() < 2) {
        id << "";
    }
    fprintf(stderr, "%s %s: [%d %]", qPrintable(id[0]), qPrintable(id[1]), percentage);
    fprintf(stderr, "\n");
}

void 
Deployer::onFinished(PackageKit::Transaction::Exit status,
        uint runtime)
{
    fprintf(stderr, "Finished transaction (status=%d, runtime=%dms)\n", status, runtime);
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
    fprintf(stderr, "\nMessage: %s\n", qPrintable(message));
}
