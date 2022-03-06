# Libraries

import sys

import pandas as pd

import os

from PyQt5.QtCore import Qt
from PyQt5 import QtWidgets
from PyQt5.QtGui import QPixmap, QFont

import numpy as np

import matplotlib

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

matplotlib.use('Qt5Agg')


# Classes

class MplCanvas(FigureCanvasQTAgg):

    def __init__(self, parent=None, width=10, height=10, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(MplCanvas, self).__init__(fig)


class Qtplotter(QtWidgets.QMainWindow):

    def __init__(self, unit: str, *args, **kwargs):
        super(Qtplotter, self).__init__(*args, **kwargs)

        self.canvas = MplCanvas(self, width=5, height=4, dpi=100)

        self.canvas.axes.set_xlabel("Time")
        self.canvas.axes.set_ylabel(unit)

        toolbar = NavigationToolbar(self.canvas, self)

        unwanted_buttons = ['Home', 'Back', 'Forward']
        for x in toolbar.actions():
            if x.text() in unwanted_buttons:
                toolbar.removeAction(x)

        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(toolbar)
        layout.addWidget(self.canvas)

        widget = QtWidgets.QWidget()
        widget.setLayout(layout)
        self.setCentralWidget(widget)
        self.setMinimumSize(1000, 800)

    def update_plot(self, datax: np.array, datay: np.array, legends: np.array):

        xlabel = self.canvas.axes.get_xlabel()
        ylabel = self.canvas.axes.get_ylabel()

        self.canvas.axes.cla()
        self.canvas.axes.plot(datax, datay, label=legends)
        self.canvas.axes.legend(legends)
        self.canvas.axes.set_xlabel(xlabel)
        self.canvas.axes.set_ylabel(ylabel)
        self.canvas.draw()


# App parameters
app = QtWidgets.QApplication(sys.argv)

# Page 1 GUI building
page1 = QtWidgets.QWidget()
page1.setFixedSize(1440, 900)
page1.setWindowTitle("Main menu")


def gotopage1():
    page1.show()
    page2.close()


# Page 1 components
page1_mainLayout = QtWidgets.QVBoxLayout()
page1_pushButtonLayout = QtWidgets.QHBoxLayout()
page1_bottomLayout = QtWidgets.QVBoxLayout()
page1_progressBar = QtWidgets.QProgressBar()
page1_filePushButton = QtWidgets.QPushButton()
page1_programTitle = QtWidgets.QLabel()
page1_image = QtWidgets.QLabel()
page1_fileSelected = QtWidgets.QLabel(" ")

# Page 1 assembling
pixmap = QPixmap('vegetation.jpg')
page1_image.setPixmap(pixmap)
page1_image.setAlignment(Qt.AlignCenter)

page1_programTitle.setText("Weather data analyser")
# page1_programTitle.setStyleSheet("background-color : gray; color : black;")
page1_programTitle.setAlignment(Qt.AlignCenter)
page1_programTitle.setFont(QFont('Arial', 40))

page1_filePushButton.setText("Choose file")
page1_filePushButton.setFixedWidth(100)
page1_filePushButton.setStyleSheet("background-color: rgb(220,220,220);")

page1_pushButtonLayout.addWidget(page1_filePushButton)
page1_pushButtonLayout.addWidget(page1_fileSelected)
page1_pushButtonLayout.setAlignment(Qt.AlignCenter)

page1_progressBar.setVisible(False)
page1_progressBar.setRange(0, 100)
page1_progressBar.setAlignment(Qt.AlignCenter)
page1_progressBar.setFixedWidth(page1_programTitle.width())

page1_bottomLayout.addWidget(page1_programTitle)
page1_bottomLayout.addSpacing(100)
page1_bottomLayout.addLayout(page1_pushButtonLayout)
page1_bottomLayout.addSpacing(10)
page1_bottomLayout.addWidget(page1_progressBar)
page1_bottomLayout.setAlignment(Qt.AlignCenter | Qt.AlignTop)

page1_mainLayout.addWidget(page1_image)
page1_mainLayout.addLayout(page1_bottomLayout)

page1.setStyleSheet("background-color:white;")
page1.setLayout(page1_mainLayout)

# Page 2 GUI building
page2 = QtWidgets.QWidget()
page2.setFixedSize(600, 600)
page2.setWindowTitle("Graphics")


def gotopage2():
    page2.show()
    page1.close()


page2_mainLayout = QtWidgets.QVBoxLayout()
page2_optionsLayout = QtWidgets.QHBoxLayout()
page2_dateoptionsLayout = QtWidgets.QVBoxLayout()
page2_returnmainmenubutton = QtWidgets.QPushButton("Return")
page2_printgraphicsbutton = QtWidgets.QPushButton("Print")

page2_date_options = QtWidgets.QGroupBox('Dates')
page2_minimumdatelabel = QtWidgets.QLabel('Minimum date')
page2_maximumdatelabel = QtWidgets.QLabel('Maximum date')
page2_minimumdatelabel.setFixedWidth(120)
page2_maximumdatelabel.setFixedWidth(120)
page2_calendarminimumdate = QtWidgets.QDateTimeEdit()
page2_calendarmaximumdate = QtWidgets.QDateTimeEdit()
page2_calendarminimumdate.setFixedWidth(120)
page2_calendarmaximumdate.setFixedWidth(120)

page2_dateoptionsLayout.addWidget(page2_minimumdatelabel)
page2_dateoptionsLayout.addWidget(page2_calendarminimumdate)
page2_dateoptionsLayout.addWidget(page2_maximumdatelabel)
page2_dateoptionsLayout.addWidget(page2_calendarmaximumdate)

page2_date_options.setLayout(page2_dateoptionsLayout)
page2_date_options.setFixedWidth(140)

page2_measurements_layout = QtWidgets.QVBoxLayout()
page2_measurements_options = QtWidgets.QGroupBox('Measurements')
page2_measurements_option1button = QtWidgets.QCheckBox("Light spectrum")
page2_measurements_option2button = QtWidgets.QCheckBox("Light intensity")
page2_measurements_option3button = QtWidgets.QCheckBox("Light cycles")
page2_measurements_option4button = QtWidgets.QCheckBox("Air temperature")
page2_measurements_option5button = QtWidgets.QCheckBox("Ground temperature")
page2_measurements_option6button = QtWidgets.QCheckBox("Air humidity")
page2_measurements_option7button = QtWidgets.QCheckBox("Wind speed")

page2_measurements_layout.addWidget(page2_measurements_option1button)
page2_measurements_layout.addWidget(page2_measurements_option2button)
page2_measurements_layout.addWidget(page2_measurements_option3button)
page2_measurements_layout.addWidget(page2_measurements_option4button)
page2_measurements_layout.addWidget(page2_measurements_option5button)
page2_measurements_layout.addWidget(page2_measurements_option6button)
page2_measurements_layout.addWidget(page2_measurements_option7button)

page2_measurements_options.setLayout(page2_measurements_layout)
page2_measurements_options.setFixedSize(150, 200)

page2_optionsLayout.addWidget(page2_measurements_options)
page2_optionsLayout.addWidget(page2_date_options)
page2_optionsLayout.addWidget(page2_printgraphicsbutton)
page2_optionsLayout.addWidget(page2_returnmainmenubutton)

page2_fileinfo = QtWidgets.QGroupBox('File information')
page2_fileinfolayout = QtWidgets.QVBoxLayout()

page2_filedirectorylabel = QtWidgets.QLabel()
page2_filesizelabel = QtWidgets.QLabel('Size: ')
page2_fileminimumdate = QtWidgets.QLabel('Minimum date: ')
page2_filemaximumdate = QtWidgets.QLabel('Maximum date: ')
page2_nbrofmeasurements = QtWidgets.QLabel('Number of measurements:')

page2_fileinfolayout.addWidget(page2_filedirectorylabel)
page2_fileinfolayout.addWidget(page2_filesizelabel)
page2_fileinfolayout.addWidget(page2_fileminimumdate)
page2_fileinfolayout.addWidget(page2_filemaximumdate)
page2_fileinfolayout.addWidget(page2_nbrofmeasurements)

page2_fileinfo.setLayout(page2_fileinfolayout)
page2_fileinfo.setFixedSize(600, 200)
page2_mainLayout.addLayout(page2_optionsLayout)
page2_mainLayout.addWidget(page2_fileinfo)

page2.setLayout(page2_mainLayout)

# Graphics pages
optionschecked = np.zeros(8)


def checkmeasurementsoptions():
    if page2_measurements_option1button.isChecked():
        optionschecked[1] = 1
    else:
        optionschecked[1] = 0
    if page2_measurements_option2button.isChecked():
        optionschecked[2] = 1
    else:
        optionschecked[2] = 0
    if page2_measurements_option3button.isChecked():
        optionschecked[3] = 1
    else:
        optionschecked[3] = 0
    if page2_measurements_option4button.isChecked():
        optionschecked[4] = 1
    else:
        optionschecked[4] = 0
    if page2_measurements_option5button.isChecked():
        optionschecked[5] = 1
    else:
        optionschecked[5] = 0
    if page2_measurements_option6button.isChecked():
        optionschecked[6] = 1
    else:
        optionschecked[6] = 0
    if page2_measurements_option7button.isChecked():
        optionschecked[7] = 1
    else:
        optionschecked[7] = 0


lightspectrumgraphicpage = Qtplotter('Unit')
lightspectrumgraphicpage.setWindowTitle("Light spectrum")

lightintensitygraphicpage = Qtplotter('Lux')
lightintensitygraphicpage.setWindowTitle("Light intensity")

lightcyclesgraphicpage = Qtplotter('bnr of hours of sun,')
lightcyclesgraphicpage.setWindowTitle("Light cycles")

airtemperaturegraphicpage = Qtplotter('Celsius')
airtemperaturegraphicpage.setWindowTitle("Air temperature")

groundtemperaturegraphicpage = Qtplotter('Celsius')
groundtemperaturegraphicpage.setWindowTitle("Ground temperature")

airhumiditygraphicpage = Qtplotter('%rel')
airhumiditygraphicpage.setWindowTitle("Air humidity")

windspeedgraphicpage = Qtplotter('m/s')
windspeedgraphicpage.setWindowTitle("Wind speed")


def printgraphics():
    dfs = pd.read_excel('DataBase.xlsx', index_col=None, sheet_name=None)

    datemin = pd.Timestamp(page2_calendarminimumdate.date().year(), page2_calendarminimumdate.date().month(),
                           page2_calendarminimumdate.date().day(), page2_calendarminimumdate.time().hour(),
                           page2_calendarminimumdate.time().minute())

    datemax = pd.Timestamp(page2_calendarmaximumdate.date().year(), page2_calendarmaximumdate.date().month(),
                           page2_calendarmaximumdate.date().day(), page2_calendarmaximumdate.time().hour(),
                           page2_calendarmaximumdate.time().minute())

    alldates = np.array(dfs['Light']['Datetime'])

    sortedmindate = alldates > datemin
    sortedmaxdate = alldates < datemax
    sorteddates = np.logical_and(sortedmindate, sortedmaxdate)

    numberofdates = np.count_nonzero(sorteddates)
    selecteddates = alldates[sorteddates]

    if numberofdates >= 1:

        if optionschecked[1] == 1:
            lightspectrumdatay = np.array(dfs['Light'][['CH1', 'CH2', 'CH3', 'CH4', 'CH5', 'CH6']])
            lightspectrumdataysorted = lightspectrumdatay[sorteddates]

            lightspectrumgraphicpage.update_plot(selecteddates, lightspectrumdataysorted,
                                                 ['CH1', 'CH2', 'CH3', 'CH4', 'CH5', 'CH6'])
            lightspectrumgraphicpage.show()

        if optionschecked[2] == 1:
            lightintensitygraphicpage.show()
        if optionschecked[3] == 1:
            lightcyclesgraphicpage.show()
        if optionschecked[4] == 1:
            airtemperaturedatay = np.array(dfs['Air_temp_%RH'][['Temp_degC']])
            airtemperaturedataysorted = airtemperaturedatay[sorteddates]

            airtemperaturegraphicpage.update_plot(selecteddates, airtemperaturedataysorted, ['Temp_degC'])
            airtemperaturegraphicpage.show()

        if optionschecked[5] == 1:
            groundtemperaturedatay = np.array(dfs['Soil_temp'][['Temp']])
            groundtemperaturedataysorted = groundtemperaturedatay[sorteddates]

            groundtemperaturegraphicpage.update_plot(selecteddates, groundtemperaturedataysorted, ['ground temp'])
            groundtemperaturegraphicpage.show()

        if optionschecked[6] == 1:
            airhumiditydatay = np.array(dfs['Air_temp_%RH'][['%RH']])
            airhumiditydataysorted = airhumiditydatay[sorteddates]

            airhumiditygraphicpage.update_plot(selecteddates, airhumiditydataysorted, ['%RH'])
            airhumiditygraphicpage.show()

        if optionschecked[7] == 1:
            windspeeddatay = np.array(dfs['Wind_speed'][['Wind_speed']])
            windspeeddataysorted = windspeeddatay[sorteddates]

            windspeedgraphicpage.update_plot(selecteddates, windspeeddataysorted, ['wind speed'])
            windspeedgraphicpage.show()


def filedirectory():
    filewindow = QtWidgets.QFileDialog()
    filewindow.setNameFilter("*.jpg *.txt")
    isvalid = filewindow.exec()

    if isvalid:
        selectedfile = filewindow.selectedUrls()[0]
        page1_fileSelected.setText(selectedfile.fileName())
        page1_progressBar.setVisible(True)
        page1_progressBar.setValue(0)
        page2_filedirectorylabel.setText('Directory: ' + selectedfile.path())

        # getting file size
        path = selectedfile.path()
        harddrivedirectory = path.find(':') + 1
        path = path[harddrivedirectory:]
        size = os.path.getsize(path)

        size = size / 1000

        # Création du fichier excel (fonction alexis)
        dfs = pd.read_excel('DataBase.xlsx', index_col=None, sheet_name=None)

        dates = dfs['Light']['Datetime']

        minimumdate = dates[dates.index.min()]
        maximumdate = dates[dates.index.max()]
        numerofelements = dates.size

        page2_filesizelabel.setText("File size: " + str(size) + 'KB')
        page2_fileminimumdate.setText('Minimum Date: ' + str(minimumdate))
        page2_filemaximumdate.setText('Maximum Date: ' + str(maximumdate))
        page2_nbrofmeasurements.setText('Number of measurments: ' + str(numerofelements))

        page2_calendarminimumdate.setMinimumDate(minimumdate)
        page2_calendarminimumdate.setMaximumDate(maximumdate)

        page2_calendarmaximumdate.setMinimumDate(minimumdate)
        page2_calendarmaximumdate.setMaximumDate(maximumdate)

        gotopage2()


# Connections

page1_filePushButton.clicked.connect(filedirectory)

page2_measurements_option1button.clicked.connect(checkmeasurementsoptions)
page2_measurements_option2button.clicked.connect(checkmeasurementsoptions)
page2_measurements_option3button.clicked.connect(checkmeasurementsoptions)
page2_measurements_option4button.clicked.connect(checkmeasurementsoptions)
page2_measurements_option5button.clicked.connect(checkmeasurementsoptions)
page2_measurements_option6button.clicked.connect(checkmeasurementsoptions)
page2_measurements_option7button.clicked.connect(checkmeasurementsoptions)

page2_returnmainmenubutton.clicked.connect(gotopage1)
page2_printgraphicsbutton.clicked.connect(printgraphics)

# Main

gotopage1()

exit(app.exec_())
