#include "AuthManager.h"

#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

AuthManager::AuthManager(QObject *parent) : QObject(parent)
{
    m_NetworkManager = new QNetworkAccessManager(this);
}

void AuthManager::login(QString username, QString password)
{
    qDebug() << "AuthManager: ได้รับคำสั่ง login จากผู้ใช้:" << username;

    QJsonObject jsonPayload;
    jsonPayload["username"] = username;
    jsonPayload["password"] = password;
    QJsonDocument jsonDoc(jsonPayload);
    QByteArray jsonData = jsonDoc.toJson();

    QUrl apiUrl("http://localhost:3000/v1/loginAndGetVM");
    QNetworkRequest request(apiUrl);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    connect(m_NetworkManager, &QNetworkAccessManager::finished, this, &AuthManager::onLoginReply);

    qDebug() << "AuthManager: กำลังยิง API ไปที่" << apiUrl.toString();
    QNetworkReply* reply = m_NetworkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished,
            this, &AuthManager::onLoginReply);
}

void AuthManager::onLoginReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() !=QNetworkReply::NoError){

        qWarning() << "AuthManager: ล็อกอินล้มเหลว (Network):" << reply->errorString();
        emit loginFailed(tr("ไม่สามารถเชื่อมต่อ Server ได้:") + reply->errorString());

    } else {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonResponse = jsonDoc.object();

        QString realVmIp = jsonResponse["vm_ip"].toString();
        QString realAppName = jsonResponse["app_name"].toString();

        if (realVmIp.isEmpty() || realAppName.isEmpty()){
            qWarning() << "AuthManager: ล็อกอินล้มเหลว (API):" << jsonResponse["Error"].toString();
            emit loginFailed(tr("รหัสผ่านไม่ถูกต้อง หรือ ไม่พบ VM"));
        } else {
            qDebug() << "AuthManager: ล็อกอินสำเร็จ (API)! ส่ง IP:" << realVmIp << "และ App:" << realAppName;
            emit loginSuccess(realVmIp, realAppName);
        }
    }
    reply->deleteLater();
}
