#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "DataStructures.h" // Include our data structures
#include "DataAnalyzer.h"

// --- DODAJ TE INCLUDY ---
#include <QtCharts/QChartView> // Include dla QChartView
#include <QtCharts/QChart>     // Include dla QChart
#include <QtCharts/QSplineSeries> // Include dla QSplineSeries (używasz tego, nie QLineSeries)
#include <QtCharts/QDateTimeAxis> // Include dla QDateTimeAxis
#include <QtCharts/QValueAxis>  // Include dla QValueAxis
// --- KONIEC DODANYCH INCLUDÓW ---

// Forward declarations - niektóre mogą stać się zbędne po dodaniu includów
namespace Ui { class MainWindow; }
class ApiService;
class DataStorage;
class DataAnalyzer;
class QListWidgetItem; // Ten nadal jest potrzebny, jeśli nie include'ujesz <QListWidgetItem>

    // Te forward-deklaracje są teraz prawdopodobnie zbędne, bo mamy #include:
    // class QChartView;
    // class QChart;
    // class QLineSeries; // Uwaga: używasz QSplineSeries, nie QLineSeries jako m_series

    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // UI Action Handlers
    void on_fetchStationsButton_clicked();
    void on_stationsListWidget_itemClicked(QListWidgetItem *item);
    void on_sensorsListWidget_itemClicked(QListWidgetItem *item);
    void on_loadStationsButton_clicked();
    void on_saveStationsButton_clicked();
    void on_loadSensorDataButton_clicked(); // Button to load data for selected sensor
    void on_saveSensorDataButton_clicked(); // Button to save current chart data
    void on_analyzeButton_clicked(); // Button to trigger analysis

    // Slots to receive data from ApiService
    void handleStationsReady(const std::vector<MeasuringStation>& stations);
    void handleSensorsReady(const std::vector<Sensor>& sensors);
    void handleSensorDataReady(const SensorData& data);
    void handleAirQualityIndexReady(const AirQualityIndex& index);
    void handleNetworkError(const QString& errorMsg);

private:
    // UI Update Methods
    void updateStationsList(const std::vector<MeasuringStation>& stations);
    void updateSensorsList(const std::vector<Sensor>& sensors);
    void updateChart(const SensorData& data);
    void updateAnalysisResults(const AnalysisResult& result);
    void updateAirQualityIndexDisplay(const AirQualityIndex& index);
    void displayErrorMessage(const QString& message);
    void clearSensorDetails();
    void clearChart();
    void clearAnalysisResults();
    void clearAirQualityIndexDisplay();

    // Helper to get selected station/sensor ID
    int getSelectedStationId();
    int getSelectedSensorId();


    Ui::MainWindow *ui;
    ApiService *m_apiService;
    DataStorage *m_dataStorage;
    DataAnalyzer *m_analyzer;


    // Store current data
    std::vector<MeasuringStation> m_currentStations;
    std::vector<Sensor> m_currentSensors;
    SensorData m_currentSensorData;
    AirQualityIndex m_currentAirQualityIndex;


    // Chart related members
    QChart *m_chart;
    QChartView *m_chartView;
    QSplineSeries *m_series; // Using SplineSeries for smoother curves
    QDateTimeAxis *m_axisX;
    QValueAxis *m_axisY;
};
#endif // MAINWINDOW_H
