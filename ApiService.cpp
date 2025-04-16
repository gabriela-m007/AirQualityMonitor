#include "ApiService.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QDebug>
#include "DataParser.h" // We'll create this next

ApiService::ApiService(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{
}

void ApiService::fetchAllStations()
{
    QUrl url(m_baseUrl + "/station/findAll");
    QNetworkRequest request(url);
    qDebug() << "Fetching all stations from:" << url.toString();
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onAllStationsReplyFinished(reply);
    });
}

void ApiService::fetchSensorsForStation(int stationId)
{
    QUrl url(m_baseUrl + QString("/station/sensors/%1").arg(stationId));
    QNetworkRequest request(url);
    qDebug() << "Fetching sensors for station" << stationId << "from:" << url.toString();
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onSensorsReplyFinished(reply);
    });
}

void ApiService::fetchSensorData(int sensorId)
{
    QUrl url(m_baseUrl + QString("/data/getData/%1").arg(sensorId));
    QNetworkRequest request(url);
    qDebug() << "Fetching data for sensor" << sensorId << "from:" << url.toString();
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onSensorDataReplyFinished(reply);
    });
}

void ApiService::fetchAirQualityIndex(int stationId)
{
    QUrl url(m_baseUrl + QString("/aqindex/getIndex/%1").arg(stationId));
    QNetworkRequest request(url);
    qDebug() << "Fetching AQ index for station" << stationId << "from:" << url.toString();
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onAirQualityIndexReplyFinished(reply);
    });
}

// --- Reply Handlers ---

void ApiService::onAllStationsReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qDebug() << "Received stations data, size:" << responseData.size();
        // *** TODO: Implement JSON parsing using DataParser ***
        DataParser parser;
        std::vector<MeasuringStation> stations = parser.parseStations(responseData);
        if (!stations.empty() || responseData == "[]") { // Handle empty valid response
            emit stationsReady(stations);
        } else {
            emit networkError("Failed to parse station data.");
        }

    } else {
        qWarning() << "Network error fetching stations:" << reply->errorString();
        emit networkError("Network Error: " + reply->errorString());
    }
    reply->deleteLater(); // Clean up the reply object
}

void ApiService::onSensorsReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qDebug() << "Received sensors data, size:" << responseData.size();
        // *** TODO: Implement JSON parsing using DataParser ***
        DataParser parser;
        std::vector<Sensor> sensors = parser.parseSensors(responseData);
        if (!sensors.empty() || responseData == "[]") {
            emit sensorsReady(sensors);
        } else {
            emit networkError("Failed to parse sensor data.");
        }
    } else {
        qWarning() << "Network error fetching sensors:" << reply->errorString();
        emit networkError("Network Error: " + reply->errorString());
    }
    reply->deleteLater();
}

void ApiService::onSensorDataReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();

        qDebug() << "Received sensor data, size:" << responseData.size();
        // *** TODO: Implement JSON parsing using DataParser ***
        DataParser parser;
        SensorData data = parser.parseSensorData(responseData);
        // Basic check if parsing produced something meaningful
        if (!data.key.isEmpty()){
            emit sensorDataReady(data);
        } else {
            emit networkError("Failed to parse measurement data.");
        }
    } else {
        qWarning() << "Network error fetching sensor data:" << reply->errorString();
        emit networkError("Network Error: " + reply->errorString());
    }
    reply->deleteLater();
}

void ApiService::onAirQualityIndexReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qDebug() << "Received AQ index data, size:" << responseData.size();
        // *** TODO: Implement JSON parsing using DataParser ***
        DataParser parser;
        AirQualityIndex index = parser.parseAirQualityIndex(responseData);
        // Basic check if parsing worked (e.g., station ID is set)
        if (index.stationId != -1) {
            emit airQualityIndexReady(index);
        } else {
            emit networkError("Failed to parse air quality index data.");
        }
    } else {
        // Handle specific error like 404 Not Found for index
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 404) {
            emit networkError("Air Quality Index not available for this station.");
        } else {
            qWarning() << "Network error fetching AQ index:" << reply->errorString();
            emit networkError("Network Error: " + reply->errorString());
        }
    }
    reply->deleteLater();
}
