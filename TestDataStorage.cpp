#include "testdatastorage.h"
#include <QFile>
#include <QJsonDocument>
#include <limits>
#include <cmath>

TestDataStorage::TestDataStorage() {}

std::vector<MeasuringStation> TestDataStorage::createTestStations(int count) {
    std::vector<MeasuringStation> stations;
    for (int i = 1; i <= count; ++i) {
        MeasuringStation s;
        s.id = i * 100;
        s.stationName = QString("Test Station %1").arg(i);
        s.gegrLat = 50.0 + i;
        s.gegrLon = 20.0 + i;
        s.city.id = i * 10;
        s.city.name = QString("City %1").arg(i);
        s.city.commune.communeName = QString("Commune %1").arg(i);
        s.city.commune.districtName = QString("District %1").arg(i);
        s.city.commune.provinceName = QString("Province %1").arg(i);
        s.city.addressStreet = QString("Street %1").arg(i);
        stations.push_back(s);
    }
    return stations;
}

std::vector<Sensor> TestDataStorage::createTestSensors(int stationId, int count) {
    std::vector<Sensor> sensors;
    for (int i = 1; i <= count; ++i) {
        Sensor s;
        s.id = stationId * 10 + i;
        s.stationId = stationId;
        s.param.idParam = i;
        s.param.paramCode = QString("P%1").arg(i);
        s.param.paramFormula = QString("Param%1").arg(i);
        s.param.paramName = QString("Parameter %1").arg(i);
        sensors.push_back(s);
    }
    return sensors;
}

SensorData TestDataStorage::createTestSensorData(const QString& key) {
    SensorData sd;
    sd.key = key;
    QDateTime dt = QDateTime::currentDateTime();
    sd.values.push_back({dt.addSecs(-3600), 10.5});
    sd.values.push_back({dt, std::numeric_limits<double>::quiet_NaN()});
    sd.values.push_back({dt.addSecs(3600), 12.0});
    return sd;
}

AirQualityIndex TestDataStorage::createTestAQI(int stationId) {
    AirQualityIndex aqi;
    aqi.stationId = stationId;
    aqi.stCalcDate = QDateTime::currentDateTime();
    aqi.stSourceDataDate = QDateTime::currentDateTime().addSecs(-600);
    aqi.stIndexLevel = {1, "Dobry"};
    aqi.pm10IndexLevel = {1, "Dobry"};
    aqi.pm25IndexLevel = {2, "Umiarkowany"};
    aqi.so2IndexLevel = {-1, "N/A"};
    return aqi;
}

void TestDataStorage::initTestCase() {
    // Wywoływane raz przed wszystkimi testami w tej klasie
    QVERIFY(tempDir.isValid());
    qInfo() << "Używanie katalogu tymczasowego:" << tempDir.path();
    storage = new DataStorage(tempDir.path());
    QVERIFY(storage != nullptr);
}

void TestDataStorage::cleanupTestCase() {
    delete storage;
    storage = nullptr;
    qInfo() << "Katalog tymczasowy powinien zostać usunięty.";
}

void TestDataStorage::init() {}

void TestDataStorage::cleanup() {}

// Testy dla stacji

void TestDataStorage::saveLoadStations_ValidData() {
    std::vector<MeasuringStation> originalStations = createTestStations(3);
    QString filename = "test_stations.json";

    // Zapisz
    QVERIFY(storage->saveStationsToJson(originalStations, filename));

    // Wczytaj
    std::vector<MeasuringStation> loadedStations = storage->loadStationsFromJson(filename);

    // Porównaj
    QCOMPARE(loadedStations.size(), originalStations.size());
    if (loadedStations.size() == originalStations.size()) {
        for (size_t i = 0; i < loadedStations.size(); ++i) {
            QCOMPARE(loadedStations[i].id, originalStations[i].id);
            QCOMPARE(loadedStations[i].stationName, originalStations[i].stationName);
            QCOMPARE(loadedStations[i].city.name, originalStations[i].city.name);
        }
    }
}

void TestDataStorage::saveLoadStations_EmptyVector() {
    std::vector<MeasuringStation> originalStations;
    QString filename = "empty_stations.json";

    QVERIFY(storage->saveStationsToJson(originalStations, filename));
    std::vector<MeasuringStation> loadedStations = storage->loadStationsFromJson(filename);
    QVERIFY(loadedStations.empty());
}

void TestDataStorage::loadStations_NonExistentFile() {
    std::vector<MeasuringStation> loadedStations = storage->loadStationsFromJson("non_existent_file.json");
    QVERIFY(loadedStations.empty());
}

