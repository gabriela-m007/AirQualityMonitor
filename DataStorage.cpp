#include "DataStorage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDebug>
#include <limits>
#include <cmath>

DataStorage::DataStorage(const QString& storagePath) : m_storagePath(storagePath)
{

    QDir dir(m_storagePath);
    if (!dir.exists()) {
        if(dir.mkpath(".")) {
            qInfo() << "Created storage directory:" << m_storagePath;
        } else {
            qWarning() << "Failed to create storage directory:" << m_storagePath;
        }
    }
}

QString DataStorage::getStoragePath() const
{
    return m_storagePath;
}

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

    obj["date"] = mv.date.isValid() ? mv.date.toString(Qt::ISODateWithMs) : QJsonValue();
    obj["value"] = std::isnan(mv.value) ? QJsonValue() : mv.value;

    return obj;
}

MeasurementValue DataStorage::measurementValueFromJson(const QJsonObject& obj) {
    MeasurementValue mv;
    mv.date = QDateTime::fromString(obj["date"].toString(), Qt::ISODateWithMs);
    QJsonValue val = obj["value"];

    if (val.isNull() || val.isUndefined()) {
        mv.value = std::numeric_limits<double>::quiet_NaN();
    } else {
        mv.value = val.toDouble(std::numeric_limits<double>::quiet_NaN());
    }
    if (!mv.date.isValid()) {
        qWarning() << "measurementValueFromJson: Failed to parse date:" << obj["date"].toString();
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

bool DataStorage::saveStationsToJson(const std::vector<MeasuringStation>& stations, const QString& filename) {
    QJsonArray stationsArray;
    for (const auto& station : stations) {
        stationsArray.append(stationToJson(station));
    }
    QJsonDocument doc(stationsArray);

    QFile file(m_storagePath + QDir::separator() + filename);
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
    QFile file(m_storagePath + QDir::separator() + filename);

    if (!file.exists()) {
        qInfo() << "Stations file does not exist:" << file.fileName();
        return stations;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << file.fileName() << file.errorString();
        return stations;
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
        qWarning() << "Cannot save empty SensorData (no key).";
        return false;
    }
    QJsonDocument doc(sensorDataToJson(data));

    QFile file(m_storagePath + QDir::separator() + filename);
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
    QFile file(m_storagePath + QDir::separator() + filename);

    if (!file.exists()) {
        qInfo() << "Sensor data file does not exist:" << file.fileName();
        return data;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << file.fileName() << file.errorString();
        return data;
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

bool DataStorage::saveSensorsToJson(int stationId, const std::vector<Sensor>& sensors) {
    if (stationId <= 0) {
        qWarning() << "Cannot save sensors, invalid stationId:" << stationId;
        return false;
    }
    QString filename = QString("station_%1_sensors.json").arg(stationId);
    QJsonArray sensorsArray;
    for (const auto& sensor : sensors) {
        sensorsArray.append(sensorToJson(sensor));
    }
    QJsonDocument doc(sensorsArray);

    QFile file(m_storagePath + QDir::separator() + filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for writing:" << file.fileName() << file.errorString();
        return false;
    }
    file.write(doc.toJson());
    file.close();
    qDebug() << "Sensors for station" << stationId << "saved to" << file.fileName();
    return true;
}

std::vector<Sensor> DataStorage::loadSensorsFromJson(int stationId) {
    std::vector<Sensor> sensors;
    if (stationId <= 0) {
        qWarning() << "Cannot load sensors, invalid stationId:" << stationId;
        return sensors;
    }
    QString filename = QString("station_%1_sensors.json").arg(stationId);
    QFile file(m_storagePath + QDir::separator() + filename);

    if (!file.exists()) {
        qInfo() << "Sensors file does not exist for station" << stationId << ":" << file.fileName();
        return sensors;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << file.fileName() << file.errorString();
        return sensors;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isArray()) {
        qWarning() << "Error parsing sensors JSON for station" << stationId << ": Not an array.";
        return sensors;
    }

    QJsonArray sensorsArray = doc.array();
    for (const QJsonValue& value : sensorsArray) {
        if (value.isObject()) {
            Sensor sensor = sensorFromJson(value.toObject());
            if (sensor.stationId == stationId || sensor.stationId == -1) {
                sensors.push_back(sensor);
            } else {
                qWarning() << "Sensor ID" << sensor.id << "in file" << filename << "has mismatching stationId" << sensor.stationId;
            }
        }
    }
    qDebug() << "Sensors for station" << stationId << "loaded from" << file.fileName() << "- Count:" << sensors.size();
    return sensors;
}

QJsonObject DataStorage::parameterToJson(const Parameter& param) {
    QJsonObject obj;
    obj["paramName"] = param.paramName;
    obj["paramFormula"] = param.paramFormula;
    obj["paramCode"] = param.paramCode;
    obj["idParam"] = param.idParam;
    return obj;
}

Parameter DataStorage::parameterFromJson(const QJsonObject& obj) {
    Parameter param;
    param.paramName = obj["paramName"].toString();
    param.paramFormula = obj["paramFormula"].toString();
    param.paramCode = obj["paramCode"].toString();
    param.idParam = obj["idParam"].toInt(-1);
    return param;
}

QJsonObject DataStorage::sensorToJson(const Sensor& sensor) {
    QJsonObject obj;
    obj["id"] = sensor.id;
    obj["stationId"] = sensor.stationId;
    obj["param"] = parameterToJson(sensor.param);
    return obj;
}

Sensor DataStorage::sensorFromJson(const QJsonObject& obj) {
    Sensor sensor;
    sensor.id = obj["id"].toInt(-1);
    sensor.stationId = obj["stationId"].toInt(-1);
    sensor.param = parameterFromJson(obj["param"].toObject());
    return sensor;
}

QJsonObject DataStorage::indexLevelToJson(const IndexLevel& level) {
    QJsonObject obj;
    obj["id"] = level.id;
    obj["indexLevelName"] = level.indexLevelName;
    return obj;
}

IndexLevel DataStorage::indexLevelFromJson(const QJsonObject& obj) {
    IndexLevel level;
    level.id = obj["id"].toInt(-1);
    level.indexLevelName = obj["indexLevelName"].toString("N/A");
    return level;
}

QJsonObject DataStorage::airQualityIndexToJson(const AirQualityIndex& index) {
    QJsonObject obj;
    obj["stationId"] = index.stationId;

    obj["stCalcDate"] = index.stCalcDate.isValid() ? index.stCalcDate.toString(Qt::ISODateWithMs) : QJsonValue();
    obj["stSourceDataDate"] = index.stSourceDataDate.isValid() ? index.stSourceDataDate.toString(Qt::ISODateWithMs) : QJsonValue();

    obj["stIndexLevel"] = indexLevelToJson(index.stIndexLevel);
    obj["so2IndexLevel"] = indexLevelToJson(index.so2IndexLevel);
    obj["no2IndexLevel"] = indexLevelToJson(index.no2IndexLevel);
    obj["coIndexLevel"] = indexLevelToJson(index.coIndexLevel);
    obj["pm10IndexLevel"] = indexLevelToJson(index.pm10IndexLevel);
    obj["pm25IndexLevel"] = indexLevelToJson(index.pm25IndexLevel);
    obj["o3IndexLevel"] = indexLevelToJson(index.o3IndexLevel);
    obj["c6h6IndexLevel"] = indexLevelToJson(index.c6h6IndexLevel);
    return obj;
}

AirQualityIndex DataStorage::airQualityIndexFromJson(const QJsonObject& obj) {
    AirQualityIndex index;
    index.stationId = obj["stationId"].toInt(-1);

    QString stCalcDateStr = obj["stCalcDate"].toString();
    QString stSourceDataDateStr = obj["stSourceDataDate"].toString();

    index.stCalcDate = QDateTime::fromString(stCalcDateStr, Qt::ISODateWithMs);
    index.stSourceDataDate = QDateTime::fromString(stSourceDataDateStr, Qt::ISODateWithMs);

    if (!index.stCalcDate.isValid() && !stCalcDateStr.isEmpty() && stCalcDateStr != "null") {
        qWarning() << "airQualityIndexFromJson: Failed to parse stCalcDate with ISODateWithMs format:" << stCalcDateStr;

        index.stCalcDate = QDateTime::fromString(stCalcDateStr, Qt::ISODate);
        if (!index.stCalcDate.isValid() && !stCalcDateStr.isEmpty() && stCalcDateStr != "null") {
            qWarning() << "airQualityIndexFromJson: Fallback to ISODate format also failed for stCalcDate:" << stCalcDateStr;
        }
    }
    if (!index.stSourceDataDate.isValid() && !stSourceDataDateStr.isEmpty() && stSourceDataDateStr != "null") {
        qWarning() << "airQualityIndexFromJson: Failed to parse stSourceDataDate with ISODateWithMs format:" << stSourceDataDateStr;

        index.stSourceDataDate = QDateTime::fromString(stSourceDataDateStr, Qt::ISODate);
        if (!index.stSourceDataDate.isValid() && !stSourceDataDateStr.isEmpty() && stSourceDataDateStr != "null") {
            qWarning() << "airQualityIndexFromJson: Fallback to ISODate format also failed for stSourceDataDate:" << stSourceDataDateStr;
        }
    }

    index.stIndexLevel = indexLevelFromJson(obj["stIndexLevel"].toObject());
    index.so2IndexLevel = indexLevelFromJson(obj["so2IndexLevel"].toObject());
    index.no2IndexLevel = indexLevelFromJson(obj["no2IndexLevel"].toObject());
    index.coIndexLevel = indexLevelFromJson(obj["coIndexLevel"].toObject());
    index.pm10IndexLevel = indexLevelFromJson(obj["pm10IndexLevel"].toObject());
    index.pm25IndexLevel = indexLevelFromJson(obj["pm25IndexLevel"].toObject());
    index.o3IndexLevel = indexLevelFromJson(obj["o3IndexLevel"].toObject());
    index.c6h6IndexLevel = indexLevelFromJson(obj["c6h6IndexLevel"].toObject());
    return index;
}

bool DataStorage::saveAirQualityIndexToJson(int stationId, const AirQualityIndex& index) {
    if (stationId <= 0 || index.stationId != stationId) {
        qWarning() << "Cannot save AQI, invalid stationId or mismatch:" << stationId << "vs" << index.stationId;
        return false;
    }
    QString filename = QString("station_%1_aqi.json").arg(stationId);
    QJsonDocument doc(airQualityIndexToJson(index));

    QFile file(m_storagePath + QDir::separator() + filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for writing:" << file.fileName() << file.errorString();
        return false;
    }
    file.write(doc.toJson());
    file.close();
    qDebug() << "AQI for station" << stationId << "saved to" << file.fileName();
    return true;
}

AirQualityIndex DataStorage::loadAirQualityIndexFromJson(int stationId) {
    AirQualityIndex index;
    if (stationId <= 0) {
        qWarning() << "Cannot load AQI, invalid stationId:" << stationId;
        return index;
    }
    QString filename = QString("station_%1_aqi.json").arg(stationId);
    QFile file(m_storagePath + QDir::separator() + filename);

    if (!file.exists()) {
        qInfo() << "AQI file does not exist for station" << stationId << ":" << file.fileName();
        return index;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << file.fileName() << file.errorString();
        return index;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        qWarning() << "Error parsing AQI JSON for station" << stationId << ": Not an object.";
        return index;
    }

    index = airQualityIndexFromJson(doc.object());
    if (index.stationId != stationId) {
        qWarning() << "Loaded AQI from" << filename << "has mismatching stationId" << index.stationId;
        index.stationId = -1;
        return index;
    }

    qDebug() << "AQI for station" << stationId << "loaded from" << file.fileName();
    return index;
}
