#include "DataStorage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDebug>
#include <limits> // For NaN comparison


DataStorage::DataStorage(const QString& storagePath) : m_storagePath(storagePath)
{
    // Ensure storage directory exists
    QDir dir(m_storagePath);
    if (!dir.exists()) {
        dir.mkpath("."); // Create directory if it doesn't exist
    }
}

QString DataStorage::getStoragePath() const
{
    return m_storagePath;
}

// --- JSON Helper Function Implementations ---

QJsonObject DataStorage::communeToJson(const Commune& commune) {
    QJsonObject obj;
    obj["communeName"] = commune.communeName;
    obj["districtName"] = commune.districtName;
    obj["provinceName"] = commune.provinceName;
    return obj;
}

Commune DataStorage::communeFromJson(const QJsonObject& obj) {
    Commune commune;
    commune.communeName = obj["communeName"].toString();
    commune.districtName = obj["districtName"].toString();
    commune.provinceName = obj["provinceName"].toString();
    return commune;
}

QJsonObject DataStorage::cityToJson(const City& city) {
    QJsonObject obj;
    obj["id"] = city.id;
    obj["name"] = city.name;
    obj["commune"] = communeToJson(city.commune);
    obj["addressStreet"] = city.addressStreet;
    return obj;
}

City DataStorage::cityFromJson(const QJsonObject& obj) {
    City city;
    city.id = obj["id"].toInt(-1);
    city.name = obj["name"].toString();
    city.commune = communeFromJson(obj["commune"].toObject());
    city.addressStreet = obj["addressStreet"].toString();
    return city;
}


QJsonObject DataStorage::stationToJson(const MeasuringStation& station) {
    QJsonObject obj;
    obj["id"] = station.id;
    obj["stationName"] = station.stationName;
    obj["gegrLat"] = station.gegrLat;
    obj["gegrLon"] = station.gegrLon;
    obj["city"] = cityToJson(station.city);
    return obj;
}

MeasuringStation DataStorage::stationFromJson(const QJsonObject& obj) {
    MeasuringStation station;
    station.id = obj["id"].toInt(-1);
    station.stationName = obj["stationName"].toString();
    station.gegrLat = obj["gegrLat"].toDouble();
    station.gegrLon = obj["gegrLon"].toDouble();
    station.city = cityFromJson(obj["city"].toObject());
    return station;
}

QJsonObject DataStorage::measurementValueToJson(const MeasurementValue& mv) {
    QJsonObject obj;
    // Store date as ISO string for consistency
    obj["date"] = mv.date.isValid() ? mv.date.toString(Qt::ISODate) : QJsonValue();
    // Store value, handle NaN by storing null
    obj["value"] = std::isnan(mv.value) ? QJsonValue() : mv.value;
    return obj;
}

MeasurementValue DataStorage::measurementValueFromJson(const QJsonObject& obj) {
    MeasurementValue mv;
    mv.date = QDateTime::fromString(obj["date"].toString(), Qt::ISODate);
    QJsonValue val = obj["value"];
    if (val.isNull() || val.isUndefined()) {
        mv.value = std::numeric_limits<double>::quiet_NaN();
    } else {
        mv.value = val.toDouble(std::numeric_limits<double>::quiet_NaN());
    }
    return mv;
}


QJsonObject DataStorage::sensorDataToJson(const SensorData& data) {
    QJsonObject obj;
    obj["key"] = data.key;
    QJsonArray valuesArray;
    for(const auto& mv : data.values) {
        valuesArray.append(measurementValueToJson(mv));
    }
    obj["values"] = valuesArray;
    return obj;
}

SensorData DataStorage::sensorDataFromJson(const QJsonObject& obj) {
    SensorData data;
    data.key = obj["key"].toString();
    QJsonArray valuesArray = obj["values"].toArray();
    for(const QJsonValue& val : valuesArray) {
        if (val.isObject()) {
            data.values.push_back(measurementValueFromJson(val.toObject()));
        }
    }
    return data;
}


// --- Main Save/Load Functions ---

bool DataStorage::saveStationsToJson(const std::vector<MeasuringStation>& stations, const QString& filename) {
    QJsonArray stationsArray;
    for (const auto& station : stations) {
        stationsArray.append(stationToJson(station));
    }
    QJsonDocument doc(stationsArray);

    QFile file(m_storagePath + "/" + filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for writing:" << file.fileName() << file.errorString();
        return false;
    }
    file.write(doc.toJson());
    file.close();
    qDebug() << "Stations saved to" << file.fileName();
    return true;
}

std::vector<MeasuringStation> DataStorage::loadStationsFromJson(const QString& filename) {
    std::vector<MeasuringStation> stations;
    QFile file(m_storagePath + "/" + filename);

    if (!file.exists()) {
        qInfo() << "Stations file does not exist:" << file.fileName();
        return stations; // Return empty if file doesn't exist
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << file.fileName() << file.errorString();
        return stations; // Return empty on error
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isArray()) {
        qWarning() << "Error parsing stations JSON: Not an array.";
        return stations;
    }

    QJsonArray stationsArray = doc.array();
    for (const QJsonValue& value : stationsArray) {
        if (value.isObject()) {
            stations.push_back(stationFromJson(value.toObject()));
        }
    }
    qDebug() << "Stations loaded from" << file.fileName() << "- Count:" << stations.size();
    return stations;
}


bool DataStorage::saveSensorDataToJson(const SensorData& data, const QString& filename) {
    if (data.key.isEmpty()) {
        qWarning() << "Cannot save empty SensorData.";
        return false;
    }
    QJsonDocument doc(sensorDataToJson(data));

    QFile file(m_storagePath + "/" + filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for writing:" << file.fileName() << file.errorString();
        return false;
    }
    file.write(doc.toJson());
    file.close();
    qDebug() << "Sensor data saved to" << file.fileName();
    return true;
}

SensorData DataStorage::loadSensorDataFromJson(const QString& filename) {
    SensorData data;
    QFile file(m_storagePath + "/" + filename);

    if (!file.exists()) {
        qInfo() << "Sensor data file does not exist:" << file.fileName();
        return data; // Return empty
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << file.fileName() << file.errorString();
        return data; // Return empty
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        qWarning() << "Error parsing sensor data JSON: Not an object.";
        return data;
    }

    data = sensorDataFromJson(doc.object());
    qDebug() << "Sensor data loaded from" << file.fileName() << "- Key:" << data.key << "Values:" << data.values.size();
    return data;
}


// --- XML Stubs (Implement later if needed) ---
/*
bool DataStorage::saveStationsToXml(...) { ... return false; }
std::vector<MeasuringStation> DataStorage::loadStationsFromXml(...) { ... return {}; }
*/
