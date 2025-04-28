#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ApiService.h"
#include "DataStorage.h"
#include "DataAnalyzer.h"

#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>
#include <QDateTimeEdit>
#include <QPushButton>
#include <algorithm>
#include <limits>
#include <cmath>

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <QLabel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_apiService(new ApiService(this))
    , m_dataStorage(new DataStorage("."))
    , m_analyzer(new DataAnalyzer())
    , m_chart(new QChart())
    , m_chartView(new QChartView(m_chart))
    , m_series(new QSplineSeries())
    , m_axisX(new QDateTimeAxis)
    , m_axisY(new QValueAxis)
{
    ui->setupUi(this);

    QList<int> initialSizes;
    initialSizes << 380 << 620;
    ui->mainSplitter->setSizes(initialSizes);

    const QString polishDateTimeFormat = "dd.MM.yyyy HH:mm";
    ui->startDateTimeEdit->setDisplayFormat(polishDateTimeFormat);
    ui->endDateTimeEdit->setDisplayFormat(polishDateTimeFormat);

    m_chart->addSeries(m_series);

    m_axisX->setTickCount(10);
    m_axisX->setFormat("yyyy-MM-dd HH:mm");
    m_axisX->setTitleText("Data i Czas");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);

    m_axisY->setLabelFormat("%.1f");
    m_axisY->setTitleText("Wartość");
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->setTitle("Dane Pomiarowe");

    if(ui->chartLayout) {
        ui->chartLayout->insertWidget(2, m_chartView);
    } else {
        qWarning("Plik UI nie zawiera layoutu o nazwie 'chartLayout'. Wykres nie będzie wyświetlany.");
    }
    m_chartView->setRenderHint(QPainter::Antialiasing);

    connect(m_apiService, &ApiService::stationsReady, this, &MainWindow::handleStationsReady);
    connect(m_apiService, &ApiService::sensorsReady, this, &MainWindow::handleSensorsReady);
    connect(m_apiService, &ApiService::sensorDataReady, this, &MainWindow::handleSensorDataReady);
    connect(m_apiService, &ApiService::airQualityIndexReady, this, &MainWindow::handleAirQualityIndexReady);
    connect(m_apiService, &ApiService::networkError, this, &MainWindow::handleNetworkError);

    connect(ui->filterDataButton, &QPushButton::clicked, this, &MainWindow::on_filterDataButton_clicked);
    connect(ui->cityFilterLineEdit, &QLineEdit::textChanged, this, &MainWindow::filterStations);
    connect(ui->clearCityFilterButton, &QPushButton::clicked, this, &MainWindow::on_clearCityFilterButton_clicked);

    ui->saveStationsButton->setEnabled(false);
    ui->sensorsListWidget->setEnabled(false);
    ui->saveSensorDataButton->setEnabled(false);
    ui->loadSensorDataButton->setEnabled(false);
    ui->analyzeButton->setEnabled(false);
    ui->filterDataButton->setEnabled(false);
    ui->startDateTimeEdit->setEnabled(false);
    ui->endDateTimeEdit->setEnabled(false);
    ui->clearCityFilterButton->setEnabled(false);

    qInfo() << "Ścieżka przechowywania danych:" << m_dataStorage->getStoragePath();
    setUiFetchingState(false, false, false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUiFetchingState(bool isFetchingStations, bool isFetchingSensors, bool isFetchingSensorData)
{
    m_isFetchingStations = isFetchingStations;
    m_isFetchingSensors = isFetchingSensors;
    m_isFetchingSensorData = isFetchingSensorData;

    ui->fetchStationsButton->setEnabled(!m_isFetchingStations);
    ui->loadStationsButton->setEnabled(!m_isFetchingStations);

    bool canLoadSensorData = !m_isFetchingSensorData && getSelectedSensorId() > 0;
    ui->loadSensorDataButton->setEnabled(canLoadSensorData);

    if (isFetchingSensorData) {
        ui->analyzeButton->setEnabled(false);
        ui->filterDataButton->setEnabled(false);
        ui->startDateTimeEdit->setEnabled(false);
        ui->endDateTimeEdit->setEnabled(false);
        ui->saveSensorDataButton->setEnabled(false);
    }
}

void MainWindow::on_fetchStationsButton_clicked()
{
    if (m_isFetchingStations) return;

    qDebug() << "Kliknięto przycisk Pobierz Stacje";
    ui->statusbar->showMessage("Pobieranie listy stacji...");
    setUiFetchingState(true, false, false);

    clearSensorDetails();
    clearChart();
    clearAnalysisResults();
    clearAirQualityIndexDisplay();
    ui->stationsListWidget->clear();
    ui->sensorsListWidget->clear();
    ui->sensorsListWidget->setEnabled(false);
    ui->cityFilterLineEdit->clear();

    m_apiService->fetchAllStations();
}

void MainWindow::on_loadStationsButton_clicked()
{
    if (m_isFetchingStations) return;

    qDebug() << "Kliknięto przycisk Wczytaj Stacje";
    ui->statusbar->showMessage("Ładowanie stacji z pliku...");

    clearSensorDetails();
    clearChart();
    clearAnalysisResults();
    clearAirQualityIndexDisplay();
    ui->stationsListWidget->clear();
    ui->sensorsListWidget->clear();
    ui->sensorsListWidget->setEnabled(false);
    ui->cityFilterLineEdit->clear();

    loadStationsFromFile();
}

bool MainWindow::loadStationsFromFile() {
    qDebug() << "Próba wczytania stacji z:" << m_dataStorage->getStoragePath() + "/stations.json";
    m_currentStations = m_dataStorage->loadStationsFromJson();
    if (!m_currentStations.empty()) {
        qDebug() << "Wczytano" << m_currentStations.size() << "stacji z pliku.";
        filterStations(ui->cityFilterLineEdit->text());
        ui->statusbar->showMessage("Stacje załadowane z pliku.", 3000);
        ui->saveStationsButton->setEnabled(true);
        ui->cityFilterLineEdit->setEnabled(true);
        return true;
    } else {
        qWarning() << "Nie udało się wczytać stacji lub plik jest pusty/uszkodzony.";
        ui->statusbar->showMessage("Nie znaleziono pliku stacji lub plik jest pusty/uszkodzony.", 5000);
        updateStationsList({});
        ui->saveStationsButton->setEnabled(false);
        ui->cityFilterLineEdit->setEnabled(false);
        return false;
    }
}


void MainWindow::on_saveStationsButton_clicked()
{
    qDebug() << "Kliknięto przycisk Zapisz Stacje";
    if(m_currentStations.empty()) {
        QMessageBox::information(this, "Zapis Stacji", "Brak stacji do zapisania.");
        return;
    }
    ui->statusbar->showMessage("Zapisywanie stacji do pliku...");
    if(m_dataStorage->saveStationsToJson(m_currentStations)) {
        ui->statusbar->showMessage("Stacje zapisane pomyślnie.", 3000);
    } else {
        QMessageBox::warning(this, "Błąd Zapisu", "Nie udało się zapisać stacji do pliku.");
        ui->statusbar->showMessage("Błąd zapisu stacji.", 5000);
    }
}

void MainWindow::on_stationsListWidget_itemClicked(QListWidgetItem *item)
{
    if (!item || m_isFetchingSensors) return;

    int stationId = item->data(Qt::UserRole).toInt();
    m_lastClickedStationId = stationId;
    qDebug() << "Wybrano stację:" << item->text() << "ID:" << stationId;
    if (stationId > 0) {
        ui->statusbar->showMessage(QString("Pobieranie czujników i AQI dla stacji ID: %1...").arg(stationId));
        setUiFetchingState(m_isFetchingStations, true, false);

        clearSensorDetails();
        clearChart();
        clearAnalysisResults();
        clearAirQualityIndexDisplay();
        ui->sensorsListWidget->clear();
        ui->sensorsListWidget->setEnabled(true);

        ui->loadSensorDataButton->setEnabled(false);

        m_apiService->fetchSensorsForStation(stationId);
        m_apiService->fetchAirQualityIndex(stationId);
    }
    else {
        m_lastClickedStationId = -1;
    }
}

void MainWindow::on_sensorsListWidget_itemClicked(QListWidgetItem *item)
{
    if (!item || m_isFetchingSensorData) return;

    int sensorId = item->data(Qt::UserRole).toInt();
    qDebug() << "Wybrano czujnik:" << item->text() << "ID:" << sensorId;
    if (sensorId > 0) {
        ui->statusbar->showMessage(QString("Pobieranie danych dla czujnika ID: %1...").arg(sensorId));
        setUiFetchingState(m_isFetchingStations, m_isFetchingSensors, true);

        clearChart();
        clearAnalysisResults();

        ui->saveSensorDataButton->setEnabled(false);
        ui->analyzeButton->setEnabled(false);
        ui->filterDataButton->setEnabled(false);
        ui->startDateTimeEdit->setEnabled(false);
        ui->endDateTimeEdit->setEnabled(false);

        m_apiService->fetchSensorData(sensorId);
    }
}

void MainWindow::on_loadSensorDataButton_clicked()
{
    if (m_isFetchingSensorData) return;

    int sensorId = getSelectedSensorId();
    if (sensorId <= 0) {
        QMessageBox::information(this, "Brak Czujnika", "Proszę wybrać czujnik z listy.");
        return;
    }
    ui->statusbar->showMessage(QString("Ładowanie danych dla czujnika %1 z pliku...").arg(sensorId));
    clearChart();
    clearAnalysisResults();
    loadSensorDataFromFile(sensorId);
}

bool MainWindow::loadSensorDataFromFile(int sensorId) {
    if (sensorId <= 0) return false;

    QString filename = QString("sensor_%1_data.json").arg(sensorId);
    qDebug() << "Wczytywanie danych czujnika z pliku:" << filename << "w ścieżce:" << m_dataStorage->getStoragePath();

    SensorData data = m_dataStorage->loadSensorDataFromJson(filename);
    qDebug() << "Wczytane dane - Klucz:" << data.key << "Liczba wartości:" << data.values.size();

    if (!data.key.isEmpty() && !data.values.empty()) {
        m_currentSensorData = data;
        setupDateTimeEditsWithDataRange(m_currentSensorData.values);
        updateChart(m_currentSensorData.values, m_currentSensorData.key);
        ui->statusbar->showMessage(QString("Dane dla czujnika %1 załadowane z pliku.").arg(sensorId), 3000);

        ui->saveSensorDataButton->setEnabled(true);
        ui->analyzeButton->setEnabled(true);
        ui->filterDataButton->setEnabled(true);
        ui->startDateTimeEdit->setEnabled(true);
        ui->endDateTimeEdit->setEnabled(true);
        qDebug() << "Wczytywanie z pliku udane, UI zaktualizowane dla czujnika" << sensorId;
        return true;
    } else {
        qWarning() << "Wczytane dane są puste lub nieprawidłowe dla czujnika" << sensorId;
        ui->statusbar->showMessage(QString("Brak zapisanych danych dla czujnika %1 w pliku.").arg(sensorId), 5000);
        m_currentSensorData = SensorData();

        clearChart();
        clearAnalysisResults();
        ui->saveSensorDataButton->setEnabled(false);
        ui->analyzeButton->setEnabled(false);
        ui->filterDataButton->setEnabled(false);
        ui->startDateTimeEdit->setEnabled(false);
        ui->endDateTimeEdit->setEnabled(false);
        return false;
    }
}


void MainWindow::on_saveSensorDataButton_clicked()
{
    int sensorId = getSelectedSensorId();
    if (sensorId <= 0) {
        QMessageBox::information(this, "Brak Czujnika", "Proszę wybrać czujnik, którego dane mają być zapisane.");
        return;
    }
    qDebug() << "on_saveSensorDataButton_clicked: Sprawdzanie m_currentSensorData. Klucz:"
             << m_currentSensorData.key << "Liczba wartości:" << m_currentSensorData.values.size();

    if (m_currentSensorData.key.isEmpty() || m_currentSensorData.values.empty()) {
        QMessageBox::information(this, "Brak Danych", "Brak aktualnych (pełnych) danych pomiarowych do zapisania dla wybranego czujnika.");
        return;
    }

    QString filename = QString("sensor_%1_data.json").arg(sensorId);
    qDebug() << "Zapisywanie pełnych danych czujnika do pliku:" << filename << "w ścieżce:" << m_dataStorage->getStoragePath();
    ui->statusbar->showMessage(QString("Zapisywanie danych dla czujnika %1...").arg(sensorId));

    if (m_dataStorage->saveSensorDataToJson(m_currentSensorData, filename)) {
        ui->statusbar->showMessage(QString("Dane dla czujnika %1 zapisane pomyślnie.").arg(sensorId), 3000);
    } else {
        QMessageBox::warning(this, "Błąd Zapisu", QString("Nie udało się zapisać danych dla czujnika %1.").arg(sensorId));
        ui->statusbar->showMessage(QString("Błąd zapisu danych dla czujnika %1.").arg(sensorId), 5000);
    }
}

void MainWindow::on_analyzeButton_clicked()
{
    if (m_currentSensorData.key.isEmpty() || m_currentSensorData.values.empty()) {
        QMessageBox::information(this, "Brak Danych", "Brak danych do analizy. Pobierz lub wczytaj dane dla czujnika.");
        return;
    }
    qDebug() << "on_analyzeButton_clicked: Analiza m_currentSensorData. Klucz:"
             << m_currentSensorData.key << "Liczba wartości:" << m_currentSensorData.values.size();

    qDebug() << "Kliknięto przycisk Analizuj dla pełnych danych czujnika, klucz:" << m_currentSensorData.key;
    ui->statusbar->showMessage("Analizowanie pełnego zestawu danych...");

    AnalysisResult results = m_analyzer->analyze(m_currentSensorData.values);
    updateAnalysisResults(results);
    ui->statusbar->showMessage("Analiza pełnego zestawu danych zakończona.", 3000);
}


void MainWindow::on_filterDataButton_clicked()
{
    if (m_currentSensorData.values.empty()) {
        QMessageBox::information(this, "Brak Danych", "Najpierw załaduj dane dla czujnika.");
        return;
    }

    QDateTime startDate = ui->startDateTimeEdit->dateTime();
    QDateTime endDate = ui->endDateTimeEdit->dateTime();

    if (!startDate.isValid() || !endDate.isValid() || startDate >= endDate) {
        QMessageBox::warning(this, "Nieprawidłowy Zakres", "Data początkowa musi być wcześniejsza niż data końcowa i obie muszą być prawidłowe.");
        return;
    }

    qDebug() << "Filtrowanie danych od" << startDate.toString(Qt::ISODate) << "do" << endDate.toString(Qt::ISODate);

    std::vector<MeasurementValue> filteredValues = filterDataByDateRange(m_currentSensorData.values, startDate, endDate);
    qDebug() << "Oryginalne wartości:" << m_currentSensorData.values.size() << "Przefiltrowane wartości:" << filteredValues.size();

    updateChart(filteredValues, m_currentSensorData.key);

    if (!filteredValues.empty()) {
        AnalysisResult filteredResults = m_analyzer->analyze(filteredValues);
        updateAnalysisResults(filteredResults);
        ui->statusbar->showMessage(QString("Wyświetlono dane z zakresu %1 - %2.").arg(startDate.toString("yyyy-MM-dd HH:mm")).arg(endDate.toString("yyyy-MM-dd HH:mm")), 4000);
    } else {
        clearAnalysisResults();
        ui->statusbar->showMessage("Brak danych w wybranym zakresie.", 4000);
    }
}

void MainWindow::filterStations(const QString &text)
{
    QString filterText = text.trimmed().toLower();
    std::vector<MeasuringStation> filteredStations;

    if (filterText.isEmpty()) {
        filteredStations = m_currentStations;
        ui->clearCityFilterButton->setEnabled(false);
    } else {
        for (const auto& station : m_currentStations) {
            if (station.city.name.toLower().contains(filterText)) {
                filteredStations.push_back(station);
            }
        }
        ui->clearCityFilterButton->setEnabled(true);
    }

    updateStationsList(filteredStations);
}

void MainWindow::on_clearCityFilterButton_clicked()
{
    ui->cityFilterLineEdit->clear();
}

void MainWindow::handleStationsReady(const std::vector<MeasuringStation>& stations)
{
    qDebug() << "Otrzymano" << stations.size() << "stacji.";
    m_currentStations = stations;
    filterStations(ui->cityFilterLineEdit->text());
    ui->saveStationsButton->setEnabled(!stations.empty());
    ui->statusbar->showMessage(QString("Pobrano %1 stacji.").arg(stations.size()), 3000);
    ui->cityFilterLineEdit->setEnabled(!stations.empty());
    setUiFetchingState(false, m_isFetchingSensors, m_isFetchingSensorData);
}

void MainWindow::handleSensorsReady(const std::vector<Sensor>& sensors)
{
    qDebug() << "Otrzymano" << sensors.size() << "czujników.";
    m_currentSensors = sensors;
    updateSensorsList(sensors);
    ui->statusbar->showMessage(QString("Pobrano %1 czujników dla wybranej stacji.").arg(sensors.size()), 3000);
    if (m_lastClickedStationId > 0 && !sensors.empty()) {
        qDebug() << "Próba zapisu listy czujników dla stacji" << m_lastClickedStationId;
        if (!m_dataStorage->saveSensorsToJson(m_lastClickedStationId, sensors)) {
            qWarning() << "Nie udało się zapisać listy czujników dla stacji" << m_lastClickedStationId;
        }
    }

    setUiFetchingState(m_isFetchingStations, false, m_isFetchingSensorData);
}

void MainWindow::handleSensorDataReady(const SensorData& data)
{
    qDebug() << "Otrzymano dane dla klucza:" << data.key << "z" << data.values.size() << "wartościami.";
    m_currentSensorData = data;

    if (!m_currentSensorData.key.isEmpty() && !m_currentSensorData.values.empty()) {
        qDebug() << "handleSensorDataReady: Dane są prawidłowe. Aktualizacja UI.";
        setupDateTimeEditsWithDataRange(m_currentSensorData.values);
        updateChart(m_currentSensorData.values, m_currentSensorData.key);
        ui->saveSensorDataButton->setEnabled(true);
        ui->analyzeButton->setEnabled(true);
        ui->filterDataButton->setEnabled(true);
        ui->startDateTimeEdit->setEnabled(true);
        ui->endDateTimeEdit->setEnabled(true);
        ui->statusbar->showMessage(QString("Pobrano dane dla parametru: %1").arg(data.key), 3000);
    } else {
        qWarning() << "Otrzymano sygnał sensorDataReady, ale klucz danych jest pusty lub brak wartości. Czyszczenie UI.";
        QMessageBox::information(this, "Brak Danych", "Nie udało się pobrać danych pomiarowych dla tego czujnika lub brak wartości w odpowiedzi API.");
        ui->statusbar->showMessage("Brak danych pomiarowych dla wybranego czujnika.", 5000);
        clearChart();
        clearAnalysisResults();
        ui->saveSensorDataButton->setEnabled(false);
        ui->analyzeButton->setEnabled(false);
        ui->filterDataButton->setEnabled(false);
        ui->startDateTimeEdit->setEnabled(false);
        ui->endDateTimeEdit->setEnabled(false);
        m_currentSensorData = SensorData();
    }

    setUiFetchingState(m_isFetchingStations, m_isFetchingSensors, false);
}


void MainWindow::handleAirQualityIndexReady(const AirQualityIndex& index)
{
    qDebug() << "Otrzymano AQI dla stacji ID:" << index.stationId;
    if (index.stationId != -1) {
        m_currentAirQualityIndex = index;
        updateAirQualityIndexDisplay(index);

        qDebug() << "Próba zapisu AQI dla stacji" << index.stationId;
        if (!m_dataStorage->saveAirQualityIndexToJson(index.stationId, index)) {
            qWarning() << "Nie udało się zapisać AQI dla stacji" << index.stationId;
        }

    } else {
        qWarning() << "Otrzymano nieprawidłowy AQI (ID=-1), nie zapisuję i nie aktualizuję UI.";
    }
}

void MainWindow::handleNetworkError(const QString& errorMsg)
{
    qWarning() << "Błąd Sieci/Przetwarzania:" << errorMsg;
    bool handled = false;

    bool isConnectionError = errorMsg.contains("Host not found", Qt::CaseInsensitive) ||
                             errorMsg.contains("timed out", Qt::CaseInsensitive) ||
                             errorMsg.contains("Network access is disabled", Qt::CaseInsensitive) ||
                             errorMsg.contains("Connection refused", Qt::CaseInsensitive) ||
                             errorMsg.contains("Error transferring", Qt::CaseInsensitive) ||
                             errorMsg.contains("Błąd sieci", Qt::CaseInsensitive);

    bool errorRelatedToStations = errorMsg.contains("(stacje)");
    bool errorRelatedToSensors = errorMsg.contains("(czujniki)");
    bool errorRelatedToAQI = errorMsg.contains("(indeks AQI)") || errorMsg.contains("(AQI)");
    bool errorRelatedToSensorData = errorMsg.contains("(dane pomiarowe)");

    bool wasFetchingStations = m_isFetchingStations;
    bool wasFetchingSensorsOrAqi = m_isFetchingSensors;
    bool wasFetchingSensorData = m_isFetchingSensorData;

    bool attemptFallback = false;
    QMessageBox::StandardButton reply = QMessageBox::NoButton;
    int currentStationId = m_lastClickedStationId;
    int currentSensorId = getSelectedSensorId();

    if (isConnectionError)
    {
        if (errorRelatedToStations || errorRelatedToSensors || errorRelatedToSensorData)
        {
            QString userMessage = "Błąd połączenia: " + errorMsg + "\n\nCzy chcesz spróbować załadować dane z lokalnej pamięci podręcznej (ostatnio zapisany plik)?";
            reply = QMessageBox::question(this, "Błąd Sieci", userMessage,
                                          QMessageBox::Yes | QMessageBox::No);
            attemptFallback = (reply == QMessageBox::Yes);

            if (attemptFallback) {

                if (errorRelatedToStations && wasFetchingStations) {
                    ui->statusbar->showMessage("Błąd sieci. Ładowanie stacji z pliku...", 4000);
                    if (loadStationsFromFile()) {
                        handled = true;
                        setUiFetchingState(false, m_isFetchingSensors, m_isFetchingSensorData);
                    } else {
                        QMessageBox::warning(this, "Brak Danych Lokalnych", "Nie znaleziono zapisanych lokalnie danych stacji.");
                        ui->statusbar->showMessage("Brak danych lokalnych stacji.", 5000);
                    }
                }
                else if (errorRelatedToSensors && wasFetchingSensorsOrAqi) {
                    if (currentStationId > 0) {
                        ui->statusbar->showMessage(QString("Błąd sieci. Ładowanie listy czujników dla stacji %1 z pliku...").arg(currentStationId), 4000);
                        std::vector<Sensor> loadedSensors = m_dataStorage->loadSensorsFromJson(currentStationId);
                        if (!loadedSensors.empty()) {
                            m_currentSensors = loadedSensors;
                            updateSensorsList(loadedSensors);
                            ui->sensorsListWidget->setEnabled(true);
                            ui->statusbar->showMessage(QString("Lista czujników dla stacji %1 załadowana z pliku.").arg(currentStationId), 3000);
                            handled = true;

                            setUiFetchingState(m_isFetchingStations, false, m_isFetchingSensorData);
                        } else {
                            QMessageBox::warning(this, "Brak Danych Lokalnych", QString("Nie znaleziono zapisanych lokalnie czujników dla stacji %1.").arg(currentStationId));
                            ui->statusbar->showMessage("Brak danych lokalnych czujników.", 5000);
                        }
                    } else {
                        qWarning() << "Nie można wczytać czujników z pliku, brak ID stacji (m_lastClickedStationId <= 0).";
                    }
                }
                else if (errorRelatedToSensorData && wasFetchingSensorData) {
                    if (currentSensorId > 0) {
                        ui->statusbar->showMessage(QString("Błąd sieci. Ładowanie danych czujnika %1 z pliku...").arg(currentSensorId), 4000);
                        if (loadSensorDataFromFile(currentSensorId)) {
                            handled = true;
                            setUiFetchingState(m_isFetchingStations, m_isFetchingSensors, false);
                        } else {
                            QMessageBox::warning(this, "Brak Danych Lokalnych", QString("Nie znaleziono zapisanych lokalnie danych dla czujnika %1.").arg(currentSensorId));
                            ui->statusbar->showMessage("Brak danych lokalnych dla czujnika.", 5000);
                        }
                    } else {
                        qWarning() << "Nie można wczytać danych sensora z pliku, brak wybranego sensora w UI.";
                    }
                }
            }
            }

            else if (errorRelatedToAQI && wasFetchingSensorsOrAqi)
        {
            qDebug() << "Wykryto błąd sieci podczas pobierania AQI, próba wczytania z cache.";
            if (currentStationId > 0) {
                AirQualityIndex cachedIndex = m_dataStorage->loadAirQualityIndexFromJson(currentStationId);
                if (cachedIndex.stationId != -1) {
                    m_currentAirQualityIndex = cachedIndex;
                    updateAirQualityIndexDisplay(cachedIndex);
                    ui->statusbar->showMessage(QString("Błąd sieci. Wyświetlono ostatni znany Indeks Jakości Powietrza dla stacji %1.").arg(currentStationId), 5000);
                    handled = true;

                    setUiFetchingState(m_isFetchingStations, false, m_isFetchingSensorData);
                } else {
                    qDebug() << "Nie znaleziono AQI w cache dla stacji" << currentStationId;
                }
            } else {
                qWarning() << "Nie można wczytać AQI z cache, brak ID stacji.";
            }
        }
        }

    if (!handled)
    {
        if (errorRelatedToStations && wasFetchingStations) {
            setUiFetchingState(false, m_isFetchingSensors, m_isFetchingSensorData);
        }
        else if (errorRelatedToSensors && wasFetchingSensorsOrAqi) {
            setUiFetchingState(m_isFetchingStations, false, m_isFetchingSensorData);
            updateSensorsList({});
            ui->sensorsListWidget->setEnabled(false);
        }
        else if (errorRelatedToAQI && wasFetchingSensorsOrAqi) {
            setUiFetchingState(m_isFetchingStations, false, m_isFetchingSensorData);
            updateAirQualityIndexDisplay(AirQualityIndex());
        }
        else if (errorRelatedToSensorData && wasFetchingSensorData) {
            setUiFetchingState(m_isFetchingStations, m_isFetchingSensors, false);
            ui->analyzeButton->setEnabled(false);
            ui->filterDataButton->setEnabled(false);
            ui->startDateTimeEdit->setEnabled(false);
            ui->endDateTimeEdit->setEnabled(false);
            ui->saveSensorDataButton->setEnabled(false);
            clearChart();
            clearAnalysisResults();
        }
        else if (wasFetchingStations) setUiFetchingState(false, m_isFetchingSensors, m_isFetchingSensorData);
        else if (wasFetchingSensorsOrAqi) setUiFetchingState(m_isFetchingStations, false, m_isFetchingSensorData);
        else if (wasFetchingSensorData) setUiFetchingState(m_isFetchingStations, m_isFetchingSensors, false);
    }

    if (!handled)
    {
        // 1. Błędy AQI
        if (errorMsg.contains("Indeks Jakości Powietrza niedostępny", Qt::CaseInsensitive) || errorRelatedToAQI) {
            updateAirQualityIndexDisplay(AirQualityIndex());
            ui->statusbar->showMessage("Błąd: " + errorMsg + " (Indeks Jakości Powietrza)", 5000);
        }
        // 2. Błędy parsowania
        else if (errorMsg.contains("Błąd przetwarzania", Qt::CaseInsensitive) || errorMsg.contains("nieprawidłowy format", Qt::CaseInsensitive)) {
            QMessageBox::warning(this, "Błąd Przetwarzania Danych", "Wystąpił problem podczas przetwarzania odpowiedzi z serwera:\n" + errorMsg);
            ui->statusbar->showMessage("Błąd przetwarzania danych.", 5000);
        }
        // 3. Błąd połączenia
        else if (isConnectionError && attemptFallback == false && reply == QMessageBox::No) {
            ui->statusbar->showMessage("Anulowano ładowanie danych lokalnych.", 4000);
        }
        // 4. Inne błędy
        else {
            QMessageBox::warning(this, "Błąd", "Wystąpił błąd:\n" + errorMsg);
            ui->statusbar->showMessage("Wystąpił błąd.", 5000);
        }
    }
}

void MainWindow::updateStationsList(const std::vector<MeasuringStation>& stations)
{
    ui->stationsListWidget->clear();
    if (stations.empty()) {
        QString infoText = m_currentStations.empty() ? "Brak stacji. Pobierz lub wczytaj." : "Brak stacji pasujących do filtra.";
        QListWidgetItem *infoItem = new QListWidgetItem(infoText);
        infoItem->setFlags(infoItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
        infoItem->setForeground(Qt::gray);
        ui->stationsListWidget->addItem(infoItem);
    } else {
        for (const auto& station : stations) {
            QString displayText = QString("%1 (%2, %3)")
            .arg(station.stationName)
                .arg(station.city.name)
                .arg(station.city.commune.provinceName);
            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, station.id);
            item->setToolTip(QString("ID: %1\nAdres: %2\nGmina: %3\nPowiat: %4")
                                 .arg(station.id)
                                 .arg(station.city.addressStreet.isEmpty() ? "Brak danych" : station.city.addressStreet)
                                 .arg(station.city.commune.communeName)
                                 .arg(station.city.commune.districtName));
            ui->stationsListWidget->addItem(item);
        }
    }
}

void MainWindow::updateSensorsList(const std::vector<Sensor>& sensors)
{
    ui->sensorsListWidget->clear();
    if (sensors.empty()) {
        QListWidgetItem *infoItem = new QListWidgetItem("Brak czujników dla tej stacji.");
        infoItem->setFlags(infoItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
        infoItem->setForeground(Qt::gray);
        ui->sensorsListWidget->addItem(infoItem);
    } else {
        for (const auto& sensor : sensors) {
            QString displayText = QString("%1 (%2)")
                                      .arg(sensor.param.paramName)
                                      .arg(sensor.param.paramFormula);
            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, sensor.id);
            item->setToolTip(QString("ID Czujnika: %1\nParametr ID: %2\nKod: %3")
                                 .arg(sensor.id)
                                 .arg(sensor.param.idParam)
                                 .arg(sensor.param.paramCode));
            ui->sensorsListWidget->addItem(item);
        }
    }
}


void MainWindow::updateChart(const std::vector<MeasurementValue>& valuesToPlot, const QString& seriesKey)
{
    m_series->clear();

    if (valuesToPlot.empty()) {
        m_chart->setTitle(QString("Brak danych dla '%1'").arg(seriesKey.isEmpty() ? "wybranego czujnika" : seriesKey));
        m_axisX->setRange(QDateTime::currentDateTime().addDays(-1), QDateTime::currentDateTime());
        m_axisY->setRange(0, 10);

        if (!m_chart->legend()->markers(m_series).isEmpty()) {
            m_chart->legend()->markers(m_series).first()->setVisible(false);
        }
        return;
    }

    m_series->setName(seriesKey);

    qint64 minTimestamp = std::numeric_limits<qint64>::max();
    qint64 maxTimestamp = std::numeric_limits<qint64>::min();
    double minValue = std::numeric_limits<double>::max();
    double maxValue = -std::numeric_limits<double>::max();
    bool validValueFound = false;

    QList<QPointF> points;
    points.reserve(valuesToPlot.size());

    for (const auto& mv : valuesToPlot) {
        if (mv.date.isValid() && !std::isnan(mv.value)) {
            points.append(QPointF(mv.date.toMSecsSinceEpoch(), mv.value));

            qint64 currentTimestamp = mv.date.toMSecsSinceEpoch();
            minTimestamp = std::min(minTimestamp, currentTimestamp);
            maxTimestamp = std::max(maxTimestamp, currentTimestamp);
            minValue = std::min(minValue, mv.value);
            maxValue = std::max(maxValue, mv.value);
            validValueFound = true;
        } else {
            qDebug() << "Pominięcie nieprawidłowego punktu: Data=" << mv.date << "Wartość=" << mv.value;
        }
    }

    m_series->replace(points);

    if (validValueFound) {
        QDateTime minDate = QDateTime::fromMSecsSinceEpoch(minTimestamp);
        QDateTime maxDate = QDateTime::fromMSecsSinceEpoch(maxTimestamp);

        if (minTimestamp == maxTimestamp) {
            m_axisX->setMin(minDate.addSecs(-3600));
            m_axisX->setMax(maxDate.addSecs(3600));
        } else {
            qint64 timeRange = maxTimestamp - minTimestamp;
            qint64 timePadding = std::max((qint64)3600 * 1000, timeRange / 20);
            m_axisX->setMin(QDateTime::fromMSecsSinceEpoch(minTimestamp - timePadding));
            m_axisX->setMax(QDateTime::fromMSecsSinceEpoch(maxTimestamp + timePadding));
        }

        double yRange = maxValue - minValue;
        double yPadding;
        if (yRange <= 1e-9) {
            yPadding = std::max(1.0, std::abs(minValue * 0.1));
        } else {
            yPadding = yRange * 0.1;
        }
        double finalMinY = std::floor(minValue - yPadding);
        double finalMaxY = std::ceil(maxValue + yPadding);

        if(std::isfinite(finalMinY) && std::isfinite(finalMaxY) && finalMaxY > finalMinY) {
            m_axisY->setMin(finalMinY);
            m_axisY->setMax(finalMaxY);
        } else {
            qWarning() << "Nieprawidłowy obliczony zakres osi Y (Min:" << finalMinY << "Max:" << finalMaxY << "), ustawianie domyślnego.";
            if (std::isfinite(minValue) && std::isfinite(maxValue)) {
                m_axisY->setRange(minValue - std::max(1.0, std::abs(minValue*0.1)), maxValue + std::max(1.0, std::abs(maxValue*0.1)));
            } else {
                m_axisY->setRange(0, 10);
            }
        }

        QString title = QString("Dane dla %1 (%2 - %3)")
                            .arg(seriesKey)
                            .arg(minDate.toString("yyyy-MM-dd HH:mm"))
                            .arg(maxDate.toString("yyyy-MM-dd HH:mm"));
        m_chart->setTitle(title);

        if (!m_chart->legend()->markers(m_series).isEmpty()) {
            m_chart->legend()->markers(m_series).first()->setVisible(true);
        }
    } else {
        m_chart->setTitle(QString("Brak ważnych danych dla '%1'").arg(seriesKey));
        m_axisX->setRange(QDateTime::currentDateTime().addDays(-1), QDateTime::currentDateTime());
        m_axisY->setRange(0, 10);
        if (!m_chart->legend()->markers(m_series).isEmpty()) {
            m_chart->legend()->markers(m_series).first()->setVisible(false);
        }
    }
}

void MainWindow::updateAirQualityIndexDisplay(const AirQualityIndex& index) {
    clearAirQualityIndexDisplay();

    bool indexAvailable = (index.stationId != -1);

    if (!indexAvailable) {
        if (ui->aqiStationLabel) ui->aqiStationLabel->setText("Indeks dla: (niedostępny)");
        if (ui->aqiOverallLabel) ui->aqiOverallLabel->setText("Ogólny: Niedostępny");

        if (ui->aqiPM10Label) ui->aqiPM10Label->setText("PM10:");
        if (ui->aqiPM25Label) ui->aqiPM25Label->setText("PM2.5:");
        if (ui->aqiO3Label) ui->aqiO3Label->setText("O3:");
        if (ui->aqiNO2Label) ui->aqiNO2Label->setText("NO2:");
        if (ui->aqiSO2Label) ui->aqiSO2Label->setText("SO2:");
        if (ui->aqiCOLabel) ui->aqiCOLabel->setText("CO:");
        if (ui->aqiC6H6Label) ui->aqiC6H6Label->setText("C6H6:");
        return;
    }

    QString stationName = QString("Stacja ID: %1").arg(index.stationId);
    for (const auto& station : m_currentStations) {
        if (station.id == index.stationId) {
            stationName = station.stationName;
            break;
        }
    }

    if(ui->aqiStationLabel) ui->aqiStationLabel->setText(QString("Indeks dla: %1").arg(stationName));
    if(ui->aqiCalcDateLabel) {
        QString calcDateText = QString("Obliczony: %1")
        .arg(index.stCalcDate.isValid() ? index.stCalcDate.toString("dd.MM.yyyy HH:mm") : "Brak danych");
        ui->aqiCalcDateLabel->setText(calcDateText);
        ui->aqiCalcDateLabel->setToolTip(QString("Data danych źródłowych: %1")
                                             .arg(index.stSourceDataDate.isValid() ? index.stSourceDataDate.toString("dd.MM.yyyy HH:mm") : "Brak danych"));
    }

    auto setRichTextLabel = [](QLabel* label, const QString& prefix, const IndexLevel& level) {
        if (!label) return;

        QString richText;
        QString toolTipText;
        QString textColorHex = "#000000";

        label->setTextFormat(Qt::RichText);

        if (level.id != -1 && !level.indexLevelName.isEmpty() && level.indexLevelName != "Brak indeksu") {
            QString levelName = level.indexLevelName;
            toolTipText = QString("ID poziomu: %1").arg(level.id);

            QString levelNameLower = level.indexLevelName.toLower();
            if (levelNameLower == QLatin1String("bardzo dobry")) { textColorHex = "#2bb51f"; }
            else if (levelNameLower == QLatin1String("dobry")) { textColorHex = "#6ab51f"; }
            else if (levelNameLower == QLatin1String("umiarkowany")) { textColorHex = "#b3b51f"; }
            else if (levelNameLower == QLatin1String("dostateczny")) { textColorHex = "#b5831f"; }
            else if (levelNameLower == QLatin1String("zły")) { textColorHex = "#b5291f"; }
            else if (levelNameLower == QLatin1String("bardzo zły")) { textColorHex = "#4124b5"; }
            else { textColorHex = "#808080"; }

            richText = QString("%1 <span style=\"color:%2; font-weight: bold;\">%3</span>")
                           .arg(prefix)
                           .arg(textColorHex)
                           .arg(levelName);
        } else {
            richText = prefix + " Brak danych";
            toolTipText = "Indeks niedostępny lub brak danych";
        }

        label->setText(richText);
        label->setToolTip(toolTipText);
    };

    setRichTextLabel(ui->aqiOverallLabel, "Ogólny:", index.stIndexLevel);
    setRichTextLabel(ui->aqiPM10Label, "PM10:", index.pm10IndexLevel);
    setRichTextLabel(ui->aqiPM25Label, "PM2.5:", index.pm25IndexLevel);
    setRichTextLabel(ui->aqiO3Label, "O3:", index.o3IndexLevel);
    setRichTextLabel(ui->aqiNO2Label, "NO2:", index.no2IndexLevel);
    setRichTextLabel(ui->aqiSO2Label, "SO2:", index.so2IndexLevel);
    setRichTextLabel(ui->aqiCOLabel, "CO:", index.coIndexLevel);
    setRichTextLabel(ui->aqiC6H6Label, "C6H6:", index.c6h6IndexLevel);
}

void MainWindow::updateAnalysisResults(const AnalysisResult& result) {
    clearAnalysisResults();

    if (result.minVal && !std::isnan(result.minVal->value)) {
        if(ui->analysisMinLabel) {
            QString minText = QString("Min: <b>%1</b> (%2)")
            .arg(QString::number(result.minVal->value, 'f', 2))
                .arg(result.minVal->date.toString("HH:mm dd.MM"));
            ui->analysisMinLabel->setTextFormat(Qt::RichText);
            ui->analysisMinLabel->setText(minText);
        }
    } else {
        if(ui->analysisMinLabel) ui->analysisMinLabel->setText("Min: Brak danych");
    }

    if (result.maxVal && !std::isnan(result.maxVal->value)) {
        if(ui->analysisMaxLabel) {
            QString maxText = QString("Max: <b>%1</b> (%2)")
            .arg(QString::number(result.maxVal->value, 'f', 2))
                .arg(result.maxVal->date.toString("HH:mm dd.MM"));
            ui->analysisMaxLabel->setTextFormat(Qt::RichText);
            ui->analysisMaxLabel->setText(maxText);
        }
    } else {
        if(ui->analysisMaxLabel) ui->analysisMaxLabel->setText("Max: Brak danych");
    }

    if (result.average && !std::isnan(*result.average)) {
        if(ui->analysisAvgLabel) {
            QString avgText = QString("Średnia: <b>%1</b>")
                                  .arg(QString::number(*result.average, 'f', 2));
            ui->analysisAvgLabel->setTextFormat(Qt::RichText);
            ui->analysisAvgLabel->setText(avgText);
        }
    } else {
        if(ui->analysisAvgLabel) ui->analysisAvgLabel->setText("Średnia: Brak danych");
    }

    QString trendStr = "Trend: ";
    QString slopeToolTip = "";

    switch(result.trend) {
    case AnalysisResult::INCREASING:
        trendStr += QString("<font color=\"red\"><b>Wzrostowy</b></font> (nachylenie: %1)")
                        .arg(QString::number(result.trendSlope, 'e', 2));
        slopeToolTip = QString("Nachylenie linii trendu: %1").arg(result.trendSlope, 0, 'g', 4);
        break;
    case AnalysisResult::DECREASING:
        trendStr += QString("<font color=\"green\"><b>Spadkowy</b></font> (nachylenie: %1)")
                        .arg(QString::number(result.trendSlope, 'e', 2));
        slopeToolTip = QString("Nachylenie linii trendu: %1").arg(result.trendSlope, 0, 'g', 4);
        break;
    case AnalysisResult::STABLE:
        trendStr += QString("<b>Stabilny</b> (nachylenie: %1)")
                        .arg(QString::number(result.trendSlope, 'e', 2));
        slopeToolTip = QString("Nachylenie linii trendu: %1").arg(result.trendSlope, 0, 'g', 4);
        break;
    case AnalysisResult::UNKNOWN:
    default:
        trendStr += "Nieznany/Brak danych";
        slopeToolTip = "Niewystarczająca ilość danych do określenia trendu.";
        break;
    }

    if(ui->analysisTrendLabel) {
        ui->analysisTrendLabel->setTextFormat(Qt::RichText);
        ui->analysisTrendLabel->setText(trendStr);
        ui->analysisTrendLabel->setToolTip(slopeToolTip);
    }
}

void MainWindow::displayErrorMessage(const QString& message) {
    QMessageBox::critical(this, "Błąd Krytyczny", message);
}

void MainWindow::clearSensorDetails() {
    qDebug() << ">>> Czyszczenie Szczegółów Czujnika <<<";
    m_currentSensors.clear();
    m_currentSensorData = SensorData();
    updateSensorsList({});
    ui->sensorsListWidget->setEnabled(false);

    ui->loadSensorDataButton->setEnabled(false);
    ui->saveSensorDataButton->setEnabled(false);
    ui->analyzeButton->setEnabled(false);
    ui->filterDataButton->setEnabled(false);
    ui->startDateTimeEdit->setEnabled(false);
    ui->endDateTimeEdit->setEnabled(false);
    ui->startDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->endDateTimeEdit->setDateTime(QDateTime::currentDateTime());

    clearChart();
    clearAnalysisResults();
}

void MainWindow::clearChart() {
    qDebug() << ">>> Czyszczenie Wykresu <<<";
    m_series->clear();
    m_chart->setTitle("Wybierz czujnik i pobierz/wczytaj dane");

    m_axisX->setRange(QDateTime::currentDateTime().addDays(-1), QDateTime::currentDateTime());
    m_axisY->setRange(0, 10);

    if (!m_chart->legend()->markers(m_series).isEmpty()) {
        m_chart->legend()->markers(m_series).first()->setVisible(false);
    }
}

void MainWindow::clearAnalysisResults() {
    if(ui->analysisMinLabel) ui->analysisMinLabel->setText("Min:");
    if(ui->analysisMaxLabel) ui->analysisMaxLabel->setText("Max:");
    if(ui->analysisAvgLabel) ui->analysisAvgLabel->setText("Średnia:");
    if(ui->analysisTrendLabel) ui->analysisTrendLabel->setText("Trend:");
    if(ui->analysisTrendLabel) ui->analysisTrendLabel->setToolTip("");
}

void MainWindow::clearAirQualityIndexDisplay() {
    m_currentAirQualityIndex = AirQualityIndex();

    auto clearLabel = [](QLabel* label, const QString& defaultText) {
        if (label) {
            label->setTextFormat(Qt::PlainText);
            label->setText(defaultText);
            label->setToolTip("");
        }
    };

    clearLabel(ui->aqiStationLabel, "Indeks dla:");
    clearLabel(ui->aqiCalcDateLabel, "Obliczony:");
    clearLabel(ui->aqiOverallLabel, "Ogólny:");
    clearLabel(ui->aqiPM10Label, "PM10:");
    clearLabel(ui->aqiPM25Label, "PM2.5:");
    clearLabel(ui->aqiO3Label, "O3:");
    clearLabel(ui->aqiNO2Label, "NO2:");
    clearLabel(ui->aqiSO2Label, "SO2:");
    clearLabel(ui->aqiCOLabel, "CO:");
    clearLabel(ui->aqiC6H6Label, "C6H6:");
}

int MainWindow::getSelectedStationId() {
    QListWidgetItem *currentItem = ui->stationsListWidget->currentItem();
    return currentItem ? currentItem->data(Qt::UserRole).toInt() : -1;
}

int MainWindow::getSelectedSensorId() {
    QListWidgetItem *currentItem = ui->sensorsListWidget->currentItem();
    return currentItem ? currentItem->data(Qt::UserRole).toInt() : -1;
}


void MainWindow::setupDateTimeEditsWithDataRange(const std::vector<MeasurementValue>& values)
{
    if (values.empty()) {

        QDateTime now = QDateTime::currentDateTime();
        ui->startDateTimeEdit->setDateTime(now.addDays(-7));
        ui->endDateTimeEdit->setDateTime(now);

        ui->startDateTimeEdit->setMinimumDateTime(QDateTime::fromSecsSinceEpoch(0));
        ui->startDateTimeEdit->setMaximumDateTime(QDateTime::currentDateTime().addYears(1));
        ui->endDateTimeEdit->setMinimumDateTime(QDateTime::fromSecsSinceEpoch(0));
        ui->endDateTimeEdit->setMaximumDateTime(QDateTime::currentDateTime().addYears(1));

        ui->startDateTimeEdit->setEnabled(false);
        ui->endDateTimeEdit->setEnabled(false);
        ui->filterDataButton->setEnabled(false);
        return;
    }

    QDateTime minDate, maxDate;
    bool firstValid = true;
    for(const auto& mv : values) {
        if (mv.date.isValid()) {
            if (firstValid) {
                minDate = maxDate = mv.date;
                firstValid = false;
            } else {
                if (mv.date < minDate) minDate = mv.date;
                if (mv.date > maxDate) maxDate = mv.date;
            }
        }
    }

    if (minDate.isValid() && maxDate.isValid()) {
        qDebug() << "Ustawianie zakresu dat w edytorach:" << minDate << "do" << maxDate;
        ui->startDateTimeEdit->setDateTime(minDate);
        ui->endDateTimeEdit->setDateTime(maxDate);

        ui->startDateTimeEdit->setMinimumDateTime(minDate);
        ui->startDateTimeEdit->setMaximumDateTime(maxDate);
        ui->endDateTimeEdit->setMinimumDateTime(minDate);
        ui->endDateTimeEdit->setMaximumDateTime(maxDate);

        ui->startDateTimeEdit->setEnabled(true);
        ui->endDateTimeEdit->setEnabled(true);
        ui->filterDataButton->setEnabled(true);
    } else {
        qWarning() << "Dane czujnika istnieją, ale nie znaleziono prawidłowych dat do ustawienia zakresu.";
        QDateTime now = QDateTime::currentDateTime();
        ui->startDateTimeEdit->setDateTime(now.addDays(-7));
        ui->endDateTimeEdit->setDateTime(now);
        ui->startDateTimeEdit->setMinimumDateTime(QDateTime::fromSecsSinceEpoch(0));
        ui->startDateTimeEdit->setMaximumDateTime(QDateTime::currentDateTime().addYears(1));
        ui->endDateTimeEdit->setMinimumDateTime(QDateTime::fromSecsSinceEpoch(0));
        ui->endDateTimeEdit->setMaximumDateTime(QDateTime::currentDateTime().addYears(1));
        ui->startDateTimeEdit->setEnabled(false);
        ui->endDateTimeEdit->setEnabled(false);
        ui->filterDataButton->setEnabled(false);
    }
}


std::vector<MeasurementValue> MainWindow::filterDataByDateRange(const std::vector<MeasurementValue>& originalValues,
                                                                const QDateTime& startDate,
                                                                const QDateTime& endDate)
{
    std::vector<MeasurementValue> filteredValues;
    QDateTime effectiveEndDate = endDate;

    for (const auto& mv : originalValues) {
        if (mv.date.isValid() && !std::isnan(mv.value) &&
            mv.date >= startDate && mv.date <= effectiveEndDate)
        {
            filteredValues.push_back(mv);
        }
    }
    return filteredValues;
}
