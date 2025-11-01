import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page {
    id: autoStartRoot
    title: qsTr("กำลังเตรียมการสตรีม")

    Rectangle{
        anchors.fill: parent
        color: "#2E2E2E"

        ColumnLayout{
            anchors.centerIn: parent
            width: 300
            spacing: 20

            Image {
                id: img_01
                source: "qrc:/res/question_mark.svg"
                sourceSize.width: 100
                sourceSize.height: 100
                Layout.alignment: Qt.AlignHCenter
            }

            Label{
                id: statusLabel
                text: qsTr("กำลังเพิ่ม PC และดาวน์โหลดรายการเกม...")
                color: "white"
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize: 18
            }

            Label{
                id: errorLabel
                text: ""
                color: "#FF5555"
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize: 14
                wrapMode: Text.WordWrap
            }
            Button{
                id: cancelButton
                text: qsTr("ยกเลิก")
                Layout.fillWidth: true
                onClicked: {
                    stackView.replace(Qt.resolvedUrl("PcView.qml"))
                }
            }
        }
    }

    property string vmIp: ""
    property string appName: ""
    property var targetComputer: null

    Component{
        id: streamSegueComponent
        StreamSegue{

        }
    }

    Component.onCompleted: {
        searchComputerAndStartStream()
    }

    function searchComputerAndStartStream(){
        targetComputer = ComputerManager.findComputerByIp(vmIp)

        if (targetComputer){
            console.log("AutoStart: พบ Computer Object แล้ว:", targetComputer.name)
            statusLabel.text = qsTr("PC ถูกเพิ่มแล้ว, กำลังรอรายการเกม...")
            appListWaiter.start()
        } else {
            console.warn("AutoStart: ยังไม่พบ Computer Object (ต้องรอ ComputerManager สร้างเสร็จ)")
            statusLabel.text = qsTr("กำลังรอ PC ถูกเพิ่มเข้าระบบ...")
            computerWaiter.start()
        }
    }

    Timer {
        id: computerWaiter
        interval: 1500
        repeat: true

        onTriggered: {
            targetComputer = ComputerManager.findComputerByIp(vmIp)
            if (targetComputer) {
                computerWaiter.stop()
                searchComputerAndStartStream()
            }
        }
    }

    Timer {
        id: appListWaiter
        interval: 2000
        repeat: true

        onTriggered: {
            if (targetComputer.appListReady) {
                appListWaiter.stop()
                statusLabel.text = qsTr("ค้นหาเกม:") + appName + qsTr("...")
                var appToStream = targetComputer.findAppByName(appName)

                if (appToStream){
                    console.log("AutoStart: พบ App แล้ว! เตรียมเริ่มสตรีม...")
                    statusLabel.text = qsTr("พบเกมแล้ว, เริ่มสตรีม...")

                    StackView.replace(streamSegueComponent, {
                        computer: targetComputer,
                        app: appToStream
                    })
                }else {
                    statusLabel.text = qsTr("❌ ผิดพลาด: ไม่พบเกมชื่อ ") + appName + qsTr(" ในรายการ!")
                    errorLabel.text = qsTr("โปรดตรวจสอบรายชื่อ App ใน GeForce Experience / Sunshine")
                    cancelButton.text = qsTr("กลับไปหน้าเลือก PC")
                }
            }
        }
    }
}
