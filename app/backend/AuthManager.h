#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#endif // AUTHMANAGER_H

#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class AuthManager : public QObject
{
    Q_OBJECT

public:
    explicit AuthManager(QObject *parent = nullptr);

public slots:
    void login(QString username, QString password);

signals:
    //void loginSuccess(QString vmIp);
    void loginSuccess(QString vmIp, QString appName);
    void loginFailed(QString errorMessage);

// private slots:
//     void onLoginReply();

private:
    QNetworkAccessManager* m_NetworkManager;
};


