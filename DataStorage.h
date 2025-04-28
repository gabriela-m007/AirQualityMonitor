/**
 * @file DataStorage.h
 * @brief Definicja klasy DataStorage odpowiedzialnej za zapis i odczyt danych aplikacji do/z plików JSON.
 */
#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <QString>
#include <vector>
#include "DataStructures.h" // Potrzebne struktury danych

class QJsonObject;
class QJsonArray;

/**
 * @class DataStorage
 * @brief Zapewnia mechanizmy do trwałego przechowywania danych aplikacji (stacje, czujniki, dane pomiarowe, AQI) w plikach JSON.
 *
 * Umożliwia zapisywanie i wczytywanie danych, co pozwala na działanie aplikacji w trybie offline
 * lub jako fallback w przypadku problemów z dostępem do API. Pliki są zapisywane w określonym
 * katalogu przechowywania (`storagePath`).
 */
class DataStorage
{
public:
    /**
     * @brief Konstruktor klasy DataStorage.
     * @param storagePath Ścieżka do katalogu, w którym będą przechowywane pliki JSON. Domyślnie bieżący katalog (".")
     *                    Jeśli katalog nie istnieje, próbuje go utworzyć.
     */
    DataStorage(const QString& storagePath = ".");

    /**
     * @brief Zapisuje wektor stacji pomiarowych do pliku JSON.
     * @param stations Wektor obiektów MeasuringStation do zapisania.
     * @param filename Nazwa pliku JSON (względem `storagePath`). Domyślnie "stations.json".
     * @return `true` jeśli zapis się powiódł, `false` w przeciwnym razie.
     */
    bool saveStationsToJson(const std::vector<MeasuringStation>& stations, const QString& filename = "stations.json");

    /**
     * @brief Wczytuje wektor stacji pomiarowych z pliku JSON.
     * @param filename Nazwa pliku JSON (względem `storagePath`). Domyślnie "stations.json".
     * @return Wektor obiektów MeasuringStation. Zwraca pusty wektor, jeśli plik nie istnieje, jest nieprawidłowy lub pusty.
     */
    std::vector<MeasuringStation> loadStationsFromJson(const QString& filename = "stations.json");

    /**
     * @brief Zapisuje dane pomiarowe (SensorData) dla konkretnego czujnika do pliku JSON.
     * @param data Obiekt SensorData do zapisania.
     * @param filename Nazwa pliku JSON (względem `storagePath`). Nazwa powinna identyfikować czujnik (np. "sensor_123_data.json").
     * @return `true` jeśli zapis się powiódł, `false` jeśli `data.key` jest pusty lub wystąpił błąd zapisu.
     */
    bool saveSensorDataToJson(const SensorData& data, const QString& filename);

    /**
     * @brief Wczytuje dane pomiarowe (SensorData) z pliku JSON.
     * @param filename Nazwa pliku JSON (względem `storagePath`).
     * @return Obiekt SensorData. Zwraca domyślny (pusty) obiekt, jeśli plik nie istnieje lub jest nieprawidłowy.
     */
    SensorData loadSensorDataFromJson(const QString& filename);

    /**
     * @brief Zapisuje wektor czujników dla konkretnej stacji do pliku JSON.
     * Nazwa pliku jest generowana automatycznie jako "station_{stationId}_sensors.json".
     * @param stationId ID stacji, której dotyczą czujniki.
     * @param sensors Wektor obiektów Sensor do zapisania.
     * @return `true` jeśli zapis się powiódł, `false` jeśli `stationId` jest nieprawidłowe (<=0) lub wystąpił błąd zapisu.
     */
    bool saveSensorsToJson(int stationId, const std::vector<Sensor>& sensors);

    /**
     * @brief Wczytuje wektor czujników dla konkretnej stacji z pliku JSON.
     * Nazwa pliku jest generowana automatycznie jako "station_{stationId}_sensors.json".
     * @param stationId ID stacji, której czujniki mają zostać wczytane.
     * @return Wektor obiektów Sensor. Zwraca pusty wektor, jeśli plik nie istnieje, jest nieprawidłowy, lub `stationId` jest nieprawidłowe.
     * @note Odfiltrowuje czujniki wczytane z pliku, które mają niezgodny `stationId`.
     */
    std::vector<Sensor> loadSensorsFromJson(int stationId);