void TestDataStorage::loadStations_InvalidJsonFile() {
    QString filename = "invalid_stations.json";
    QFile file(tempDir.filePath(filename));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write("[{\"id\": 1, ]");
        file.close();
    } else {
        QFAIL("Nie można utworzyć pliku z niepoprawnym JSON");
    }

    std::vector<MeasuringStation> loadedStations = storage->loadStationsFromJson(filename);
    QVERIFY(loadedStations.empty());
}

// Testy dla czujników

void TestDataStorage::saveLoadSensors_ValidData() {
    int stationId = 123;
    std::vector<Sensor> originalSensors = createTestSensors(stationId, 2);

    QVERIFY(storage->saveSensorsToJson(stationId, originalSensors));
    std::vector<Sensor> loadedSensors = storage->loadSensorsFromJson(stationId);

    QCOMPARE(loadedSensors.size(), originalSensors.size());
    if (loadedSensors.size() == originalSensors.size()) {
        QCOMPARE(loadedSensors[0].id, originalSensors[0].id);
        QCOMPARE(loadedSensors[0].stationId, originalSensors[0].stationId);
        QCOMPARE(loadedSensors[0].param.paramCode, originalSensors[0].param.paramCode);
        QCOMPARE(loadedSensors[1].id, originalSensors[1].id);
    }
}

void TestDataStorage::saveLoadSensors_EmptyVector() {
    int stationId = 456;
    std::vector<Sensor> originalSensors;

    QVERIFY(storage->saveSensorsToJson(stationId, originalSensors));
    std::vector<Sensor> loadedSensors = storage->loadSensorsFromJson(stationId);
    QVERIFY(loadedSensors.empty());
}

void TestDataStorage::loadSensors_NonExistentFile() {
    std::vector<Sensor> loadedSensors = storage->loadSensorsFromJson(999);
    QVERIFY(loadedSensors.empty());
}

void TestDataStorage::loadSensors_InvalidStationId() {
    std::vector<Sensor> loadedSensors = storage->loadSensorsFromJson(0);
    QVERIFY(loadedSensors.empty());
    loadedSensors = storage->loadSensorsFromJson(-10);
    QVERIFY(loadedSensors.empty());
}

