#ifndef DEVEL_DEPLOY_RPM_DEPLOYER_H
#define DEVEL_DEPLOY_RPM_DEPLOYER_H

#include <QObject>
#include <QStringList>

#include <Transaction>

class Deployer : public QObject {
    Q_OBJECT

public:
    Deployer(QStringList rpms);
    ~Deployer();

public slots:
    void run();
    void onChanged();
    void onItemProgress(const QString &itemID,
            PackageKit::Transaction::Status status,
            uint percentage);
    void onFinished(PackageKit::Transaction::Exit status,
            uint runtime);
    void onMessage(PackageKit::Transaction::Message type,
            const QString &message);

private:
    PackageKit::Transaction *transaction();

    PackageKit::Transaction *tx;

    enum State {
        INITIAL = 0,
        INSTALLING,
        DONE,
    } state;

    QStringList rpms;
};

#endif /* DEVEL_DEPLOY_RPM_DEPLOYER_H */
