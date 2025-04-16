#include "mainwindow.h"
#include "ui_mainwindow.h" // This file is generated from mainwindow.ui
#include "ApiService.h"
#include "DataStorage.h"
#include "DataAnalyzer.h"

#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths> // To find a good place for storage

// Include Qt Charts headers
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <limits> // for NaN check

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_apiService(new ApiService(this)) // Create ApiService
    , m_dataStorage(new DataStorage(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/AirQualityData")) // Store data in app data folder
    , m_analyzer(new DataAnalyzer()) // Create Analyzer
    , m_chart(new QChart())
    , m_chartView(new QChartView(m_chart))
    , m_series(new QSplineSeries()) // Use QSplineSeries
    , m_axisX(new QDateTimeAxis)
    , m_axisY(new QValueAxis)
{
    ui->setupUi(this);

    // --- Setup Chart ---
    m_chart->addSeries(m_series);

    // Configure X Axis (Time)
    m_axisX->setTickCount(10); // Adjust number of ticks as needed
    m_axisX->setFormat("yyyy-MM-dd HH:mm"); // Display format for axis labels
    m_axisX->setTitleText("Data i Czas");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);

    // Configure Y Axis (Value)
    m_axisY->setLabelFormat("%.1f"); // Format for measurement values (e.g., 1 decimal)
    m_axisY->setTitleText("Wartość");
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->setTitle("Dane Pomiarowe");

    // Add chart view to the layout (assuming you have a QVBoxLayout named 'chartLayout' in your UI)
    if(ui->chartLayout) { // Check if the layout exists in the UI file
        ui->chartLayout->addWidget(m_chartView);
    } else {
        qWarning("UI file does not contain a layout named 'chartLayout'. Chart will not be displayed.");
        // As a fallback, you might add it directly to the main window's layout or a placeholder widget
        // Example: ui->centralwidget->layout()->addWidget(m_chartView); // If central widget has layout
    }

    m_chartView->setRenderHint(QPainter::Antialiasing); // Make chart look nicer

    // --- Connect Signals and Slots ---
    connect(m_apiService, &ApiService::stationsReady, this, &MainWindow::handleStationsReady);
    connect(m_apiService, &ApiService::sensorsReady, this, &MainWindow::handleSensorsReady);
    connect(m_apiService, &ApiService::sensorDataReady, this, &MainWindow::handleSensorDataReady);
    connect(m_apiService, &ApiService::airQualityIndexReady, this, &MainWindow::handleAirQualityIndexReady);
    connect(m_apiService, &ApiService::networkError, this, &MainWindow::handleNetworkError);

    // Disable buttons that require selection initially
    ui->saveStationsButton->setEnabled(false);
    ui->sensorsListWidget->setEnabled(false); // Disable sensor list until station selected
    ui->saveSensorDataButton->setEnabled(false);
    ui->loadSensorDataButton->setEnabled(false);
    ui->analyzeButton->setEnabled(false);

    qInfo() << "Data storage path:" << m_dataStorage->getStoragePath(); // Use member access if public/friend or via getter
}

MainWindow::~MainWindow()
{
    delete ui;
    // No need to delete m_apiService, m_dataStorage, etc. if they have 'this' as parent
}

// --- UI Action Handlers ---

void MainWindow::on_fetchStationsButton_clicked()
{
    qDebug() << "Fetch Stations button clicked";
    ui->statusbar->showMessage("Pobieranie listy stacji...");
    clearSensorDetails(); // Clear details from previous selections
    clearChart();
    clearAnalysisResults();
    clearAirQualityIndexDisplay();
    ui->stationsListWidget->clear(); // Clear list before fetching
    ui->sensorsListWidget->clear();
    ui->sensorsListWidget->setEnabled(false);
    m_apiService->fetchAllStations();
}

void MainWindow::on_loadStationsButton_clicked()
{
    qDebug() << "Load Stations button clicked";
    ui->statusbar->showMessage("Ładowanie stacji z pliku...");
    clearSensorDetails();
    clearChart();
    clearAnalysisResults();
    clearAirQualityIndexDisplay();
    ui->stationsListWidget->clear();
    ui->sensorsListWidget->clear();
    ui->sensorsListWidget->setEnabled(false);

    m_currentStations = m_dataStorage->loadStationsFromJson(); // Load from default file
    if (!m_currentStations.empty()) {
        updateStationsList(m_currentStations);
        ui->statusbar->showMessage("Stacje załadowane z pliku.", 3000);
        ui->saveStationsButton->setEnabled(true); // Enable saving if loaded
    } else {
        ui->statusbar->showMessage("Nie znaleziono pliku stacji lub plik jest pusty/uszkodzony.", 5000);
        ui->saveStationsButton->setEnabled(false);
    }
}

void MainWindow::on_saveStationsButton_clicked()
{
    qDebug() << "Save Stations button clicked";
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
    int stationId = item->data(Qt::UserRole).toInt(); // Get station ID stored in the item
    qDebug() << "Station selected:" << item->text() << "ID:" << stationId;

    if (stationId > 0) {
        ui->statusbar->showMessage(QString("Pobieranie czujników dla stacji ID: %1...").arg(stationId));
        clearSensorDetails();
        clearChart();
        clearAnalysisResults();
        clearAirQualityIndexDisplay();
        ui->sensorsListWidget->clear(); // Clear previous sensors
        ui->sensorsListWidget->setEnabled(true); // Enable sensor list
        ui->loadSensorDataButton->setEnabled(false); // Disable until sensor selected
        ui->saveSensorDataButton->setEnabled(false);
        ui->analyzeButton->setEnabled(false);

        m_apiService->fetchSensorsForStation(stationId);
        m_apiService->fetchAirQualityIndex(stationId); // Also fetch AQI for the station
    }
}

void MainWindow::on_sensorsListWidget_itemClicked(QListWidgetItem *item)
{
    int sensorId = item->data(Qt::UserRole).toInt(); // Get sensor ID stored in the item
    qDebug() << "Sensor selected:" << item->text() << "ID:" << sensorId;

    if (sensorId > 0) {
        ui->statusbar->showMessage(QString("Pobieranie danych dla czujnika ID: %1...").arg(sensorId));
        clearChart();
        clearAnalysisResults();
        ui->loadSensorDataButton->setEnabled(true); // Enable loading/saving for this sensor
        ui->saveSensorDataButton->setEnabled(false); // Disable saving until data is loaded/fetched
        ui->analyzeButton->setEnabled(false); // Disable analysis until data is loaded/fetched

        m_apiService->fetchSensorData(sensorId);
    }
}

void MainWindow::on_loadSensorDataButton_clicked()
{
    int sensorId = getSelectedSensorId();
    if (sensorId <= 0) {
        QMessageBox::information(this, "Brak Czujnika", "Proszę wybrać czujnik z listy.");
        return;
    }

    QString filename = QString("sensor_%1_data.json").arg(sensorId);
    qDebug() << "Loading sensor data from file:" << filename;
    ui->statusbar->showMessage(QString("Ładowanie danych dla czujnika %1 z pliku...").arg(sensorId));
    clearChart();
    clearAnalysisResults();

    SensorData data = m_dataStorage->loadSensorDataFromJson(filename);

    if (!data.key.isEmpty() && !data.values.empty()) {
        m_currentSensorData = data; // Store loaded data
        updateChart(m_currentSensorData);
        ui->statusbar->showMessage(QString("Dane dla czujnika %1 załadowane.").arg(sensorId), 3000);
        ui->saveSensorDataButton->setEnabled(true); // Enable saving of loaded data
        ui->analyzeButton->setEnabled(true); // Enable analysis of loaded data
    } else {
        QMessageBox::information(this, "Brak Danych", QString("Nie znaleziono zapisanych danych dla czujnika %1 lub plik jest uszkodzony.").arg(sensorId));
        ui->statusbar->showMessage(QString("Brak zapisanych danych dla czujnika %1.").arg(sensorId), 5000);
        ui->saveSensorDataButton->setEnabled(false);
        ui->analyzeButton->setEnabled(false);
    }
}

void MainWindow::on_saveSensorDataButton_clicked()
{
    int sensorId = getSelectedSensorId();
    if (sensorId <= 0) {
        QMessageBox::information(this, "Brak Czujnika", "Proszę wybrać czujnik, którego dane mają być zapisane.");
        return;
    }

    // **** DODAJ TEN LOG ****
    qDebug() << "on_saveSensorDataButton_clicked: Checking m_currentSensorData. Key:"
             << m_currentSensorData.key << "Values count:" << m_currentSensorData.values.size();
    // **** KONIEC DODANEGO LOGU ****

    if (m_currentSensorData.key.isEmpty() || m_currentSensorData.values.empty()) {
        QMessageBox::information(this, "Brak Danych", "Brak aktualnych danych pomiarowych do zapisania dla wybranego czujnika.");
        return;
    }

    QString filename = QString("sensor_%1_data.json").arg(sensorId);
    qDebug() << "Saving sensor data to file:" << filename;
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

    // **** DODAJ TEN LOG ****
    qDebug() << "on_analyzeButton_clicked: Checking m_currentSensorData. Key:"
             << m_currentSensorData.key << "Values count:" << m_currentSensorData.values.size();
    // **** KONIEC DODANEGO LOGU ****

    qDebug() << "Analyze button clicked for sensor data key:" << m_currentSensorData.key;
    ui->statusbar->showMessage("Analizowanie danych...");

    AnalysisResult results = m_analyzer->analyze(m_currentSensorData.values);
    updateAnalysisResults(results);
    ui->statusbar->showMessage("Analiza zakończona.", 3000);
}


// --- Slots to receive data from ApiService ---

void MainWindow::handleStationsReady(const std::vector<MeasuringStation>& stations)
{
    qDebug() << "Received" << stations.size() << "stations.";
    m_currentStations = stations; // Store the fetched stations
    updateStationsList(m_currentStations);
    ui->saveStationsButton->setEnabled(!stations.empty()); // Enable save if we got stations
    ui->statusbar->showMessage(QString("Pobrano %1 stacji.").arg(stations.size()), 3000);
}

void MainWindow::handleSensorsReady(const std::vector<Sensor>& sensors)
{
    qDebug() << "Received" << sensors.size() << "sensors.";
    m_currentSensors = sensors; // Store the fetched sensors
    updateSensorsList(sensors);
    ui->statusbar->showMessage(QString("Pobrano %1 czujników dla wybranej stacji.").arg(sensors.size()), 3000);
}

void MainWindow::handleSensorDataReady(const SensorData& data)
{
    qDebug() << "Received data for key:" << data.key << "with" << data.values.size() << "values.";
    if (data.key.isEmpty()) {
        qWarning() << "Received empty sensor data key.";
        // Optionally show message to user
        QMessageBox::information(this, "Brak Danych", "Nie udało się pobrać danych pomiarowych dla tego czujnika (prawdopodobnie brak danych w systemie GIOŚ).");
        ui->statusbar->showMessage("Brak danych pomiarowych dla wybranego czujnika.", 5000);
        clearChart();
        clearAnalysisResults();
        ui->saveSensorDataButton->setEnabled(false);
        ui->analyzeButton->setEnabled(false);
        return;
    }

    m_currentSensorData = data; // Store the received data

    // **** DODAJ TEN LOG ****
    qDebug() << "handleSensorDataReady: Assigned data. m_currentSensorData now has key:"
             << m_currentSensorData.key << "and" << m_currentSensorData.values.size() << "values.";
    // **** KONIEC DODANEGO LOGU ****

    updateChart(data);
    ui->saveSensorDataButton->setEnabled(true); // Enable saving fetched data
    ui->analyzeButton->setEnabled(true); // Enable analysis of fetched data
    ui->statusbar->showMessage(QString("Pobrano dane dla parametru: %1").arg(data.key), 3000);
}

void MainWindow::handleAirQualityIndexReady(const AirQualityIndex& index)
{
    qDebug() << "Received AQI for station ID:" << index.stationId;
    m_currentAirQualityIndex = index; // Store the index
    updateAirQualityIndexDisplay(index);
    // No status bar message here as it might overwrite sensor data messages
}


void MainWindow::handleNetworkError(const QString& errorMsg)
{
    qWarning() << "Network Error:" << errorMsg;
    displayErrorMessage("Błąd Sieci: " + errorMsg);
    ui->statusbar->showMessage("Błąd sieci: " + errorMsg, 5000); // Show error in status bar
}


// --- UI Update Methods ---

void MainWindow::updateStationsList(const std::vector<MeasuringStation>& stations)
{
    ui->stationsListWidget->clear();
    for (const auto& station : stations) {
        // Combine name and city for better identification
        QString displayText = QString("%1 (%2, %3)")
                                  .arg(station.stationName)
                                  .arg(station.city.name)
                                  .arg(station.city.commune.provinceName);
        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, station.id); // Store station ID in the item
        ui->stationsListWidget->addItem(item);
    }
}