    /**
     * @brief Zapisuje indeks jakości powietrza (AQI) dla konkretnej stacji do pliku JSON.
     * Nazwa pliku jest generowana automatycznie jako "station_{stationId}_aqi.json".
     * @param stationId ID stacji, której dotyczy indeks. Musi być zgodne z `index.stationId`.
     * @param index Obiekt AirQualityIndex do zapisania.
     * @return `true` jeśli zapis się powiódł, `false` jeśli ID są nieprawidłowe/niezgodne lub wystąpił błąd zapisu.
     */
    bool saveAirQualityIndexToJson(int stationId, const AirQualityIndex& index);

    /**
     * @brief Wczytuje indeks jakości powietrza (AQI) dla konkretnej stacji z pliku JSON.
     * Nazwa pliku jest generowana automatycznie jako "station_{stationId}_aqi.json".
     * @param stationId ID stacji, której indeks ma zostać wczytany.
     * @return Obiekt AirQualityIndex. Zwraca domyślny obiekt (z `stationId = -1`), jeśli plik nie istnieje, jest nieprawidłowy,
     *         `stationId` jest nieprawidłowe, lub ID stacji w pliku nie zgadza się z podanym `stationId`.
     */
    AirQualityIndex loadAirQualityIndexFromJson(int stationId);

    /**
     * @brief Zwraca aktualnie używaną ścieżkę do katalogu przechowywania danych.
     * @return Ścieżka do katalogu jako QString.
     */
    QString getStoragePath() const;

private:
    ///< Ścieżka do katalogu, w którym zapisywane są pliki JSON.
    QString m_storagePath;

    // --- Prywatne metody pomocnicze do konwersji na/z QJsonObject ---
    // (Dokumentacja dla nich może być mniej szczegółowa lub pominięta, jeśli są proste)

    /// Konwertuje obiekt MeasuringStation na QJsonObject.
    QJsonObject stationToJson(const MeasuringStation& station);
    /// Konwertuje QJsonObject na obiekt MeasuringStation.
    MeasuringStation stationFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt SensorData na QJsonObject.
    QJsonObject sensorDataToJson(const SensorData& data);
    /// Konwertuje QJsonObject na obiekt SensorData.
    SensorData sensorDataFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt City na QJsonObject.
    QJsonObject cityToJson(const City& city);
    /// Konwertuje QJsonObject na obiekt City.
    City cityFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt Commune na QJsonObject.
    QJsonObject communeToJson(const Commune& commune);
    /// Konwertuje QJsonObject na obiekt Commune.
    Commune communeFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt MeasurementValue na QJsonObject. Używa formatu ISODateWithMs dla daty.
    QJsonObject measurementValueToJson(const MeasurementValue& mv);
    /// Konwertuje QJsonObject na obiekt MeasurementValue. Używa formatu ISODateWithMs dla daty.
    MeasurementValue measurementValueFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt Sensor na QJsonObject.
    QJsonObject sensorToJson(const Sensor& sensor);
    /// Konwertuje QJsonObject na obiekt Sensor.
    Sensor sensorFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt Parameter na QJsonObject.
    QJsonObject parameterToJson(const Parameter& param);
    /// Konwertuje QJsonObject na obiekt Parameter.
    Parameter parameterFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt IndexLevel na QJsonObject.
    QJsonObject indexLevelToJson(const IndexLevel& level);
    /// Konwertuje QJsonObject na obiekt IndexLevel.
    IndexLevel indexLevelFromJson(const QJsonObject& obj);

    /// Konwertuje obiekt AirQualityIndex na QJsonObject. Używa formatu ISODateWithMs dla dat.
    QJsonObject airQualityIndexToJson(const AirQualityIndex& index);
    /// Konwertuje QJsonObject na obiekt AirQualityIndex. Używa formatu ISODateWithMs dla dat.
    AirQualityIndex airQualityIndexFromJson(const QJsonObject& obj);
};

#endif // DATASTORAGE_H
