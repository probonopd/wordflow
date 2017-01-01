import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Styles 1.4
import Qt.labs.settings 1.0
import "controls" as Controls

ApplicationWindow {
    id: window
    flags: Qt.Window
    property var vocabularyList: []

    Component.onCompleted: {
        vocabularyImpl.createDatabase()
        if (vocabularyImpl.checkIfVocabularyExist())  {
            window.vocabularyList = vocabularyImpl.listVocabularies()
            vocabularyBox.currentIndex = settings.defaultVocabularyId
            toolBar.state = ""
        }
        splashScreen.item.state = "StopSplash"
        if (!vocabularyImpl.checkIfVocabularyExist()) {
            configurationPopup.open()
        }
    }

    Component.onDestruction: {
        settings.defaultVocabularyId = vocabularyBox.currentIndex
    }

    visible: true
    Material.theme: Material.Dark
    Material.accent: Material.Red
    width:  Screen.desktopAvailableWidth / 3
    height: Screen.desktopAvailableHeight * 2 / 3
    title: qsTr("WordFlow")


    Settings {
        id: settings
        property int defaultVocabularyId: 0
    }

    header: ToolBar {
        id: toolBar
        state: "beforeConfiguration"
        states: [
            State {
                name: "beforeConfiguration"
                PropertyChanges {
                    target: menu
                    visible: false
                }
                PropertyChanges {
                    target: vocabularyBox
                    visible: false
                }
            }
        ]

        ToolButton {
            id: menu
            visible: true
            contentItem: Image {
                fillMode: Image.Pad
                source: "qrc:/images/drawer.png"
                horizontalAlignment: Image.AlignHCenter
                verticalAlignment: Image.AlignVCenter
            }
            onClicked: {
                actionPane.visible = !actionPane.visible
            }
        }

        Label {
            id: titleLabel
            text: "WordFlow"
            font.pixelSize: 20
            anchors.centerIn: parent
        }

        ComboBox {
            id: vocabularyBox
            width: 200
            height: toolBar.height
            model: window.vocabularyList
            textRole: "name"
            anchors.right: parent.right
            background: Rectangle {
                color: "white"
            }
            onCurrentIndexChanged: {
                console.log("current index: " + settings.defaultVocabularyId)
                //TODO connect index changing with auto vocabulary learning change
            }

            Material.foreground: Material.Green
            Material.accent: Material.Green
        }
    }

    Controls.Menu {
        id: actionPane
    }

    Pane {
        id: vocabulary
        width: window.width - actionPane.width
        height: window.height - toolBar.height
        anchors.left: actionPane.right
        states: [
            State {
                name: "wider"
                when: !actionPane.visible
                PropertyChanges {
                    target: vocabulary
                    width: window.width
                    anchors.fill: parent
                }
            },
            State {
                name: "narrower"
                when: actionPane.visible
                PropertyChanges {
                    target: vocabulary
                    width: window.width - actionPane.width
                    anchors.left: actionPane.right
                }
            }
        ]

        StackView {
            id: stackView
            anchors.fill: parent

            initialItem: Pane {
                Label {
                    text: qsTr("Your vocabulary is empty")
                    font.pixelSize: 20
                    anchors.centerIn: parent
                    horizontalAlignment: Label.AlignHCenter
                    verticalAlignment: Label.AlignVCenter
                }
            }
        }
    }

    Controls.ConfigurationPopup {
        id: configurationPopup
        onClosed: {
            window.vocabularyList = vocabularyImpl.listVocabularies()
            toolBar.state = ""
        }
        onOpened: {
            toolBar.state = "beforeConfiguration"
        }
    }

    Loader {
        id: splashScreen
        source: "qrc:/screens/SplashScreen.qml"
        anchors.fill: parent
    }

    //    Drawer {
    //        id: drawer
    //        width: Math.min(window.width, window.height) / 3
    //        height: window.height
    //        visible: true

    //    }
}
