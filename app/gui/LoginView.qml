import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import AuthManager 1.0

Page {
    id: loginRoot
    title: qsTr("เข้าสู่ระบบ")

    Rectangle {
        anchors.fill: parent
        color: "#2E2E2E" // สีพื้นหลัง (สีเทาเข้ม)
    }

    Connections {
        target: AuthManager
        onLoginFailed: {
            loginButton.enabled = true;
            loginButton.text = qsTr("เข้าสู่ระบบ");
            errorLabel.text = "Login Error"
        }
        onLoginSuccess: {
            loginButton.enabled = true;
            loginButton.text = qsTr("เข้าสู่ระบบ");
            errorLabel.text = "Login Completed" + vmIp;
            // (เดี๋ยว main.qml จะดักฟังอันนี้ แล้วเปลี่ยนหน้าเอง)
        }
    }

    // จัดเรียง UI เป็นแนวตั้ง (เหมือนเดิม)
    ColumnLayout {
        anchors.centerIn: parent
        width: 300
        spacing: 5 // (เพิ่มระยะห่างนิดหน่อย)

        // --- โลโก้ ---
        Image {
            id: logo
            // source: "qrc:/res/yourlogo.svg" (เดี๋ยวเรามาเปลี่ยน)
            sourceSize.width: 100
            sourceSize.height: 100
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 15 // เว้นระยะล่าง
            Rectangle { anchors.fill: parent; color: "gray" } // Placeholder ชั่วคราว
        }

        // --- ข้อความ Username ---
        Label {
            text: qsTr("ชื่อผู้ใช้:")
            color: "white"
        }

        // --- ช่องกรอก Username ---
        TextField {
            id: usernameField
            Layout.fillWidth: true
            placeholderText: qsTr("กรอกชื่อผู้ใช้ของคุณ")
            color: "white" // (เพิ่มสีตัวอักษร)
        }

        // --- ข้อความ Password ---
        Label {
            text: qsTr("รหัสผ่าน:")
            color: "white"
            Layout.topMargin: 10
        }

        // --- ช่องกรอก Password ---
        TextField {
            id: passwordField
            Layout.fillWidth: true
            placeholderText: qsTr("กรอกรหัสผ่านของคุณ")
            echoMode: TextInput.Password
            color: "white" // (เพิ่มสีตัวอักษร)
        }

        // --- ปุ่ม Login ---
        Button {
            id: loginButton
            Layout.fillWidth: true
            Layout.topMargin: 20
            text: qsTr("เข้าสู่ระบบ")
            onClicked: {
                loginButton.enabled = false;
                loginButton.text = qsTr("กำลังตรวจสอบ...");
                errorLabel.text = "";
                AuthManager.login(usernameField.text, passwordField.text);
            }
        }

        // --- ข้อความแสดง Error ---
        Label {
            id: errorLabel
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            color: "#FF5555"
            font.pixelSize: 12
            Layout.topMargin: 10
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
