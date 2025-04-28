/**
 * @file ApiService.h
 * @brief Definicja klasy ApiService odpowiedzialnej za komunikację z API GIOS.
 */
#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <vector>
#include "DataStructures.h" // Zakładamy, że DataStructures.h ma już komentarze

class QNetworkReply;

/**
 * @class ApiService
 * @brief Odpowiada za pobieranie danych z publicznego API Głównego Inspektoratu Ochrony Środowiska (GIOS).
 *
 * Klasa wykonuje żądania sieciowe asynchronicznie w osobnych wątkach (używając QtConcurrent)
 * i emituje sygnały z wynikami (sparsowanymi danymi) lub informacjami o błędach sieciowych/parsowania.
 */
class ApiService : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy ApiService.
     * @param parent Wskaźnik na obiekt nadrzędny (dla zarządzania pamięcią w Qt).
     */
    explicit ApiService(QObject *parent = nullptr);

    /**
     * @brief Rozpoczyna asynchroniczne pobieranie listy wszystkich stacji pomiarowych z API GIOS.
     *
     * Po pomyślnym zakończeniu operacji i sparsowaniu danych emitowany jest sygnał stationsReady().
     * W przypadku błędu emitowany jest sygnał networkError().
     */
    void fetchAllStations();

    /**
     * @brief Rozpoczyna asynchroniczne pobieranie listy czujników (stanowisk pomiarowych) dla podanej stacji.
     * @param stationId Unikalny identyfikator stacji pomiarowej, dla której pobierane są czujniki.
     *
     * Po pomyślnym zakończeniu operacji i sparsowaniu danych emitowany jest sygnał sensorsReady().
     * W przypadku błędu emitowany jest sygnał networkError().
     */
    void fetchSensorsForStation(int stationId);

    /**
     * @brief Rozpoczyna asynchroniczne pobieranie danych pomiarowych (serii czasowej) dla podanego czujnika.
     * @param sensorId Unikalny identyfikator czujnika, dla którego pobierane są dane.
     *
     * Po pomyślnym zakończeniu operacji i sparsowaniu danych emitowany jest sygnał sensorDataReady().
     * W przypadku błędu emitowany jest sygnał networkError(). Może również emitować sensorDataReady z pustymi danymi, jeśli API zwróciło dane, ale bez wartości pomiarowych.
     */
    void fetchSensorData(int sensorId);

    /**
     * @brief Rozpoczyna asynchroniczne pobieranie aktualnego indeksu jakości powietrza (AQI) dla podanej stacji.
     * @param stationId Unikalny identyfikator stacji pomiarowej, dla której pobierany jest indeks AQI.
     *
     * Po pomyślnym zakończeniu operacji i sparsowaniu danych emitowany jest sygnał airQualityIndexReady().
     * W przypadku błędu (np. 404 Not Found, błąd sieci, błąd parsowania) emitowany jest sygnał networkError().
     * @note Nie wszystkie stacje posiadają obliczony indeks AQI.
     */
    void fetchAirQualityIndex(int stationId);

signals:
    /**
     * @brief Sygnał emitowany, gdy lista stacji zostanie pomyślnie pobrana i sparsowana.
     * @param stations Wektor obiektów MeasuringStation zawierający pobrane stacje.
     */
    void stationsReady(const std::vector<MeasuringStation>& stations);

    /**
     * @brief Sygnał emitowany, gdy lista czujników dla danej stacji zostanie pomyślnie pobrana i sparsowana.
     * @param sensors Wektor obiektów Sensor zawierający pobrane czujniki.
     */
    void sensorsReady(const std::vector<Sensor>& sensors);

    /**
     * @brief Sygnał emitowany, gdy dane pomiarowe dla czujnika zostaną pomyślnie pobrane i sparsowane.
     * @param data Obiekt SensorData zawierający klucz parametru i wektor wartości pomiarowych. Może zawierać pusty wektor wartości, jeśli API nie zwróciło pomiarów.
     */
    void sensorDataReady(const SensorData& data);

    /**
     * @brief Sygnał emitowany, gdy indeks jakości powietrza (AQI) dla stacji zostanie pomyślnie pobrany i sparsowany.
     * @param index Obiekt AirQualityIndex zawierający obliczone indeksy.
     */
    void airQualityIndexReady(const AirQualityIndex& index);

    /**
     * @brief Sygnał emitowany w przypadku wystąpienia błędu podczas komunikacji sieciowej lub przetwarzania odpowiedzi z API.
     * @param errorMsg Komunikat tekstowy opisujący napotkany błąd.
     */
    void networkError(const QString& errorMsg);

private:
    /**
     * @brief Manager Qt do obsługi żądań sieciowych.
     * @note W obecnej implementacji (z QtConcurrent::run i QEventLoop) ten manager nie jest bezpośrednio używany w metodach fetch*,
     *       gdyż każda operacja tworzy własny tymczasowy manager. Może to być pole do refaktoryzacji w przyszłości
     *       na model w pełni asynchroniczny oparty o sygnały i sloty QNetworkReply.
     */
    QNetworkAccessManager *m_networkManager;

    ///< Podstawowy URL dla endpointów API Głównego Inspektoratu Ochrony Środowiska.
    const QString m_baseUrl = "https://api.gios.gov.pl/pjp-api/rest";
};

#endif // APISERVICE_H