void MainWindow::updateSensorsList(const std::vector<Sensor>& sensors)
{
    ui->sensorsListWidget->clear();
    for (const auto& sensor : sensors) {
        // Display parameter name and formula
        QString displayText = QString("%1 (%2)")
                                  .arg(sensor.param.paramName)
                                  .arg(sensor.param.paramFormula); // e.g., "PM10 (PM10)"
        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, sensor.id); // Store sensor ID
        ui->sensorsListWidget->addItem(item);
    }
}

void MainWindow::updateChart(const SensorData& data)
{
    clearChart(); // Clear previous data first

    if (data.values.empty()) {
        m_chart->setTitle("Brak danych pomiarowych");
        return;
    }

    m_series->setName(data.key); // Set series name (e.g., "PM10")

    qint64 minTimestamp = std::numeric_limits<qint64>::max();
    qint64 maxTimestamp = std::numeric_limits<qint64>::min();
    double minValue = std::numeric_limits<double>::max();
    double maxValue = -std::numeric_limits<double>::max();
    bool validValueFound = false;

    for (const auto& mv : data.values) {
        // Only plot valid points
        if (mv.date.isValid() && !std::isnan(mv.value)) {
            m_series->append(mv.date.toMSecsSinceEpoch(), mv.value); // Use milliseconds for QDateTimeAxis

            qint64 currentTimestamp = mv.date.toMSecsSinceEpoch();
            minTimestamp = std::min(minTimestamp, currentTimestamp);
            maxTimestamp = std::max(maxTimestamp, currentTimestamp);
            minValue = std::min(minValue, mv.value);
            maxValue = std::max(maxValue, mv.value);
            validValueFound = true;
        }
    }

    if (validValueFound) {
        // Adjust axes ranges based on data
        m_axisX->setMin(QDateTime::fromMSecsSinceEpoch(minTimestamp).addSecs(-3600)); // Add some padding
        m_axisX->setMax(QDateTime::fromMSecsSinceEpoch(maxTimestamp).addSecs(3600)); // Add some padding

        // Adjust Y axis range with padding
        double padding = (maxValue - minValue) * 0.1; // 10% padding
        if (padding < 1.0) padding = 1.0; // Minimum padding of 1 unit
        m_axisY->setMin(std::floor(minValue - padding));
        m_axisY->setMax(std::ceil(maxValue + padding));

        // Set chart title dynamically
        QString title = QString("Dane dla %1 (%2 - %3)")
                            .arg(data.key)
                            .arg(QDateTime::fromMSecsSinceEpoch(minTimestamp).toString("yyyy-MM-dd HH:mm"))
                            .arg(QDateTime::fromMSecsSinceEpoch(maxTimestamp).toString("yyyy-MM-dd HH:mm"));
        m_chart->setTitle(title);
    } else {
        m_chart->setTitle(QString("Brak ważnych danych dla %1").arg(data.key));
        // Optionally reset axes if no valid data
        // m_axisX->setRange(QDateTime(), QDateTime());
        // m_axisY->setRange(0, 10); // Default range
    }

    m_chart->legend()->markers(m_series).first()->setVisible(true); // Ensure legend marker is visible

}


