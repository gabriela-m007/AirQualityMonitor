/**
 * @file MainWindow.h
 * @brief Definicja klasy MainWindow - głównego okna aplikacji monitorującej jakość powietrza.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "DataStructures.h" // Podstawowe struktury danych
#include "DataAnalyzer.h"   // Do wyników analizy

// Forward declarations dla klas QtCharts
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

// Forward declarations dla własnych klas i klas Qt używanych jako wskaźniki/referencje
namespace Ui { class MainWindow; }
class ApiService;
class DataStorage;
class DataAnalyzer;
class QListWidgetItem;
class QDateTimeEdit;

/**
 * @class MainWindow
 * @brief Główne okno aplikacji, zarządzające interfejsem użytkownika i koordynujące przepływ danych.
 *
 * Odpowiada za wyświetlanie list stacji i czujników, inicjowanie pobierania danych (przez ApiService),
 * zapis/odczyt danych z lokalnego magazynu (przez DataStorage), prezentację danych pomiarowych na wykresie (QtCharts),
 * wyświetlanie indeksu jakości powietrza (AQI) oraz wyników prostej analizy danych (przez DataAnalyzer).
 * Obsługuje również interakcje użytkownika (kliknięcia przycisków, wybór elementów list, filtrowanie).
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor głównego okna aplikacji.
     * @param parent Wskaźnik na widget nadrzędny (zazwyczaj nullptr).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor.
     */
    ~MainWindow();

    // Prywatne sloty podłączone do sygnałów z GUI (np. kliknięcia przycisków) lub innych obiektów
private slots:
    /** @brief Slot obsługujący kliknięcie przycisku "Pobierz Stacje". Inicjuje pobieranie listy stacji przez ApiService. */
    void on_fetchStationsButton_clicked();
    /** @brief Slot obsługujący kliknięcie elementu na liście stacji. Inicjuje pobieranie czujników i AQI dla wybranej stacji. */
    void on_stationsListWidget_itemClicked(QListWidgetItem *item);
    /** @brief Slot obsługujący kliknięcie elementu na liście czujników. Inicjuje pobieranie danych pomiarowych dla wybranego czujnika. */
    void on_sensorsListWidget_itemClicked(QListWidgetItem *item);
    /** @brief Slot obsługujący kliknięcie przycisku "Wczytaj Stacje". Ładuje listę stacji z pliku JSON. */
    void on_loadStationsButton_clicked();
    /** @brief Slot obsługujący kliknięcie przycisku "Zapisz Stacje". Zapisuje aktualną listę stacji do pliku JSON. */
    void on_saveStationsButton_clicked();
    /** @brief Slot obsługujący kliknięcie przycisku "Wczytaj Dane". Ładuje dane pomiarowe dla wybranego czujnika z pliku JSON. */
    void on_loadSensorDataButton_clicked();
    /** @brief Slot obsługujący kliknięcie przycisku "Zapisz Dane". Zapisuje aktualne dane pomiarowe dla wybranego czujnika do pliku JSON. */
    void on_saveSensorDataButton_clicked();
    /** @brief Slot obsługujący kliknięcie przycisku "Analizuj". Uruchamia analizę aktualnie wyświetlanych danych pomiarowych. */
    void on_analyzeButton_clicked();
    /** @brief Slot obsługujący kliknięcie przycisku "Filtruj Dane". Filtruje dane na wykresie według zakresu dat ustawionego przez użytkownika i ponownie je analizuje. */
    void on_filterDataButton_clicked();
    /** @brief Slot obsługujący kliknięcie przycisku "Wyczyść Filtr". Czyści pole filtrowania stacji po mieście. */
    void on_clearCityFilterButton_clicked();

    /** @brief Slot obsługujący zmianę tekstu w polu filtrowania stacji po mieście. Aktualizuje listę stacji. */
    void filterStations(const QString &text);

    // Sloty obsługujące sygnały z ApiService
    /** @brief Slot obsługujący sygnał ApiService::stationsReady. Aktualizuje listę stacji w GUI. */
    void handleStationsReady(const std::vector<MeasuringStation>& stations);
    /** @brief Slot obsługujący sygnał ApiService::sensorsReady. Aktualizuje listę czujników w GUI. */
    void handleSensorsReady(const std::vector<Sensor>& sensors);
    /** @brief Slot obsługujący sygnał ApiService::sensorDataReady. Aktualizuje wykres i wyniki analizy danymi pomiarowymi. */
    void handleSensorDataReady(const SensorData& data);
    /** @brief Slot obsługujący sygnał ApiService::airQualityIndexReady. Aktualizuje wyświetlanie indeksu AQI. */
    void handleAirQualityIndexReady(const AirQualityIndex& index);
    /** @brief Slot obsługujący sygnał ApiService::networkError. Wyświetla komunikat o błędzie i/lub proponuje wczytanie danych z pliku. */
    void handleNetworkError(const QString& errorMsg);

    // Metody prywatne - logika pomocnicza
