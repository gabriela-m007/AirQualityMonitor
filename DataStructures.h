/**
 * @file DataStructures.h
 * @brief Definicje podstawowych struktur danych używanych w aplikacji do przechowywania informacji o stacjach, czujnikach i pomiarach GIOS.
 */
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QString>
#include <QDateTime>
#include <vector>
#include <limits>

/**
 * @brief Przechowuje informacje o gminie, powiecie i województwie.
 * Odpowiada strukturze 'commune' w danych API GIOS.
 */
struct Commune {
    QString communeName;    ///< Nazwa gminy.
    QString districtName;   ///< Nazwa powiatu.
    QString provinceName;   ///< Nazwa województwa.
};

/**
 * @brief Przechowuje informacje o mieście/lokalizacji stacji.
 * Odpowiada strukturze 'city' w danych API GIOS.
 */
struct City {
    int id = -1;            ///< Unikalny identyfikator lokalizacji (z API, -1 jeśli nieznany).
    QString name;           ///< Nazwa miejscowości.
    Commune commune;        ///< Zagnieżdżone informacje o gminie.
    QString addressStreet;  ///< Nazwa ulicy, na której znajduje się stacja (jeśli dostępna).
};

/**
 * @brief Reprezentuje stację pomiarową jakości powietrza.
 * Odpowiada głównemu obiektowi w odpowiedzi API dla stacji.
 */
struct MeasuringStation {
    int id = -1;            ///< Unikalny identyfikator stacji (z API, -1 jeśli nieznany).
    QString stationName;    ///< Pełna nazwa stacji pomiarowej.
    double gegrLat = 0.0;   ///< Szerokość geograficzna stacji.
    double gegrLon = 0.0;   ///< Długość geograficzna stacji.
    City city;              ///< Zagnieżdżone informacje o lokalizacji stacji.
};

/**
 * @brief Przechowuje informacje o parametrze mierzonym przez czujnik.
 * Odpowiada strukturze 'param' w danych API GIOS dla czujników.
 */
struct Parameter {
    QString paramName;      ///< Pełna nazwa mierzonego parametru (np. "Dwutlenek azotu").
    QString paramFormula;   ///< Wzór chemiczny lub symbol parametru (np. "NO2").
    QString paramCode;      ///< Krótki kod parametru (np. "NO2").
    int idParam = -1;       ///< Unikalny identyfikator parametru (z API, -1 jeśli nieznany).
};

/**
 * @brief Reprezentuje pojedynczy czujnik (stanowisko pomiarowe) na stacji.
 * Odpowiada głównemu obiektowi w odpowiedzi API dla czujników stacji.
 */
struct Sensor {
    int id = -1;            ///< Unikalny identyfikator czujnika (z API, -1 jeśli nieznany).
    int stationId = -1;     ///< ID stacji, do której przypisany jest czujnik (-1 jeśli nieznany).
    Parameter param;        ///< Zagnieżdżone informacje o mierzonym parametrze.
};

/**
 * @brief Reprezentuje pojedynczą wartość pomiarową wraz z datą jej wykonania.
 */
struct MeasurementValue {
    QDateTime date;         ///< Data i czas wykonania pomiaru.
    /**
     * @brief Zmierzona wartość.
     * Może być NaN (Not a Number), jeśli wartość była `null` w odpowiedzi API lub nie udało się jej sparsować.
     */
    double value = std::numeric_limits<double>::quiet_NaN();
};

/**
 * @brief Przechowuje serię danych pomiarowych dla konkretnego parametru (klucza) z jednego czujnika.
 * Odpowiada strukturze danych zwracanej przez API dla danych pomiarowych czujnika.
 */
struct SensorData {
    QString key;            ///< Kod parametru, którego dotyczą dane (np. "PM10", "SO2").
    std::vector<MeasurementValue> values; ///< Sekwencja wartości pomiarowych (par data-wartość).
};

/**
 * @brief Reprezentuje poziom indeksu jakości powietrza (ogólny lub dla konkretnego zanieczyszczenia).
 * Odpowiada strukturze '*IndexLevel' w danych API GIOS dla indeksu AQI.
 */
struct IndexLevel {
    int id = -1;            ///< ID poziomu indeksu (zwykle 0-5, -1 jeśli nieznany/brak danych).
    QString indexLevelName; ///< Tekstowa nazwa poziomu (np. "Dobry", "Zły", "Brak indeksu", "N/A").
};

/**
 * @brief Przechowuje zagregowany indeks jakości powietrza (AQI) dla stacji.
 * Odpowiada głównemu obiektowi w odpowiedzi API dla indeksu AQI.
 */
struct AirQualityIndex {
    int stationId = -1;     ///< ID stacji, której dotyczy indeks (-1 jeśli nieznany lub błąd).
    QDateTime stCalcDate;   ///< Data i czas obliczenia indeksu przez system GIOS.
    QDateTime stSourceDataDate; ///< Data i czas ostatnich danych źródłowych użytych do obliczenia indeksu.
    IndexLevel stIndexLevel; ///< Ogólny (najgorszy) poziom indeksu dla stacji.

    // Poziomy indeksów dla poszczególnych zanieczyszczeń
    IndexLevel so2IndexLevel;   ///< Indeks dla SO2.
    IndexLevel no2IndexLevel;   ///< Indeks dla NO2.
    IndexLevel coIndexLevel;    ///< Indeks dla CO.
    IndexLevel pm10IndexLevel;  ///< Indeks dla PM10.
    IndexLevel pm25IndexLevel;  ///< Indeks dla PM2.5.
    IndexLevel o3IndexLevel;    ///< Indeks dla O3.
    IndexLevel c6h6IndexLevel;  ///< Indeks dla C6H6 (benzen).
};


#endif // DATASTRUCTURES_H
