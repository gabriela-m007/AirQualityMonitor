#include "DataParser.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <limits>
#include <cmath>

DataParser::DataParser() {}


QString DataParser::getString(const QJsonObject& obj, const QString& key) {

    return obj.value(key).toString();
}

int DataParser::getInt(const QJsonObject& obj, const QString& key, int defaultValue) {
    if (!obj.contains(key)) {
        return defaultValue;
    }
    QJsonValue val = obj.value(key);
    if (val.isNull() || val.isUndefined()) {
        return defaultValue;
    }

    if (val.isDouble()) {
        return static_cast<int>(val.toDouble());
    } else if (val.isString()) {
        bool ok;
        int intVal = val.toString().toInt(&ok);
        return ok ? intVal : defaultValue;
    } else if (val.isObject() || val.isArray()) {
        return defaultValue;
    }

    return val.toInt(defaultValue);
}


double DataParser::getDouble(const QJsonObject& obj, const QString& key, double defaultValue) {

    if (!obj.contains(key) || obj.value(key).isNull() || obj.value(key).isUndefined()) {
        return defaultValue;
    }

    QJsonValue jsonVal = obj.value(key);

    if (!jsonVal.isDouble()) {
        qWarning() << "Value for key" << key << "is not stored as a number in JSON. Type:" << jsonVal.type();

        bool conversionOk = false;
        double convertedValue = jsonVal.toString().toDouble(&conversionOk);
        if (conversionOk) {
            return convertedValue;
        } else {
            return defaultValue;
        }
    }

    return jsonVal.toDouble();
}

IndexLevel DataParser::parseIndexLevel(const QJsonObject& obj, const QString& baseKey) {
    IndexLevel level;
    QJsonValue indexVal = obj.value(baseKey);
    if (indexVal.isObject()) {
        QJsonObject indexObj = indexVal.toObject();
        level.id = getInt(indexObj, "id", -1);
        level.indexLevelName = getString(indexObj, "indexLevelName");
    } else {

        level.id = -1;
        level.indexLevelName = "N/A";
    }
    return level;
}




std::vector<MeasuringStation> DataParser::parseStations(const QByteArray& jsonData) {
    std::vector<MeasuringStation> stations;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isArray()) {
        qWarning() << "parseStations: Input JSON is not an array.";
        return stations;
    }

    QJsonArray stationsArray = doc.array();
    for (const QJsonValue& value : stationsArray) {
        if (!value.isObject()) continue;
        QJsonObject stationObj = value.toObject();
        MeasuringStation station;

        station.id = getInt(stationObj, "id");
        station.stationName = getString(stationObj, "stationName");
        station.gegrLat = getDouble(stationObj, "gegrLat", 0.0);
        station.gegrLon = getDouble(stationObj, "gegrLon", 0.0);

        QJsonObject cityObj = stationObj.value("city").toObject();
        station.city.id = getInt(cityObj, "id");
        station.city.name = getString(cityObj, "name");
        //station.city.addressStreet = getString(stationObj.value("address").toObject(), "street");
        station.city.addressStreet = getString(cityObj, "addressStreet");

        QJsonObject commObj = cityObj.value("commune").toObject();
        station.city.commune.communeName = getString(commObj, "communeName");
        station.city.commune.districtName = getString(commObj, "districtName");
        station.city.commune.provinceName = getString(commObj, "provinceName");

        if (station.id != -1) {
            stations.push_back(station);
        } else {
            qWarning() << "parseStations: Skipped station with invalid ID.";
        }
    }
    return stations;
}

std::vector<Sensor> DataParser::parseSensors(const QByteArray& jsonData) {
    std::vector<Sensor> sensors;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isArray()) {
        qWarning() << "parseSensors: Input JSON is not an array.";
        return sensors;
    }

    QJsonArray sensorsArray = doc.array();
    for (const QJsonValue& value : sensorsArray) {
        if (!value.isObject()) continue;
        QJsonObject sensorObj = value.toObject();
        Sensor sensor;

        sensor.id = getInt(sensorObj, "id");
        sensor.stationId = getInt(sensorObj, "stationId");

        QJsonObject paramObj = sensorObj.value("param").toObject();
        sensor.param.paramName = getString(paramObj, "paramName");
        sensor.param.paramFormula = getString(paramObj, "paramFormula");
        sensor.param.paramCode = getString(paramObj, "paramCode");
        sensor.param.idParam = getInt(paramObj, "idParam");

        if (sensor.id != -1) {
            sensors.push_back(sensor);
        } else {
            qWarning() << "parseSensors: Skipped sensor with invalid ID.";
        }
    }
    return sensors;
}

