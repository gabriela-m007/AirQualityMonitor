/**
 * @file DataParser.h
 * @brief Definicja klasy DataParser odpowiedzialnej za parsowanie odpowiedzi JSON z API GIOS.
 */
#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QByteArray>
#include <vector>
#include "DataStructures.h" // Potrzebne struktury danych

class QJsonObject;

/**
 * @class DataParser
 * @brief Odpowiada za konwersję danych w formacie JSON (jako QByteArray) na struktury C++ zdefiniowane w DataStructures.h.
 *
 * Klasa udostępnia metody do parsowania różnych typów odpowiedzi z API GIOS: listy stacji, listy czujników,
 * danych pomiarowych oraz indeksu jakości powietrza. Zawiera również prywatne metody pomocnicze do bezpiecznego
 * odczytywania wartości różnych typów z obiektów QJsonObject.
 */
class DataParser
{
public:
    /**
     * @brief Domyślny konstruktor.
     */
    DataParser();

    /**
     * @brief Parsuje dane JSON zawierające listę stacji pomiarowych.
     * @param jsonData Dane w formacie JSON jako QByteArray. Oczekiwana tablica obiektów stacji.
     * @return Wektor obiektów MeasuringStation. Zwraca pusty wektor w przypadku błędu parsowania JSON lub gdy tablica jest pusta.
     * @note Pomija stacje, dla których nie udało się sparsować poprawnego ID (ID = -1).
     */
    std::vector<MeasuringStation> parseStations(const QByteArray& jsonData);

    /**
     * @brief Parsuje dane JSON zawierające listę czujników (stanowisk pomiarowych) dla danej stacji.
     * @param jsonData Dane w formacie JSON jako QByteArray. Oczekiwana tablica obiektów czujników.
     * @return Wektor obiektów Sensor. Zwraca pusty wektor w przypadku błędu parsowania JSON lub gdy tablica jest pusta.
     * @note Pomija czujniki, dla których nie udało się sparsować poprawnego ID (ID = -1).
     */
    std::vector<Sensor> parseSensors(const QByteArray& jsonData);

    /**
     * @brief Parsuje dane JSON zawierające dane pomiarowe (serię czasową) dla danego czujnika.
     * @param jsonData Dane w formacie JSON jako QByteArray. Oczekiwany obiekt zawierający klucz 'key' i tablicę 'values'.
     * @return Obiekt SensorData. Zwraca obiekt z pustym kluczem i/lub pustym wektorem wartości w przypadku błędu parsowania JSON lub niekompletnych danych.
     * @note Poprawnie obsługuje wartości 'null' w pomiarach, konwertując je na NaN.
     */
    SensorData parseSensorData(const QByteArray& jsonData);

    /**
     * @brief Parsuje dane JSON zawierające indeks jakości powietrza (AQI) dla danej stacji.
     * @param jsonData Dane w formacie JSON jako QByteArray. Oczekiwany obiekt indeksu AQI.
     * @return Obiekt AirQualityIndex. Zwraca obiekt z stationId = -1 w przypadku błędu parsowania JSON lub gdy obiekt JSON nie zawiera poprawnego ID stacji.
     * @note Poprawnie obsługuje brakujące lub `null`'owe pola dla poszczególnych poziomów indeksów, ustawiając dla nich wartości domyślne (ID=-1, Name="N/A").
     */
    AirQualityIndex parseAirQualityIndex(const QByteArray& jsonData);

private:
    /**
     * @brief Metoda pomocnicza do bezpiecznego pobierania wartości typu QString z QJsonObject.
     * @param obj Obiekt JSON, z którego pobierana jest wartość.
     * @param key Klucz wartości do pobrania.
     * @return Wartość QString. Zwraca pusty string, jeśli klucz nie istnieje lub wartość nie jest stringiem.
     */
    QString getString(const QJsonObject& obj, const QString& key);

    /**
     * @brief Metoda pomocnicza do bezpiecznego pobierania wartości typu int z QJsonObject.
     * @param obj Obiekt JSON, z którego pobierana jest wartość.
     * @param key Klucz wartości do pobrania.
     * @param defaultValue Wartość zwracana, jeśli klucz nie istnieje, wartość jest null/undefined, lub konwersja na int zawiedzie. Domyślnie -1.
     * @return Wartość int.
     * @note Próbuje konwersji również jeśli wartość jest zapisana jako double lub string.
     */
    int getInt(const QJsonObject& obj, const QString& key, int defaultValue = -1);

    /**
     * @brief Metoda pomocnicza do bezpiecznego pobierania wartości typu double z QJsonObject.
     * @param obj Obiekt JSON, z którego pobierana jest wartość.
     * @param key Klucz wartości do pobrania.
     * @param defaultValue Wartość zwracana, jeśli klucz nie istnieje, wartość jest null/undefined, lub konwersja na double zawiedzie. Domyślnie 0.0.
     * @return Wartość double.
     * @note Próbuje konwersji również jeśli wartość jest zapisana jako string.
     */
    double getDouble(const QJsonObject& obj, const QString& key, double defaultValue = 0.0);

    /**
     * @brief Metoda pomocnicza do parsowania zagnieżdżonego obiektu poziomu indeksu AQI.
     * @param obj Główny obiekt JSON indeksu AQI.
     * @param baseKey Klucz, pod którym znajduje się obiekt IndexLevel (np. "stIndexLevel", "pm10IndexLevel").
     * @return Obiekt IndexLevel. Zwraca obiekt z wartościami domyślnymi (ID=-1, Name="N/A"), jeśli obiekt pod danym kluczem nie istnieje, jest null lub nie jest obiektem JSON.
     */
    IndexLevel parseIndexLevel(const QJsonObject& obj, const QString& baseKey);
};

#endif // DATAPARSER_H
