#ifndef TESTDATASTORAGE_H
#define TESTDATASTORAGE_H

#include <QObject>
#include <QtTest/QtTest>
#include <QTemporaryDir>
#include "DataStorage.h"
#include "DataStructures.h"

class TestDataStorage : public QObject
{
    Q_OBJECT

public:
    TestDataStorage();

private:
    QTemporaryDir tempDir;
    DataStorage* storage = nullptr;

    std::vector<MeasuringStation> createTestStations(int count = 2);
    std::vector<Sensor> createTestSensors(int stationId, int count = 3);
    SensorData createTestSensorData(const QString& key);
    AirQualityIndex createTestAQI(int stationId);

private slots:
    void initTestCase();    // Wykonywane raz przed wszystkimi testami
    void cleanupTestCase(); // Wykonywane raz po wszystkich testach
    void init();            // Wykonywane przed każdym testem
    void cleanup();         // Wykonywane po każdym teście

    // Testy dla stacji
    void saveLoadStations_ValidData();
    void saveLoadStations_EmptyVector();
    void loadStations_NonExistentFile();
    void loadStations_InvalidJsonFile();

    // Testy dla czujników
    void saveLoadSensors_ValidData();
    void saveLoadSensors_EmptyVector();
    void loadSensors_NonExistentFile();
    void loadSensors_InvalidStationId();
    void loadSensors_MismatchedStationIdInFile();

    // Testy dla danych czujnika
    void saveLoadSensorData_ValidData();
    void saveLoadSensorData_WithNaN();
    void saveSensorData_EmptyData();
    void loadSensorData_NonExistentFile();

    // Testy dla indeksu AQI
    void saveLoadAQI_ValidData();
    void saveAQI_InvalidOrMismatchedId();
    void loadAQI_NonExistentFile();
    void loadAQI_MismatchedStationIdInFile();
};

#endif
