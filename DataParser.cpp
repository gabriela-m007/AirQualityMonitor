#include "DataParser.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <limits> // Required for quiet_NaN

DataParser::DataParser() {}

// --- Helper Functions ---
QString DataParser::getString(const QJsonObject& obj, const QString& key) {
    // Returns empty string if key doesn't exist, is null, or not a string
    return obj.value(key).toString();
}

int DataParser::getInt(const QJsonObject& obj, const QString& key, int defaultValue) {
    // Returns defaultValue if key doesn't exist, is null, or not a number
    int val = obj.value(key).toInt(defaultValue);
    // QJsonValue::toInt returns 0 on failure, check if it was actually 0 or null
    if (obj.value(key).isNull() || obj.value(key).isUndefined()) return defaultValue;
    // Optional: Check if conversion was truly successful if 0 is valid
    // double checkVal = obj.value(key).toDouble(&ok);
    // if(!ok) return defaultValue;
    return val;
}


double DataParser::getDouble(const QJsonObject& obj, const QString& key, double defaultValue) {
    // Sprawdź, czy klucz istnieje i czy wartość nie jest null lub undefined
    if (!obj.contains(key) || obj.value(key).isNull() || obj.value(key).isUndefined()) {
        return defaultValue;
    }

    QJsonValue jsonVal = obj.value(key);

    return jsonVal.toDouble(defaultValue);

    if (!jsonVal.isDouble()) { // Sprawdza, czy typ JSON to faktycznie liczba
         qWarning() << "Value for key" << key << "is not stored as a number in JSON. Type:" << jsonVal.type();
        return defaultValue;
    }
    // Jeśli isDouble() jest true, to toDouble() bez argumentów jest bezpieczne.
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
        // Handle cases where the index level might be null or missing
        level.id = -1; // Mark as invalid/not present
        level.indexLevelName = "N/A";
    }
    return level;
}


// --- Main Parsing Functions ---

std::vector<MeasuringStation> DataParser::parseStations(const QByteArray& jsonData) {
    std::vector<MeasuringStation> stations;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isArray()) {
        qWarning() << "parseStations: Input JSON is not an array.";
        return stations; // Return empty vector
    }

    QJsonArray stationsArray = doc.array();
    for (const QJsonValue& value : stationsArray) {
        if (!value.isObject()) continue;
        QJsonObject stationObj = value.toObject();
        MeasuringStation station;

        station.id = getInt(stationObj, "id");
        station.stationName = getString(stationObj, "stationName");
        station.gegrLat = getString(stationObj, "gegrLat").toDouble(); // API returns string
        station.gegrLon = getString(stationObj, "gegrLon").toDouble(); // API returns string

        QJsonObject cityObj = stationObj.value("city").toObject();
        station.city.id = getInt(cityObj, "id");
        station.city.name = getString(cityObj, "name");
        station.city.addressStreet = getString(cityObj, "addressStreet"); // Get addressStreet

        QJsonObject commObj = cityObj.value("commune").toObject();
        station.city.commune.communeName = getString(commObj, "communeName");
        station.city.commune.districtName = getString(commObj, "districtName");
        station.city.commune.provinceName = getString(commObj, "provinceName");

        if (station.id != -1) { // Basic validation: ensure ID was parsed
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
        sensor.param.idParam = getInt(paramObj, "idParam"); // Parsing idParam

        if (sensor.id != -1) { // Basic validation
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
        return sensorData; // Return empty data
    }

    QJsonObject dataObj = doc.object();
    sensorData.key = getString(dataObj, "key"); // e.g., "PM10"

    QJsonArray valuesArray = dataObj.value("values").toArray();
    for (const QJsonValue& value : valuesArray) {
        if (!value.isObject()) continue;
        QJsonObject valObj = value.toObject();

        MeasurementValue mv;
        QString dateStr = getString(valObj, "date");

        // *** POPRAWIONY KOD PARSOWANIA DATY ***
        // Najpierw spróbuj formatu zaobserwowanego w API: "yyyy-MM-dd HH:mm:ss"
        mv.date = QDateTime::fromString(dateStr, "yyyy-MM-dd HH:mm:ss");

        // Jeśli pierwszy format zawiedzie (na wypadek gdyby API zmieniło format),
        // spróbuj standardowego ISO jako fallback
        if (!mv.date.isValid()) {
            mv.date = QDateTime::fromString(dateStr, Qt::ISODate);
        }

        // Jeśli oba formaty zawiodły, zaloguj błąd i pomiń wartość
        if (!mv.date.isValid()) {
            qWarning() << "parseSensorData: Could not parse date:" << dateStr << "with expected formats.";
            continue; // Pomiń tę wartość
        }
        // *** KONIEC POPRAWIONEGO KODU PARSOWANIA DATY ***


        // Reszta kodu (parsowanie wartości) pozostaje bez zmian:
        QJsonValue measurementVal = valObj.value("value");
        if (measurementVal.isNull() || measurementVal.isUndefined()) {
            mv.value = std::numeric_limits<double>::quiet_NaN(); // Store NaN for null
            qDebug() << "parseSensorData: Null value found for date:" << dateStr;
        } else {
            mv.value = measurementVal.toDouble(std::numeric_limits<double>::quiet_NaN());
            if (std::isnan(mv.value) && !measurementVal.isDouble() && !measurementVal.isNull()) { // Dodatkowe sprawdzenie !isNull
                qWarning() << "parseSensorData: Value for date:" << dateStr
                           << "was not stored as number, parsed as NaN. Original value:" << measurementVal;
            }
        }
        sensorData.values.push_back(mv); // Teraz wartości powinny być dodawane
    }
    return sensorData;
}

AirQualityIndex DataParser::parseAirQualityIndex(const QByteArray& jsonData) {
    AirQualityIndex index;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isObject()) {
        qWarning() << "parseAirQualityIndex: Input JSON is not an object.";
        return index; // Return default (invalid) index
    }

    QJsonObject indexObj = doc.object();

    // GIOŚ API returns station ID as "id" at the top level of the index object
    index.stationId = getInt(indexObj, "id");

    QString calcDateStr = getString(indexObj, "stCalcDate");
    QString sourceDateStr = getString(indexObj, "stSourceDataDate");

    // Use Qt::ISODate if the format is like "2024-01-15T10:00:00Z" or similar
    // Adjust format string "yyyy-MM-dd HH:mm:ss" if needed
    index.stCalcDate = QDateTime::fromString(calcDateStr, Qt::ISODate);
    index.stSourceDataDate = QDateTime::fromString(sourceDateStr, Qt::ISODate);


    // Parse the main ("st") index level
    index.stIndexLevel = parseIndexLevel(indexObj, "stIndexLevel");

    // Parse specific parameter index levels (handle potential nulls)
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