void MainWindow::updateAirQualityIndexDisplay(const AirQualityIndex& index) {
    clearAirQualityIndexDisplay(); // Clear previous values

    // Assuming you have QLabels in your UI named like:
    // aqiStationLabel, aqiCalcDateLabel, aqiOverallLabel, aqiPM10Label, etc.
    if (index.stationId == -1) {
        if(ui->aqiOverallLabel) ui->aqiOverallLabel->setText("Indeks niedostępny");
        return;
    }

    // Find station name (requires searching m_currentStations)
    QString stationName = "Nieznana Stacja";
    int stationId = getSelectedStationId(); // Use selected ID for lookup
    if (stationId == index.stationId) { // Check if index matches selection
        for (const auto& station : m_currentStations) {
            if (station.id == index.stationId) {
                stationName = station.stationName;
                break;
            }
        }
    } else {
        stationName = QString("Stacja ID: %1").arg(index.stationId); // Fallback if mismatch
    }


    if(ui->aqiStationLabel) ui->aqiStationLabel->setText(QString("Indeks dla: %1").arg(stationName));
    if(ui->aqiCalcDateLabel) ui->aqiCalcDateLabel->setText(QString("Obliczony: %1").arg(index.stCalcDate.toString("yyyy-MM-dd HH:mm")));

    auto setIndexLabel = [](QLabel* label, const QString& prefix, const IndexLevel& level) {
        if (!label) return;
        if (level.id != -1) {
            // You might want to add color coding here based on level.id
            label->setText(QString("%1: %2 (%3)").arg(prefix).arg(level.indexLevelName).arg(level.id));
        } else {
            label->setText(QString("%1: B.D.").arg(prefix)); // Brak Danych
        }
    };

    setIndexLabel(ui->aqiOverallLabel, "Ogólny", index.stIndexLevel);
    setIndexLabel(ui->aqiPM10Label, "PM10", index.pm10IndexLevel);
    setIndexLabel(ui->aqiPM25Label, "PM2.5", index.pm25IndexLevel);
    setIndexLabel(ui->aqiO3Label, "O3", index.o3IndexLevel);
    setIndexLabel(ui->aqiNO2Label, "NO2", index.no2IndexLevel);
    setIndexLabel(ui->aqiSO2Label, "SO2", index.so2IndexLevel);
    setIndexLabel(ui->aqiCOLabel, "CO", index.coIndexLevel);
    setIndexLabel(ui->aqiC6H6Label, "C6H6", index.c6h6IndexLevel);
}


