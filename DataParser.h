#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QByteArray>
#include <vector>
#include "DataStructures.h"

class DataParser
{
public:
    DataParser();

    std::vector<MeasuringStation> parseStations(const QByteArray& jsonData);
    std::vector<Sensor> parseSensors(const QByteArray& jsonData);
    SensorData parseSensorData(const QByteArray& jsonData);
    AirQualityIndex parseAirQualityIndex(const QByteArray& jsonData);

private:
    // Helper function to safely get string from JSON object
    QString getString(const QJsonObject& obj, const QString& key);
    // Helper function to safely get int
    int getInt(const QJsonObject& obj, const QString& key, int defaultValue = -1);
        // Helper function to safely get double
    double getDouble(const QJsonObject& obj, const QString& key, double defaultValue = 0.0);
        // Helper to parse IndexLevel structs
    IndexLevel parseIndexLevel(const QJsonObject& obj, const QString& baseKey);
};

#endif // DATAPARSER_H
