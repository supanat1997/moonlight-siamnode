#include "AuthManager.h"

#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QSslError>

AuthManager::AuthManager(QObject *parent) : QObject(parent)
{
    m_NetworkManager = new QNetworkAccessManager(this);

    // เชื่อมต่อสัญญาณ error พื้นฐานครั้งเดียวพอ
    connect(m_NetworkManager, &QNetworkAccessManager::sslErrors,
            this, [](QNetworkReply* reply, const QList<QSslError>& errors) {
                QStringList msgs;
                for (const auto& e : errors) msgs << e.errorString();
                qWarning() << "[AuthManager] SSL errors:" << msgs.join("; ");
                // reply->ignoreSslErrors(); // ใช้เฉพาะตอน dev เท่านั้น
            });
}

void AuthManager::login(QString username, QString password)
{
    qDebug() << "[AuthManager] login(): เริ่มยิง API ของผู้ใช้:" << username;

    // เตรียม payload JSON
    QJsonObject jsonPayload{
        {"username", username},
        {"password", password}
    };
    QJsonDocument jsonDoc(jsonPayload);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    QUrl apiUrl("http://localhost:3000/v1/loginAndGetVM");
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // ยิง API ออกไป
    QNetworkReply* reply = m_NetworkManager->post(request, jsonData);
    if (!reply) {
        qCritical() << "[AuthManager] ERROR: QNetworkReply เป็น nullptr";
        emit loginFailed(tr("ไม่สามารถสร้างคำขอได้"));
        return;
    }

    // ใช้ lambda แทน slot แยก เพื่อไม่ connect ซ้ำหลายรอบ
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> guard(reply);

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "[AuthManager] Network error:" << reply->errorString();
            emit loginFailed(tr("เชื่อมต่อ Server ไม่สำเร็จ: ") + reply->errorString());
            return;
        }

        QByteArray responseData = reply->readAll();
        if (responseData.isEmpty()) {
            qWarning() << "[AuthManager] Response ว่างเปล่า";
            emit loginFailed(tr("Server ไม่ได้ส่งข้อมูลกลับมา"));
            return;
        }

        QJsonParseError parseErr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &parseErr);
        if (parseErr.error != QJsonParseError::NoError) {
            qWarning() << "[AuthManager] JSON parse error:" << parseErr.errorString()
            << "Body:" << QString::fromUtf8(responseData.left(256));
            emit loginFailed(tr("ข้อมูล JSON ไม่ถูกต้อง"));
            return;
        }

        if (!jsonDoc.isObject()) {
            qWarning() << "[AuthManager] JSON root ไม่ใช่ object";
            emit loginFailed(tr("รูปแบบข้อมูลไม่ถูกต้อง"));
            return;
        }

        QJsonObject jsonResponse = jsonDoc.object();
        QString realVmIp = jsonResponse.value("vm_ip").toString();
        QString realAppName = jsonResponse.value("app_name").toString();

        if (realVmIp.isEmpty() || realAppName.isEmpty()) {
            QString errMsg = jsonResponse.value("Error").toString();
            qWarning() << "[AuthManager] ล็อกอินล้มเหลว (API):" << errMsg;
            emit loginFailed(tr("รหัสผ่านไม่ถูกต้อง หรือ ไม่พบ VM"));
            return;
        }

        qDebug() << "[AuthManager] ล็อกอินสำเร็จ! IP:" << realVmIp
                 << "App:" << realAppName;
        emit loginSuccess(realVmIp, realAppName);
    });
}