void MainWindow::updateAnalysisResults(const AnalysisResult& result) {
    clearAnalysisResults(); // Clear previous results

    // Assuming QLabels named: analysisMinLabel, analysisMaxLabel, analysisAvgLabel, analysisTrendLabel
    if (result.minVal) {
        if(ui->analysisMinLabel) ui->analysisMinLabel->setText(QString("Min: %.2f (%1)")
                                              .arg(result.minVal->value)
                                              .arg(result.minVal->date.toString("yyyy-MM-dd HH:mm")));
    } else {
        if(ui->analysisMinLabel) ui->analysisMinLabel->setText("Min: B.D.");
    }

    if (result.maxVal) {
        if(ui->analysisMaxLabel) ui->analysisMaxLabel->setText(QString("Max: %.2f (%1)")
                                              .arg(result.maxVal->value)
                                              .arg(result.maxVal->date.toString("yyyy-MM-dd HH:mm")));
    } else {
        if(ui->analysisMaxLabel) ui->analysisMaxLabel->setText("Max: B.D.");
    }

    if (result.average) {
        if(ui->analysisAvgLabel) ui->analysisAvgLabel->setText(QString("Średnia: %.2f").arg(*result.average));
    } else {
        if(ui->analysisAvgLabel) ui->analysisAvgLabel->setText("Średnia: B.D.");
    }

    QString trendStr = "Trend: ";
    switch(result.trend) {
    case AnalysisResult::INCREASING: trendStr += QString("Wzrostowy (nachylenie: %1)").arg(result.trendSlope, 0, 'e', 2); break;
    case AnalysisResult::DECREASING: trendStr += QString("Spadkowy (nachylenie: %1)").arg(result.trendSlope, 0, 'e', 2); break;
    case AnalysisResult::STABLE: trendStr += QString("Stabilny (nachylenie: %1)").arg(result.trendSlope, 0, 'e', 2); break;
    case AnalysisResult::UNKNOWN: trendStr += "Nieznany/Brak danych"; break;
    }
    if(ui->analysisTrendLabel) ui->analysisTrendLabel->setText(trendStr);
}


