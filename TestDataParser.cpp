#include "testdataparser.h"
#include <limits>
#include <cmath>

void TestDataParser::parseStations_ValidData()
{
    QByteArray jsonData = R"(
        [
            {
                "id": 123,
                "stationName": "Test Station Alpha",
                "gegrLat": "51.000",
                "gegrLon": "21.500",
                "city": {
                    "id": 10,
                    "name": "TestCity",
                    "commune": {
                        "communeName": "TestCommune",
                        "districtName": "TestDistrict",
                        "provinceName": "TestProvince"
                    }
                },
                "addressStreet": "Test Street 1/A"
            },
            {
                "id": 456,
                "stationName": "Test Station Beta",
                "gegrLat": "52",
                "gegrLon": "22",
                "city": {
                    "id": 11,
                    "name": "AnotherCity",
                    "commune": {
                        "communeName": "AnotherCommune",
                        "districtName": "AnotherDistrict",
                        "provinceName": "AnotherProvince"
                    }
                },
                "addressStreet": null
            }
        ]
    )";

    std::vector<MeasuringStation> stations = parser.parseStations(jsonData);

    QCOMPARE(stations.size(), 2);
    if (stations.size() == 2) {
        // Stacja 1
        QCOMPARE(stations[0].id, 123);
        QCOMPARE(stations[0].stationName, QString("Test Station Alpha"));
        QCOMPARE(stations[0].gegrLat, 51.0);
        QCOMPARE(stations[0].gegrLon, 21.5);
        QCOMPARE(stations[0].city.id, 10);
        QCOMPARE(stations[0].city.name, QString("TestCity"));
        QCOMPARE(stations[0].city.commune.communeName, QString("TestCommune"));
        QCOMPARE(stations[0].city.commune.districtName, QString("TestDistrict"));
        QCOMPARE(stations[0].city.commune.provinceName, QString("TestProvince"));

        // Stacja 2
        QCOMPARE(stations[1].id, 456);
        QCOMPARE(stations[1].stationName, QString("Test Station Beta"));
        QCOMPARE(stations[1].gegrLat, 52.0);
        QCOMPARE(stations[1].gegrLon, 22.0);
        QCOMPARE(stations[1].city.name, QString("AnotherCity"));
        QCOMPARE(stations[1].city.addressStreet, QString());
    }
}

void TestDataParser::parseStations_EmptyArray()
{
    QByteArray jsonData = "[]";
    std::vector<MeasuringStation> stations = parser.parseStations(jsonData);
    QVERIFY(stations.empty());
}

void TestDataParser::parseStations_MalformedJson()
{
    QByteArray jsonData = "[{\"id\": 1}";
    std::vector<MeasuringStation> stations = parser.parseStations(jsonData);
    QVERIFY(stations.empty());
}

void TestDataParser::parseStations_MissingFields()
{
    QByteArray jsonData = R"(
        [
            {
                "id": 789,
                "stationName": "Incomplete Station",
                "city": {
                    "name": "PartialCity"
                    // Brak commune, id, addressStreet
                }
                // Brak gegrLat, gegrLon
            }
        ]
    )";
    std::vector<MeasuringStation> stations = parser.parseStations(jsonData);
    QVERIFY(stations.empty());
    if(!stations.empty()) {
        QCOMPARE(stations[0].id, 789);
        QCOMPARE(stations[0].stationName, QString("Incomplete Station"));
        QCOMPARE(stations[0].gegrLat, 0.0);
        QCOMPARE(stations[0].gegrLon, 0.0);
        QCOMPARE(stations[0].city.id, -1);
        QCOMPARE(stations[0].city.name, QString("PartialCity"));
        QCOMPARE(stations[0].city.addressStreet, QString());
        QCOMPARE(stations[0].city.commune.communeName, QString());
    }
}


// Testy dla parseSensors

void TestDataParser::parseSensors_ValidData()
{
    QByteArray jsonData = R"(
        [
            {
                "id": 101,
                "stationId": 50,
                "param": {
                    "paramName": "Dwutlenek Azotu",
                    "paramFormula": "NO2",
                    "paramCode": "NO2",
                    "idParam": 6
                }
            },
            {
                "id": 102,
                "stationId": 50,
                "param": {
                    "paramName": "Pył zawieszony PM10",
                    "paramFormula": "PM10",
                    "paramCode": "PM10",
                    "idParam": 3
                }
            }
        ]
    )";
    std::vector<Sensor> sensors = parser.parseSensors(jsonData);
    QCOMPARE(sensors.size(), 2);
    if(sensors.size() == 2) {
        QCOMPARE(sensors[0].id, 101);
        QCOMPARE(sensors[0].stationId, 50);
        QCOMPARE(sensors[0].param.paramName, QString("Dwutlenek Azotu"));
        QCOMPARE(sensors[0].param.paramFormula, QString("NO2"));
        QCOMPARE(sensors[0].param.paramCode, QString("NO2"));
        QCOMPARE(sensors[0].param.idParam, 6);

        QCOMPARE(sensors[1].id, 102);
        QCOMPARE(sensors[1].stationId, 50);
        QCOMPARE(sensors[1].param.paramFormula, QString("PM10"));
        QCOMPARE(sensors[1].param.idParam, 3);
    }
}

