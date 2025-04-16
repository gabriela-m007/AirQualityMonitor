/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_Main;
    QWidget *topButtonWidget;
    QHBoxLayout *horizontalLayout_TopButtons;
    QPushButton *fetchStationsButton;
    QPushButton *loadStationsButton;
    QPushButton *saveStationsButton;
    QSpacerItem *horizontalSpacer_Top;
    QSplitter *mainSplitter;
    QWidget *leftPaneWidget;
    QVBoxLayout *verticalLayout_Left;
    QLabel *label_Stations;
    QListWidget *stationsListWidget;
    QLabel *label_Sensors;
    QListWidget *sensorsListWidget;
    QPushButton *loadSensorDataButton;
    QPushButton *saveSensorDataButton;
    QWidget *rightPaneWidget;
    QVBoxLayout *verticalLayout_Right;
    QGroupBox *aqiGroupBox;
    QVBoxLayout *verticalLayout_AQI;
    QLabel *aqiStationLabel;
    QLabel *aqiCalcDateLabel;
    QLabel *aqiOverallLabel;
    QLabel *aqiPM10Label;
    QLabel *aqiPM25Label;
    QLabel *aqiO3Label;
    QLabel *aqiNO2Label;
    QLabel *aqiSO2Label;
    QLabel *aqiCOLabel;
    QLabel *aqiC6H6Label;
    QGroupBox *chartGroupBox;
    QVBoxLayout *chartLayout;
    QPushButton *analyzeButton;
    QGroupBox *analysisGroupBox;
    QVBoxLayout *verticalLayout_Analysis;
    QLabel *analysisMinLabel;
    QLabel *analysisMaxLabel;
    QLabel *analysisAvgLabel;
    QLabel *analysisTrendLabel;
    QSpacerItem *verticalSpacer_Right;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1024, 768);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_Main = new QVBoxLayout(centralwidget);
        verticalLayout_Main->setObjectName("verticalLayout_Main");
        topButtonWidget = new QWidget(centralwidget);
        topButtonWidget->setObjectName("topButtonWidget");
        horizontalLayout_TopButtons = new QHBoxLayout(topButtonWidget);
        horizontalLayout_TopButtons->setObjectName("horizontalLayout_TopButtons");
        fetchStationsButton = new QPushButton(topButtonWidget);
        fetchStationsButton->setObjectName("fetchStationsButton");

        horizontalLayout_TopButtons->addWidget(fetchStationsButton);

        loadStationsButton = new QPushButton(topButtonWidget);
        loadStationsButton->setObjectName("loadStationsButton");

        horizontalLayout_TopButtons->addWidget(loadStationsButton);

        saveStationsButton = new QPushButton(topButtonWidget);
        saveStationsButton->setObjectName("saveStationsButton");

        horizontalLayout_TopButtons->addWidget(saveStationsButton);

        horizontalSpacer_Top = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_TopButtons->addItem(horizontalSpacer_Top);


        verticalLayout_Main->addWidget(topButtonWidget);

        mainSplitter = new QSplitter(centralwidget);
        mainSplitter->setObjectName("mainSplitter");
        mainSplitter->setOrientation(Qt::Horizontal);
        leftPaneWidget = new QWidget(mainSplitter);
        leftPaneWidget->setObjectName("leftPaneWidget");
        verticalLayout_Left = new QVBoxLayout(leftPaneWidget);
        verticalLayout_Left->setObjectName("verticalLayout_Left");
        label_Stations = new QLabel(leftPaneWidget);
        label_Stations->setObjectName("label_Stations");

        verticalLayout_Left->addWidget(label_Stations);

        stationsListWidget = new QListWidget(leftPaneWidget);
        stationsListWidget->setObjectName("stationsListWidget");

        verticalLayout_Left->addWidget(stationsListWidget);

        label_Sensors = new QLabel(leftPaneWidget);
        label_Sensors->setObjectName("label_Sensors");

        verticalLayout_Left->addWidget(label_Sensors);

        sensorsListWidget = new QListWidget(leftPaneWidget);
        sensorsListWidget->setObjectName("sensorsListWidget");

        verticalLayout_Left->addWidget(sensorsListWidget);

        loadSensorDataButton = new QPushButton(leftPaneWidget);
        loadSensorDataButton->setObjectName("loadSensorDataButton");

        verticalLayout_Left->addWidget(loadSensorDataButton);

        saveSensorDataButton = new QPushButton(leftPaneWidget);
        saveSensorDataButton->setObjectName("saveSensorDataButton");

        verticalLayout_Left->addWidget(saveSensorDataButton);

        mainSplitter->addWidget(leftPaneWidget);
        rightPaneWidget = new QWidget(mainSplitter);
        rightPaneWidget->setObjectName("rightPaneWidget");
        verticalLayout_Right = new QVBoxLayout(rightPaneWidget);
        verticalLayout_Right->setObjectName("verticalLayout_Right");
        aqiGroupBox = new QGroupBox(rightPaneWidget);
        aqiGroupBox->setObjectName("aqiGroupBox");
        verticalLayout_AQI = new QVBoxLayout(aqiGroupBox);
        verticalLayout_AQI->setObjectName("verticalLayout_AQI");
        aqiStationLabel = new QLabel(aqiGroupBox);
        aqiStationLabel->setObjectName("aqiStationLabel");

        verticalLayout_AQI->addWidget(aqiStationLabel);

        aqiCalcDateLabel = new QLabel(aqiGroupBox);
        aqiCalcDateLabel->setObjectName("aqiCalcDateLabel");

        verticalLayout_AQI->addWidget(aqiCalcDateLabel);

        aqiOverallLabel = new QLabel(aqiGroupBox);
        aqiOverallLabel->setObjectName("aqiOverallLabel");
        QFont font;
        aqiOverallLabel->setFont(font);

        verticalLayout_AQI->addWidget(aqiOverallLabel);

        aqiPM10Label = new QLabel(aqiGroupBox);
        aqiPM10Label->setObjectName("aqiPM10Label");

        verticalLayout_AQI->addWidget(aqiPM10Label);

        aqiPM25Label = new QLabel(aqiGroupBox);
        aqiPM25Label->setObjectName("aqiPM25Label");

        verticalLayout_AQI->addWidget(aqiPM25Label);

        aqiO3Label = new QLabel(aqiGroupBox);
        aqiO3Label->setObjectName("aqiO3Label");

        verticalLayout_AQI->addWidget(aqiO3Label);

        aqiNO2Label = new QLabel(aqiGroupBox);
        aqiNO2Label->setObjectName("aqiNO2Label");

        verticalLayout_AQI->addWidget(aqiNO2Label);

        aqiSO2Label = new QLabel(aqiGroupBox);
        aqiSO2Label->setObjectName("aqiSO2Label");

        verticalLayout_AQI->addWidget(aqiSO2Label);

        aqiCOLabel = new QLabel(aqiGroupBox);
        aqiCOLabel->setObjectName("aqiCOLabel");

        verticalLayout_AQI->addWidget(aqiCOLabel);

        aqiC6H6Label = new QLabel(aqiGroupBox);
        aqiC6H6Label->setObjectName("aqiC6H6Label");

        verticalLayout_AQI->addWidget(aqiC6H6Label);


        verticalLayout_Right->addWidget(aqiGroupBox);

        chartGroupBox = new QGroupBox(rightPaneWidget);
        chartGroupBox->setObjectName("chartGroupBox");
        chartLayout = new QVBoxLayout(chartGroupBox);
        chartLayout->setObjectName("chartLayout");

        verticalLayout_Right->addWidget(chartGroupBox);

        analyzeButton = new QPushButton(rightPaneWidget);
        analyzeButton->setObjectName("analyzeButton");

        verticalLayout_Right->addWidget(analyzeButton);

        analysisGroupBox = new QGroupBox(rightPaneWidget);
        analysisGroupBox->setObjectName("analysisGroupBox");
        verticalLayout_Analysis = new QVBoxLayout(analysisGroupBox);
        verticalLayout_Analysis->setObjectName("verticalLayout_Analysis");
        analysisMinLabel = new QLabel(analysisGroupBox);
        analysisMinLabel->setObjectName("analysisMinLabel");

        verticalLayout_Analysis->addWidget(analysisMinLabel);

        analysisMaxLabel = new QLabel(analysisGroupBox);
        analysisMaxLabel->setObjectName("analysisMaxLabel");

        verticalLayout_Analysis->addWidget(analysisMaxLabel);

        analysisAvgLabel = new QLabel(analysisGroupBox);
        analysisAvgLabel->setObjectName("analysisAvgLabel");

        verticalLayout_Analysis->addWidget(analysisAvgLabel);

        analysisTrendLabel = new QLabel(analysisGroupBox);
        analysisTrendLabel->setObjectName("analysisTrendLabel");

        verticalLayout_Analysis->addWidget(analysisTrendLabel);


        verticalLayout_Right->addWidget(analysisGroupBox);

        verticalSpacer_Right = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_Right->addItem(verticalSpacer_Right);

        mainSplitter->addWidget(rightPaneWidget);

        verticalLayout_Main->addWidget(mainSplitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1024, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Monitor Jako\305\233ci Powietrza", nullptr));
        fetchStationsButton->setText(QCoreApplication::translate("MainWindow", "Pobierz Stacje (API)", nullptr));
        loadStationsButton->setText(QCoreApplication::translate("MainWindow", "Wczytaj Stacje (Plik)", nullptr));
        saveStationsButton->setText(QCoreApplication::translate("MainWindow", "Zapisz Stacje (Plik)", nullptr));
        label_Stations->setText(QCoreApplication::translate("MainWindow", "Stacje Pomiarowe:", nullptr));
        label_Sensors->setText(QCoreApplication::translate("MainWindow", "Czujniki (Stanowiska):", nullptr));
        loadSensorDataButton->setText(QCoreApplication::translate("MainWindow", "Wczytaj Dane Czujnika", nullptr));
        saveSensorDataButton->setText(QCoreApplication::translate("MainWindow", "Zapisz Dane Czujnika", nullptr));
        aqiGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Indeks Jako\305\233ci Powietrza", nullptr));
        aqiStationLabel->setText(QCoreApplication::translate("MainWindow", "Indeks dla:", nullptr));
        aqiCalcDateLabel->setText(QCoreApplication::translate("MainWindow", "Obliczony:", nullptr));
        aqiOverallLabel->setText(QCoreApplication::translate("MainWindow", "Og\303\263lny:", nullptr));
        aqiPM10Label->setText(QCoreApplication::translate("MainWindow", "PM10:", nullptr));
        aqiPM25Label->setText(QCoreApplication::translate("MainWindow", "PM2.5:", nullptr));
        aqiO3Label->setText(QCoreApplication::translate("MainWindow", "O3:", nullptr));
        aqiNO2Label->setText(QCoreApplication::translate("MainWindow", "NO2:", nullptr));
        aqiSO2Label->setText(QCoreApplication::translate("MainWindow", "SO2:", nullptr));
        aqiCOLabel->setText(QCoreApplication::translate("MainWindow", "CO:", nullptr));
        aqiC6H6Label->setText(QCoreApplication::translate("MainWindow", "C6H6:", nullptr));
        chartGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Wykres Danych Pomiarowych", nullptr));
        analyzeButton->setText(QCoreApplication::translate("MainWindow", "Analizuj Dane Wykresu", nullptr));
        analysisGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Analiza Danych", nullptr));
        analysisMinLabel->setText(QCoreApplication::translate("MainWindow", "Min: -", nullptr));
        analysisMaxLabel->setText(QCoreApplication::translate("MainWindow", "Max: -", nullptr));
        analysisAvgLabel->setText(QCoreApplication::translate("MainWindow", "\305\232rednia: -", nullptr));
        analysisTrendLabel->setText(QCoreApplication::translate("MainWindow", "Trend: -", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