void MainWindow::displayErrorMessage(const QString& message) {
    QMessageBox::critical(this, "Błąd", message);
}

void MainWindow::clearSensorDetails() {
    qDebug() << ">>> Entering clearSensorDetails() <<<"; // Dodaj log
    m_currentSensors.clear();
    m_currentSensorData = SensorData(); // Reset current data
    ui->sensorsListWidget->clear();
    ui->sensorsListWidget->setEnabled(false);
    ui->loadSensorDataButton->setEnabled(false);
    ui->saveSensorDataButton->setEnabled(false);
    ui->analyzeButton->setEnabled(false);
}

void MainWindow::clearChart() {
    qDebug() << ">>> Entering clearChart() <<<";
    m_series->clear(); // Remove points from series
    m_chart->setTitle("Wybierz czujnik");
    // Optionally reset axes ranges
    // m_axisX->setRange(QDateTime::currentDateTime().addDays(-1), QDateTime::currentDateTime());
    // m_axisY->setRange(0, 10);
    m_currentSensorData = SensorData(); // Clear associated data too
}

void MainWindow::clearAnalysisResults() {
    // Clear the analysis labels
    if(ui->analysisMinLabel) ui->analysisMinLabel->setText("Min: -");
    if(ui->analysisMaxLabel) ui->analysisMaxLabel->setText("Max: -");
    if(ui->analysisAvgLabel) ui->analysisAvgLabel->setText("Średnia: -");
    if(ui->analysisTrendLabel) ui->analysisTrendLabel->setText("Trend: -");
}