void TestDataParser::parseSensors_EmptyArray()
{
    QByteArray jsonData = "[]";
    std::vector<Sensor> sensors = parser.parseSensors(jsonData);
    QVERIFY(sensors.empty());
}

void TestDataParser::parseSensors_MalformedJson()
{
    QByteArray jsonData = "[{\"id\": 101, ]";
    std::vector<Sensor> sensors = parser.parseSensors(jsonData);
    QVERIFY(sensors.empty());
}


// Testy dla parseSensorData

void TestDataParser::parseSensorData_ValidData()
{
    QByteArray jsonData = R"(
        {
            "key": "PM10",
            "values": [
                { "date": "2024-05-10 12:00:00", "value": 45.6 },
                { "date": "2024-05-10 13:00:00", "value": 50.1 }
            ]
        }
    )";
    SensorData data = parser.parseSensorData(jsonData);
    QCOMPARE(data.key, QString("PM10"));
    QCOMPARE(data.values.size(), 2);
    if(data.values.size() == 2) {
        QCOMPARE(data.values[0].date, QDateTime::fromString("2024-05-10 12:00:00", "yyyy-MM-dd HH:mm:ss"));
        QCOMPARE(data.values[0].value, 45.6);
        QCOMPARE(data.values[1].date, QDateTime::fromString("2024-05-10 13:00:00", "yyyy-MM-dd HH:mm:ss"));
        QCOMPARE(data.values[1].value, 50.1);
    }
}

void TestDataParser::parseSensorData_EmptyValues()
{
    QByteArray jsonData = R"({ "key": "SO2", "values": [] })";
    SensorData data = parser.parseSensorData(jsonData);
    QCOMPARE(data.key, QString("SO2"));
    QVERIFY(data.values.empty());
}

void TestDataParser::parseSensorData_NullValues()
{
    QByteArray jsonData = R"(
        {
            "key": "O3",
            "values": [
                { "date": "2024-05-10 14:00:00", "value": 70.0 },
                { "date": "2024-05-10 15:00:00", "value": null },
                { "date": "2024-05-10 16:00:00", "value": 75.5 }
            ]
        }
    )";
    SensorData data = parser.parseSensorData(jsonData);
    QCOMPARE(data.key, QString("O3"));
    QCOMPARE(data.values.size(), 3);
    if(data.values.size() == 3) {
        QCOMPARE(data.values[0].value, 70.0);
        QVERIFY(std::isnan(data.values[1].value));
        QCOMPARE(data.values[2].value, 75.5);
    }
}

void TestDataParser::parseSensorData_MalformedJson()
{
    QByteArray jsonData = R"({ "key": "CO", "values": [ }])";
    SensorData data = parser.parseSensorData(jsonData);
    QVERIFY(data.key.isEmpty());
    QVERIFY(data.values.empty());
}

void TestDataParser::parseSensorData_IsoDateFormat()
{
    QByteArray jsonData = R"(
        {
            "key": "PM2.5",
            "values": [
                { "date": "2024-05-10T12:00:00Z", "value": 22.1 }
            ]
        }
    )";
    SensorData data = parser.parseSensorData(jsonData);
    QCOMPARE(data.key, QString("PM2.5"));
    QCOMPARE(data.values.size(), 1);
    if(!data.values.empty()) {
        QDateTime expectedDate = QDateTime::fromString("2024-05-10T12:00:00Z", Qt::ISODate);
        QVERIFY(expectedDate.isValid());
        QCOMPARE(data.values[0].date.isValid(), true);
        QCOMPARE(data.values[0].date.toUTC(), expectedDate.toUTC());
        QCOMPARE(data.values[0].value, 22.1);
    } else {
        QFAIL("Parsing failed for ISO date format");
    }
}


// Testy dla parseAirQualityIndex