SensorData DataParser::parseSensorData(const QByteArray& jsonData) {
    SensorData sensorData;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isObject()) {
        qWarning() << "parseSensorData: Input JSON is not an object.";
        return sensorData;
    }

    QJsonObject dataObj = doc.object();
    sensorData.key = getString(dataObj, "key");

    QJsonArray valuesArray = dataObj.value("values").toArray();
    for (const QJsonValue& value : valuesArray) {
        if (!value.isObject()) continue;
        QJsonObject valObj = value.toObject();

        MeasurementValue mv;
        QString dateStr = getString(valObj, "date");


        mv.date = QDateTime::fromString(dateStr, "yyyy-MM-dd HH:mm:ss");


        if (!mv.date.isValid()) {
            mv.date = QDateTime::fromString(dateStr, Qt::ISODate);
        }


        if (!mv.date.isValid()) {
            qWarning() << "parseSensorData: Could not parse date:" << dateStr << "with expected formats.";
            continue;
        }


        QJsonValue measurementVal = valObj.value("value");
        if (measurementVal.isNull() || measurementVal.isUndefined()) {
            mv.value = std::numeric_limits<double>::quiet_NaN();
            qDebug() << "parseSensorData: Null value found for date:" << dateStr;
        } else {
            mv.value = getDouble(valObj, "value", std::numeric_limits<double>::quiet_NaN());
            if (std::isnan(mv.value) && !measurementVal.isNull()) {
                qWarning() << "parseSensorData: Value for date:" << dateStr
                           << "was not null but parsed as NaN. Original value:" << measurementVal;
            }
        }
        sensorData.values.push_back(mv);
    }
    return sensorData;
}

AirQualityIndex DataParser::parseAirQualityIndex(const QByteArray& jsonData) {
    AirQualityIndex index;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isObject()) {
        qWarning() << "parseAirQualityIndex: Input JSON is not an object.";
        return index;
    }

    QJsonObject indexObj = doc.object();


    index.stationId = getInt(indexObj, "id");

    QString calcDateStr = getString(indexObj, "stCalcDate");
    QString sourceDateStr = getString(indexObj, "stSourceDataDate");


    index.stCalcDate = QDateTime::fromString(calcDateStr, "yyyy-MM-dd HH:mm:ss");
    if (!index.stCalcDate.isValid()) {
        index.stCalcDate = QDateTime::fromString(calcDateStr, Qt::ISODate);
    }
    index.stSourceDataDate = QDateTime::fromString(sourceDateStr, "yyyy-MM-dd HH:mm:ss");
    if (!index.stSourceDataDate.isValid()) {
        index.stSourceDataDate = QDateTime::fromString(sourceDateStr, Qt::ISODate);
    }


    index.stIndexLevel = parseIndexLevel(indexObj, "stIndexLevel");


    index.so2IndexLevel = parseIndexLevel(indexObj, "so2IndexLevel");
    index.no2IndexLevel = parseIndexLevel(indexObj, "no2IndexLevel");
    index.coIndexLevel = parseIndexLevel(indexObj, "coIndexLevel");
    index.pm10IndexLevel = parseIndexLevel(indexObj, "pm10IndexLevel");
    index.pm25IndexLevel = parseIndexLevel(indexObj, "pm25IndexLevel");
    index.o3IndexLevel = parseIndexLevel(indexObj, "o3IndexLevel");
    index.c6h6IndexLevel = parseIndexLevel(indexObj, "c6h6IndexLevel");

    if (index.stationId == -1) {
        qWarning() << "parseAirQualityIndex: Failed to parse valid station ID.";
    }

    return index;
}