void MainWindow::clearAirQualityIndexDisplay() {
    m_currentAirQualityIndex = AirQualityIndex(); // Reset stored index
    // Clear all AQI labels
    if(ui->aqiStationLabel) ui->aqiStationLabel->setText("Indeks dla:");
    if(ui->aqiCalcDateLabel) ui->aqiCalcDateLabel->setText("Obliczony:");
    if(ui->aqiOverallLabel) ui->aqiOverallLabel->setText("Ogólny:");
    if(ui->aqiPM10Label) ui->aqiPM10Label->setText("PM10:");
    if(ui->aqiPM25Label) ui->aqiPM25Label->setText("PM2.5:");
    if(ui->aqiO3Label) ui->aqiO3Label->setText("O3:");
    if(ui->aqiNO2Label) ui->aqiNO2Label->setText("NO2:");
    if(ui->aqiSO2Label) ui->aqiSO2Label->setText("SO2:");
    if(ui->aqiCOLabel) ui->aqiCOLabel->setText("CO:");
    if(ui->aqiC6H6Label) ui->aqiC6H6Label->setText("C6H6:");
}

// --- Helper Methods ---

int MainWindow::getSelectedStationId() {
    QListWidgetItem *currentItem = ui->stationsListWidget->currentItem();
    if (currentItem) {
        return currentItem->data(Qt::UserRole).toInt();
    }
    return -1; // Return invalid ID if nothing selected
}

int MainWindow::getSelectedSensorId() {
    QListWidgetItem *currentItem = ui->sensorsListWidget->currentItem();
    if (currentItem) {
        return currentItem->data(Qt::UserRole).toInt();
    }
    return -1; // Return invalid ID if nothing selected
}
