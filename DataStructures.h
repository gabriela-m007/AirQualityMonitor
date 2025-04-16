#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QString>
#include <QDateTime>
#include <vector>
#include <map>

// Represents commune information (nested within City)
struct Commune {
    QString communeName;
    QString districtName;
    QString provinceName;
};

// Represents city/address information (nested within MeasuringStation)
struct City {
    int id = -1; // Use -1 or similar to indicate invalid/uninitialized
    QString name;
    Commune commune;
    QString addressStreet;
};

// Represents a measuring station
struct MeasuringStation {
    int id = -1;
    QString stationName;
    double gegrLat = 0.0;
    double gegrLon = 0.0;
    City city;
};

// Represents parameter details (nested within Sensor)
struct Parameter {
    QString paramName;
    QString paramFormula;
    QString paramCode;
    int idParam = -1; // Added based on description
};

// Represents a sensor (measuring stand) at a station
struct Sensor {
    int id = -1;
    int stationId = -1;
    Parameter param;
};

// Represents a single measurement value pair
struct MeasurementValue {
    QDateTime date;
    double value = std::numeric_limits<double>::quiet_NaN(); // Use NaN for null/invalid
};

// Represents the data collected for a specific sensor (parameter)
struct SensorData {
    QString key; // Parameter code (e.g., "PM10")
    std::vector<MeasurementValue> values;
};

// Represents the index level details (nested within AirQualityIndex)
struct IndexLevel {
    int id = -1; // Scale 0-5 (or -1 for invalid/not available)
    QString indexLevelName;
};

// Represents the overall air quality index for a station
struct AirQualityIndex {
    int stationId = -1;
    QDateTime stCalcDate;
    QDateTime stSourceDataDate;
    IndexLevel stIndexLevel; // Overall index

    // Optional: Add specific index levels if needed based on API response structure
    IndexLevel so2IndexLevel;
    IndexLevel no2IndexLevel;
    IndexLevel coIndexLevel;
    IndexLevel pm10IndexLevel;
    IndexLevel pm25IndexLevel;
    IndexLevel o3IndexLevel;
    IndexLevel c6h6IndexLevel;
    // Add flags or check indexLevel.id != -1 to see if they are valid
};


#endif // DATASTRUCTURES_H
