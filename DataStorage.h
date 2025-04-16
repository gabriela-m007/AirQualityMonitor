#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <QString>
#include <vector>
#include "DataStructures.h"

class DataStorage
{
public:
    DataStorage(const QString& storagePath = "."); // Store in current dir by default

    // --- JSON Storage ---
    bool saveStationsToJson(const std::vector<MeasuringStation>& stations, const QString& filename = "stations.json");
    std::vector<MeasuringStation> loadStationsFromJson(const QString& filename = "stations.json");

    // Save sensor data for ONE sensor. Filename could be sensorId based.
    bool saveSensorDataToJson(const SensorData& data, const QString& filename);
    SensorData loadSensorDataFromJson(const QString& filename);

    // Could add methods for saving/loading AQI, Sensors list etc. if needed

    // --- XML Storage (Optional - Add later if needed) ---
    // bool saveStationsToXml(const std::vector<MeasuringStation>& stations, const QString& filename = "stations.xml");
    // std::vector<MeasuringStation> loadStationsFromXml(const QString& filename = "stations.xml");
    // ... other XML methods

    QString getStoragePath() const;

private:
    QString m_storagePath;

    // JSON Helper functions (can be moved to DataParser or kept here)
    QJsonObject stationToJson(const MeasuringStation& station);
    MeasuringStation stationFromJson(const QJsonObject& obj);
    QJsonObject sensorDataToJson(const SensorData& data);
    SensorData sensorDataFromJson(const QJsonObject& obj);
    // Add helpers for nested structs (City, Commune, Parameter, MeasurementValue)
    QJsonObject cityToJson(const City& city);
    City cityFromJson(const QJsonObject& obj);
    QJsonObject communeToJson(const Commune& commune);
    Commune communeFromJson(const QJsonObject& obj);
    QJsonObject parameterToJson(const Parameter& param);
    Parameter parameterFromJson(const QJsonObject& obj);
    QJsonObject measurementValueToJson(const MeasurementValue& mv);
    MeasurementValue measurementValueFromJson(const QJsonObject& obj);
};

#endif // DATASTORAGE_H