void TestDataParser::parseAirQualityIndex_ValidData()
{
    QByteArray jsonData = R"(
        {
            "id": 555,
            "stCalcDate": "2024-05-10 14:00:00",
            "stSourceDataDate": "2024-05-10 13:00:00",
            "stIndexLevel": {"id": 1, "indexLevelName": "Dobry"},
            "so2IndexLevel": {"id": 0, "indexLevelName": "Bardzo dobry"},
            "no2IndexLevel": {"id": 1, "indexLevelName": "Dobry"},
            "coIndexLevel": {"id": 0, "indexLevelName": "Bardzo dobry"},
            "pm10IndexLevel": {"id": 1, "indexLevelName": "Dobry"},
            "pm25IndexLevel": {"id": 2, "indexLevelName": "Umiarkowany"},
            "o3IndexLevel": {"id": 0, "indexLevelName": "Bardzo dobry"},
            "c6h6IndexLevel": {"id": 0, "indexLevelName": "Bardzo dobry"}
        }
    )";
    AirQualityIndex index = parser.parseAirQualityIndex(jsonData);

    QCOMPARE(index.stationId, 555);
    QCOMPARE(index.stCalcDate, QDateTime::fromString("2024-05-10 14:00:00", "yyyy-MM-dd HH:mm:ss"));
    QCOMPARE(index.stSourceDataDate, QDateTime::fromString("2024-05-10 13:00:00", "yyyy-MM-dd HH:mm:ss"));
    QCOMPARE(index.stIndexLevel.id, 1);
    QCOMPARE(index.stIndexLevel.indexLevelName, QString("Dobry"));
    QCOMPARE(index.pm25IndexLevel.id, 2);
    QCOMPARE(index.pm25IndexLevel.indexLevelName, QString("Umiarkowany"));
    QCOMPARE(index.so2IndexLevel.id, 0);
    QCOMPARE(index.so2IndexLevel.indexLevelName, QString("Bardzo dobry"));
}

void TestDataParser::parseAirQualityIndex_MissingIndexLevels()
{
    QByteArray jsonData = R"(
        {
            "id": 556,
            "stCalcDate": "2024-05-10 15:00:00",
            "stSourceDataDate": "2024-05-10 14:00:00",
            "stIndexLevel": {"id": 2, "indexLevelName": "Umiarkowany"},
            "pm10IndexLevel": {"id": 2, "indexLevelName": "Umiarkowany"}
            // Brakuje SO2, NO2, CO, PM2.5, O3, C6H6
        }
    )";
    AirQualityIndex index = parser.parseAirQualityIndex(jsonData);
    QCOMPARE(index.stationId, 556);
    QCOMPARE(index.stIndexLevel.id, 2);
    QCOMPARE(index.pm10IndexLevel.id, 2);
    QCOMPARE(index.so2IndexLevel.id, -1);
    QCOMPARE(index.so2IndexLevel.indexLevelName, QString("N/A"));
    QCOMPARE(index.no2IndexLevel.id, -1);
    QCOMPARE(index.no2IndexLevel.indexLevelName, QString("N/A"));
    QCOMPARE(index.pm25IndexLevel.id, -1);
    QCOMPARE(index.pm25IndexLevel.indexLevelName, QString("N/A"));
}

void TestDataParser::parseAirQualityIndex_NullIndexLevels()
{
    QByteArray jsonData = R"(
        {
            "id": 557,
            "stCalcDate": "2024-05-10 16:00:00",
            "stSourceDataDate": "2024-05-10 15:00:00",
            "stIndexLevel": {"id": 0, "indexLevelName": "Bardzo dobry"},
            "pm10IndexLevel": null,
            "pm25IndexLevel": null
        }
    )";
    AirQualityIndex index = parser.parseAirQualityIndex(jsonData);
    QCOMPARE(index.stationId, 557);
    QCOMPARE(index.stIndexLevel.id, 0);
    QCOMPARE(index.pm10IndexLevel.id, -1);
    QCOMPARE(index.pm10IndexLevel.indexLevelName, QString("N/A"));
    QCOMPARE(index.pm25IndexLevel.id, -1);
    QCOMPARE(index.pm25IndexLevel.indexLevelName, QString("N/A"));
}

void TestDataParser::parseAirQualityIndex_NoDataOrInvalidId()
{
    QByteArray jsonData = R"(null)";
    AirQualityIndex index = parser.parseAirQualityIndex(jsonData);
    QCOMPARE(index.stationId, -1);

    jsonData = R"({})";
    index = parser.parseAirQualityIndex(jsonData);
    QCOMPARE(index.stationId, -1);

    jsonData = R"({"some_other_field": 1})";
    index = parser.parseAirQualityIndex(jsonData);
    QCOMPARE(index.stationId, -1);
}

void TestDataParser::parseAirQualityIndex_MalformedJson()
{
    QByteArray jsonData = R"({"id": 558, "stIndexLevel": )";
    AirQualityIndex index = parser.parseAirQualityIndex(jsonData);
    QCOMPARE(index.stationId, -1);
}
