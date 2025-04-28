#ifndef TESTDATAPARSER_H
#define TESTDATAPARSER_H

#include <QObject>
#include <QtTest/QtTest>
#include "DataParser.h"
#include "DataStructures.h"

class TestDataParser : public QObject
{
    Q_OBJECT

private:
    DataParser parser;

private slots:

    // Testy dla parseStations
    void parseStations_ValidData();
    void parseStations_EmptyArray();
    void parseStations_MalformedJson();
    void parseStations_MissingFields();

    // Testy dla parseSensors
    void parseSensors_ValidData();
    void parseSensors_EmptyArray();
    void parseSensors_MalformedJson();

    // Testy dla parseSensorData
    void parseSensorData_ValidData();
    void parseSensorData_EmptyValues();
    void parseSensorData_NullValues();
    void parseSensorData_MalformedJson();
    void parseSensorData_IsoDateFormat();

    // Testy dla parseAirQualityIndex
    void parseAirQualityIndex_ValidData();
    void parseAirQualityIndex_MissingIndexLevels();
    void parseAirQualityIndex_NullIndexLevels();
    void parseAirQualityIndex_NoDataOrInvalidId();
    void parseAirQualityIndex_MalformedJson();
};

#endif