private:
    /** @brief Aktualizuje QListWidget (ui->stationsListWidget) na podstawie podanego wektora stacji. */
    void updateStationsList(const std::vector<MeasuringStation>& stations);
    /** @brief Aktualizuje QListWidget (ui->sensorsListWidget) na podstawie podanego wektora czujników. */
    void updateSensorsList(const std::vector<Sensor>& sensors);
    /** @brief Aktualizuje wykres (m_chart, m_series) nowymi danymi pomiarowymi. */
    void updateChart(const std::vector<MeasurementValue>& valuesToPlot, const QString& seriesKey);
    /** @brief Aktualizuje etykiety w GUI wynikami analizy danych (min, max, średnia, trend). */
    void updateAnalysisResults(const AnalysisResult& result);
    /** @brief Aktualizuje etykiety w GUI danymi o indeksie jakości powietrza (AQI). */
    void updateAirQualityIndexDisplay(const AirQualityIndex& index);
    /** @brief Wyświetla krytyczny komunikat o błędzie w okienku QMessageBox. */
    void displayErrorMessage(const QString& message);
    /** @brief Czyści sekcję szczegółów czujnika (lista czujników, wykres, analiza, AQI, przyciski). */
    void clearSensorDetails();
    /** @brief Czyści dane i tytuł wykresu. */
    void clearChart();
    /** @brief Czyści etykiety z wynikami analizy danych. */
    void clearAnalysisResults();
    /** @brief Czyści etykiety z danymi indeksu jakości powietrza. */
    void clearAirQualityIndexDisplay();

    /** @brief Zwraca ID aktualnie wybranej stacji z listy lub -1, jeśli nic nie jest wybrane. */
    int getSelectedStationId();
    /** @brief Zwraca ID aktualnie wybranego czujnika z listy lub -1, jeśli nic nie jest wybrane. */
    int getSelectedSensorId();

    ///< Przechowuje ID ostatnio klikniętej stacji (używane m.in. do zapisu/odczytu cache).
    int m_lastClickedStationId = -1;

    /** @brief Ustawia zakres dat w QDateTimeEdit (start/koniec) na podstawie zakresu dat w podanych danych pomiarowych. */
    void setupDateTimeEditsWithDataRange(const std::vector<MeasurementValue>& values);
    /** @brief Filtruje wektor danych pomiarowych, zwracając tylko te, które mieszczą się w podanym zakresie dat. */
    std::vector<MeasurementValue> filterDataByDateRange(const std::vector<MeasurementValue>& originalValues,
                                                        const QDateTime& startDate,
                                                        const QDateTime& endDate);

    /** @brief Ładuje listę stacji z domyślnego pliku JSON i aktualizuje GUI. Zwraca true jeśli się udało. */
    bool loadStationsFromFile();
    /** @brief Ładuje dane pomiarowe dla podanego ID czujnika z pliku JSON i aktualizuje GUI. Zwraca true jeśli się udało. */
    bool loadSensorDataFromFile(int sensorId);

    /**
     * @brief Ustawia stan aktywności/nieaktywności przycisków i kontrolek GUI w zależności od tego, która operacja pobierania danych jest w toku.
     * @param isFetchingStations Czy trwa pobieranie listy stacji.
     * @param isFetchingSensors Czy trwa pobieranie listy czujników i/lub AQI.
     * @param isFetchingSensorData Czy trwa pobieranie danych pomiarowych dla czujnika.
     */
    void setUiFetchingState(bool isFetchingStations, bool isFetchingSensors, bool isFetchingSensorData);

    // --- Komponenty UI i Logika Biznesowa ---
    ///< Wskaźnik na obiekt UI wygenerowany z pliku .ui.
    Ui::MainWindow *ui;
    ///< Wskaźnik na serwis komunikacji z API.
    ApiService *m_apiService;
    ///< Wskaźnik na serwis zapisu/odczytu danych z plików.
    DataStorage *m_dataStorage;
    ///< Wskaźnik na obiekt wykonujący analizę danych.
    DataAnalyzer *m_analyzer;

    // --- Buforowane dane ---
    ///< Aktualnie załadowana/pobrana lista wszystkich stacji (lub przefiltrowana).
    std::vector<MeasuringStation> m_currentStations;
    ///< Aktualnie załadowana/pobrana lista czujników dla wybranej stacji.
    std::vector<Sensor> m_currentSensors;
    ///< Aktualnie załadowane/pobrane dane pomiarowe dla wybranego czujnika.
    SensorData m_currentSensorData;
        ///< Aktualnie załadowany/pobrany indeks AQI dla wybranej stacji.
    AirQualityIndex m_currentAirQualityIndex;

    // --- Komponenty Wykresu ---
    ///< Główny obiekt wykresu QtCharts.
    QChart *m_chart;
    ///< Widget wyświetlający wykres.
    QChartView *m_chartView;
    ///< Seria danych wyświetlana na wykresie (typ Spline).
    QSplineSeries *m_series;
    ///< Oś X (czasowa) wykresu.
    QDateTimeAxis *m_axisX;
    ///< Oś Y (wartości) wykresu.
    QValueAxis *m_axisY;

    // --- Flagi stanu ---
    ///< Czy aktualnie trwa pobieranie listy stacji.
    bool m_isFetchingStations = false;
    ///< Czy aktualnie trwa pobieranie listy czujników lub AQI.
    bool m_isFetchingSensors = false;
        ///< Czy aktualnie trwa pobieranie danych pomiarowych.
    bool m_isFetchingSensorData = false;
};
#endif // MAINWINDOW_H
