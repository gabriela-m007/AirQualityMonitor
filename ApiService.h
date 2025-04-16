#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <vector>
#include "DataStructures.h" // Include our data structures

class QNetworkReply; // Forward declaration

class ApiService : public QObject
{
    Q_OBJECT

public:
    explicit ApiService(QObject *parent = nullptr);

    void fetchAllStations();
    void fetchSensorsForStation(int stationId);
    void fetchSensorData(int sensorId);
    void fetchAirQualityIndex(int stationId);

signals:
    void stationsReady(const std::vector<MeasuringStation>& stations);
    void sensorsReady(const std::vector<Sensor>& sensors);
    void sensorDataReady(const SensorData& data);
    void airQualityIndexReady(const AirQualityIndex& index);
    void networkError(const QString& errorMsg); // Signal for errors

private slots:
    void onAllStationsReplyFinished(QNetworkReply *reply);
    void onSensorsReplyFinished(QNetworkReply *reply);
    void onSensorDataReplyFinished(QNetworkReply *reply);
    void onAirQualityIndexReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    const QString m_baseUrl = "https://api.gios.gov.pl/pjp-api/rest"; // Corrected base URL
    // Placeholder for DataParser - we'll need it here or pass data to it
};

#endif // APISERVICE_H