void TestDataStorage::loadSensors_MismatchedStationIdInFile() {
    int correctStationId = 777;
    int wrongStationId = 888;
    std::vector<Sensor> sensorsForWrongStation = createTestSensors(wrongStationId);

    QString filename = QString("station_%1_sensors.json").arg(correctStationId);
    QJsonArray sensorsArray;
    for (const auto& sensor : sensorsForWrongStation) {
        QJsonObject sObj;
        sObj["id"] = sensor.id;
        sObj["stationId"] = sensor.stationId;
        QJsonObject pObj;
        pObj["paramCode"] = sensor.param.paramCode;
        sObj["param"] = pObj;
        sensorsArray.append(sObj);
    }
    QJsonDocument doc(sensorsArray);
    QFile file(tempDir.filePath(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QFAIL("Nie można otworzyć pliku do zapisu dla testu niezgodności ID");
    }
    file.write(doc.toJson());
    file.close();

    std::vector<Sensor> loadedSensors = storage->loadSensorsFromJson(correctStationId);
    QVERIFY(loadedSensors.empty());
}


// Testy dla danych czujnika

void TestDataStorage::saveLoadSensorData_ValidData() {
    SensorData originalData = createTestSensorData("PM10");
    QString filename = "test_sensordata_pm10.json";

    QVERIFY(storage->saveSensorDataToJson(originalData, filename));
    SensorData loadedData = storage->loadSensorDataFromJson(filename);

    QCOMPARE(loadedData.key, originalData.key);
    QCOMPARE(loadedData.values.size(), originalData.values.size());
    if(loadedData.values.size() == originalData.values.size()) {
        QCOMPARE(loadedData.values[0].date, originalData.values[0].date);
        QCOMPARE(loadedData.values[0].value, originalData.values[0].value);
        QVERIFY(std::isnan(loadedData.values[1].value));
        QVERIFY(std::isnan(originalData.values[1].value));
        QCOMPARE(loadedData.values[2].date, originalData.values[2].date);
        QCOMPARE(loadedData.values[2].value, originalData.values[2].value);
    }
}

void TestDataStorage::saveLoadSensorData_WithNaN() {

    SensorData originalData;
    originalData.key = "NaN_Test";
    originalData.values.push_back({QDateTime::currentDateTime(), std::numeric_limits<double>::quiet_NaN()});
    QString filename = "test_sensordata_nan.json";

    QVERIFY(storage->saveSensorDataToJson(originalData, filename));
    SensorData loadedData = storage->loadSensorDataFromJson(filename);

    QCOMPARE(loadedData.key, originalData.key);
    QCOMPARE(loadedData.values.size(), 1);
    if(!loadedData.values.empty()) {
        QVERIFY(std::isnan(loadedData.values[0].value));
    }
}


void TestDataStorage::saveSensorData_EmptyData() {
    SensorData emptyData;
    QString filename = "should_not_save.json";
    QVERIFY(!storage->saveSensorDataToJson(emptyData, filename));
    QFile file(tempDir.filePath(filename));
    QVERIFY(!file.exists());

    emptyData.key = "HasKey";
    QVERIFY(storage->saveSensorDataToJson(emptyData, filename));
    QVERIFY(file.exists());
    SensorData loadedData = storage->loadSensorDataFromJson(filename);
    QCOMPARE(loadedData.key, QString("HasKey"));
    QVERIFY(loadedData.values.empty());
}

void TestDataStorage::loadSensorData_NonExistentFile() {
    SensorData loadedData = storage->loadSensorDataFromJson("non_existent_sensor_data.json");
    QVERIFY(loadedData.key.isEmpty());
    QVERIFY(loadedData.values.empty());
}


// Testy dla indeksu AQI

void TestDataStorage::saveLoadAQI_ValidData() {
    int stationId = 222;
    AirQualityIndex originalAQI = createTestAQI(stationId);

    QVERIFY(storage->saveAirQualityIndexToJson(stationId, originalAQI));
    AirQualityIndex loadedAQI = storage->loadAirQualityIndexFromJson(stationId);

    QCOMPARE(loadedAQI.stationId, originalAQI.stationId);
    QCOMPARE(loadedAQI.stCalcDate, originalAQI.stCalcDate);
    QCOMPARE(loadedAQI.stIndexLevel.id, originalAQI.stIndexLevel.id);
    QCOMPARE(loadedAQI.stIndexLevel.indexLevelName, originalAQI.stIndexLevel.indexLevelName);
    QCOMPARE(loadedAQI.pm10IndexLevel.id, originalAQI.pm10IndexLevel.id);
    QCOMPARE(loadedAQI.so2IndexLevel.id, originalAQI.so2IndexLevel.id);
    QCOMPARE(loadedAQI.so2IndexLevel.indexLevelName, originalAQI.so2IndexLevel.indexLevelName);
}

void TestDataStorage::saveAQI_InvalidOrMismatchedId() {
    int stationId = 333;
    AirQualityIndex aqi = createTestAQI(stationId);
    QString filename = QString("station_%1_aqi.json").arg(stationId);
    QString wrongFilename = QString("station_%1_aqi.json").arg(444);

    QVERIFY(!storage->saveAirQualityIndexToJson(0, aqi));
    QVERIFY(!storage->saveAirQualityIndexToJson(-1, aqi));

    aqi.stationId = 444;
    QVERIFY(!storage->saveAirQualityIndexToJson(stationId, aqi));

    QFile file(tempDir.filePath(filename));
    QVERIFY(!file.exists());
    QFile wrongFile(tempDir.filePath(wrongFilename));
    QVERIFY(!wrongFile.exists());
}

void TestDataStorage::loadAQI_NonExistentFile() {
    AirQualityIndex loadedAQI = storage->loadAirQualityIndexFromJson(1111);
    QCOMPARE(loadedAQI.stationId, -1);
}

void TestDataStorage::loadAQI_MismatchedStationIdInFile() {
    int correctStationId = 555;
    int wrongStationId = 666;
    AirQualityIndex aqiForWrongStation = createTestAQI(wrongStationId);

    QString filename = QString("station_%1_aqi.json").arg(correctStationId);

    QJsonObject aqiJson;
    aqiJson["stationId"] = aqiForWrongStation.stationId;
    QJsonDocument doc(aqiJson);
    QFile file(tempDir.filePath(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QFAIL("Nie można otworzyć pliku do zapisu dla testu niezgodności ID AQI");
    }
    file.write(doc.toJson());
    file.close();

    AirQualityIndex loadedAQI = storage->loadAirQualityIndexFromJson(correctStationId);
    QCOMPARE(loadedAQI.stationId, -1);
}
