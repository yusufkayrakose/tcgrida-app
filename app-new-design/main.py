################################################################################
##
## BY: WANDERSON M.PIMENTA
## PROJECT MADE WITH: Qt Designer and PySide2
## V: 1.0.0
##
## This project can be used freely for all uses, as long as they maintain the
## respective credits only in the Python scripts, any information in the visual
## interface (GUI) can be modified without any implication.
##
## There are limitations on Qt licenses if you want to use your products
## commercially, I recommend reading them on the official website:
## https://doc.qt.io/qtforpython/licenses.html
##
################################################################################

import sys
import platform
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import (QCoreApplication, QPropertyAnimation, QDate, QDateTime, QMetaObject, QObject, QPoint, QRect,
                          QSize, QTime, QUrl, Qt, QEvent, QTimer)
from PyQt5.QtGui import (QBrush, QColor, QConicalGradient, QCursor, QFont, QFontDatabase, QIcon, QKeySequence, QLinearGradient, QPalette, QPainter, QPixmap, QRadialGradient)
from PyQt5.QtWidgets import *

# GUI FILE
from app_modules import *

class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        print('System: ' + platform.system())
        print('Version: ' +platform.release())

        ## START TIMER (TO UPDATE FRAMES)
        self.ui.page_home.timer = QTimer()
        self.ui.page_home.timer.timeout.connect(self.nextFrameSlot)

        ## START CAPTURING PC CAMERA
        self.openCamPC()

        ########################################################################
        ## START - WINDOW ATTRIBUTES
        ########################################################################

        ## REMOVE ==> STANDARD TITLE BAR
        UIFunctions.removeTitleBar(True)

        ## SET ==> WINDOW TITLE
        self.setWindowTitle('tcGrida App')
        UIFunctions.labelTitle(self, 'tcGrida App')
        UIFunctions.labelDescription(self, 'Your underwater henchman.')

        ## WINDOW SIZE ==> DEFAULT SIZE
        startSize = QSize(1000, 720)
        #self.resize(startSize)
        self.setMinimumSize(startSize)
        # UIFunctions.enableMaximumSize(self, 500, 720)

        ## ==> CREATE MENUS
        ########################################################################

        ## ==> TOGGLE MENU SIZE
        self.ui.btn_toggle_menu.clicked.connect(lambda: UIFunctions.toggleMenu(self, 220, True))

        ## ==> ADD CUSTOM MENUS
        self.ui.stackedWidget.setMinimumWidth(20)
        UIFunctions.addNewMenu(self, "HOME", "btn_home", "url(:/16x16/icons/16x16/cil-home.png)", True)
        UIFunctions.addNewMenu(self, "INFO PAGE", "btn_new_user", "url(:/16x16/icons/16x16/cil-align-center.png)", True)
        UIFunctions.addNewMenu(self, "SETTINGS", "btn_settings", "url(:/16x16/icons/16x16/cil-settings.png)", True)
        UIFunctions.addNewMenu(self, "FOLLOW US!", "btn_widgets", "url(:/16x16/icons/16x16/cil-thumb-up.png)", False)

        # START MENU => SELECTION
        UIFunctions.selectStandardMenu(self, "btn_home")

        ## ==> START PAGE
        self.ui.stackedWidget.setCurrentWidget(self.ui.page_home)

        ## Recording button ==> SHOW HIDE
        self.recordingButton()

        ## ==> MOVE WINDOW / MAXIMIZE / RESTORE
        ########################################################################
        def moveWindow(event):
            # IF MAXIMIZED CHANGE TO NORMAL
            if UIFunctions.returStatus(self) == 1:
                UIFunctions.maximize_restore(self)

            # MOVE WINDOW
            if event.buttons() == Qt.LeftButton:
                try:
                    self.move(self.pos() + event.globalPos() - self.dragPos)
                    self.dragPos = event.globalPos()
                    event.accept()
                except:
                    pass

        # WIDGET TO MOVE
        self.ui.frame_label_top_btns.mouseMoveEvent = moveWindow

        ## ==> LOAD DEFINITIONS
        ########################################################################
        UIFunctions.uiDefinitions(self)

        ########################################################################
        ## END - WINDOW ATTRIBUTES
        ############################## ---/--/--- ##############################

        ## SHOW ==> MAIN WINDOW
        self.showMaximized()

    ########################################################################
    ## MENUS ==> DYNAMIC MENUS FUNCTIONS
    ########################################################################
    def Button(self):
        # GET BT CLICKED
        btnWidget = self.sender()

        # PAGE HOME
        if btnWidget.objectName() == "btn_home":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_home)
            UIFunctions.resetStyle(self, "btn_home")
            UIFunctions.labelPage(self, "Home")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        # PAGE INFO
        if btnWidget.objectName() == "btn_new_user":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_home) #TODO: burası self.ui.page_info yapılacak
            UIFunctions.resetStyle(self, "btn_new_user")
            UIFunctions.labelPage(self, "INFO PAGE")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

        # PAGE WIDGETS 
        if btnWidget.objectName() == "btn_widgets":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_widgets)
            UIFunctions.resetStyle(self, "btn_widgets")
            UIFunctions.labelPage(self, "FOLLOW US!")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))
        
        # PAGE SETTINGS
        if btnWidget.objectName() == "btn_settings":
            self.ui.stackedWidget.setCurrentWidget(self.ui.page_widgets)
            UIFunctions.resetStyle(self, "btn_settings")
            UIFunctions.labelPage(self, "SETTINGS")
            btnWidget.setStyleSheet(UIFunctions.selectMenu(btnWidget.styleSheet()))

    def openCamPC(self):
        self.ui.page_home.vc = cv2.VideoCapture(0)
        # vc.set(5, 30)  #set FPS
        self.ui.page_home.vc.set(3, self.ui.page_home.width()*2)  # set width
        self.ui.page_home.vc.set(4, self.ui.page_home.height()*2)  # set height
        self.ui.page_home.timer.start(round(1000. / 24))

    #def openCamUSB(self):
        #self.vc = cv2.VideoCapture(1)
        # vc.set(5, 30)  #set FPS
        #self.vc.set(3, 300)  # set width
        #self.vc.set(4, 400)  # set height
        #self.timer.start(round(1000. / 24))

    # https://stackoverflow.com/questions/41103148/capture-webcam-video-using-pyqt
    def nextFrameSlot(self):
        rval, frame = self.ui.page_home.vc.read()
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
       # resize = cv2.cv2.resize(frame, (self.ui.page_home.width(), self.ui.page_home.height()))
       # image = QImage(resize, resize.shape[1], resize.shape[0], QImage.Format_RGB888)
        image = QImage(frame, frame.shape[1], frame.shape[0], QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(image)
       # logo_pixmap = cv2.cv2.imread('logo.png')
       # dst = cv2.addWeighted(frame,1.0,logo_pixmap,0.7,0) #bu satır düzeltilecek

        self.ui.page_home.label.setPixmap(pixmap)

    # TODO: THIS BUTTON SHOULD ACTUALLY RECORD SOMETHING
    def recordingButton(self):
        print("hello, im here to record videos")
    ## ==> END ##

if __name__ == "__main__":
    app = QApplication(sys.argv)
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeui.ttf')
    QtGui.QFontDatabase.addApplicationFont('fonts/segoeuib.ttf')
    window = MainWindow()
    sys.exit(app.exec_())
