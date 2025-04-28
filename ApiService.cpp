#include "ApiService.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QDebug>
#include <QEventLoop>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QJsonDocument>
#include <QThreadPool>
#include "DataParser.h"

ApiService::ApiService(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{
}

void ApiService::fetchAllStations()
{
    QUrl url(m_baseUrl + "/station/findAll");
    qDebug() << "Żądanie pobrania wszystkich stacji w wątku tła z:" << url.toString();

    (void)QtConcurrent::run([this, url]() {
        QNetworkAccessManager manager;
        QNetworkRequest request(url);
        QScopedPointer<QNetworkReply> reply(manager.get(request));

        QEventLoop loop;
        connect(reply.data(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "Otrzymano dane stacji w tle, rozmiar:" << responseData.size();
            DataParser parser;
            std::vector<MeasuringStation> stations = parser.parseStations(responseData);
            if (!stations.empty() || responseData == "[]") {
                emit stationsReady(stations);
            } else {
                qWarning() << "Nie udało się sparsować danych stacji. Surowe dane:" << responseData.trimmed();
                emit networkError("Błąd przetwarzania danych stacji.");
            }
        } else {
            qWarning() << "Błąd sieci podczas pobierania stacji:" << reply->errorString();
            emit networkError("Błąd sieci (stacje): " + reply->errorString());
        }
    });
}

void ApiService::fetchSensorsForStation(int stationId)
{
    QUrl url(m_baseUrl + QString("/station/sensors/%1").arg(stationId));
    qDebug() << "Żądanie pobrania czujników w wątku tła dla stacji" << stationId << "z:" << url.toString();

    (void)QtConcurrent::run([this, url]() {
        QNetworkAccessManager manager;
        QNetworkRequest request(url);
        QScopedPointer<QNetworkReply> reply(manager.get(request));

        QEventLoop loop;
        connect(reply.data(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "Otrzymano dane czujników w tle, rozmiar:" << responseData.size();
            DataParser parser;
            std::vector<Sensor> sensors = parser.parseSensors(responseData);
            if (!sensors.empty() || responseData == "[]") {
                emit sensorsReady(sensors);
            } else {
                qWarning() << "Nie udało się sparsować danych czujników. Surowe dane:" << responseData.trimmed();
                emit networkError("Błąd przetwarzania danych czujników.");
            }
        } else {
            qWarning() << "Błąd sieci podczas pobierania czujników:" << reply->errorString();
            emit networkError("Błąd sieci (czujniki): " + reply->errorString());
        }
    });
}

void ApiService::fetchSensorData(int sensorId)
{
    QUrl url(m_baseUrl + QString("/data/getData/%1").arg(sensorId));
    qDebug() << "Żądanie pobrania danych czujnika w wątku tła dla czujnika" << sensorId << "z:" << url.toString();

    (void)QtConcurrent::run([this, url]() {
        QNetworkAccessManager manager;
        QNetworkRequest request(url);
        QScopedPointer<QNetworkReply> reply(manager.get(request));

        QEventLoop loop;
        connect(reply.data(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "Otrzymano dane czujnika w tle, rozmiar:" << responseData.size();
            DataParser parser;
            SensorData data = parser.parseSensorData(responseData);

            if (!data.key.isEmpty()) {
                emit sensorDataReady(data);
            } else {
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isNull() && !responseData.isEmpty() && responseData != "[]") {
                    qWarning() << "Nie udało się sparsować danych pomiarowych czujnika. Nieprawidłowy JSON. Surowe dane:" << responseData.trimmed();
                    emit networkError("Błąd przetwarzania danych pomiarowych (nieprawidłowy format).");
                } else {
                    qWarning() << "Dane czujnika sparsowane, ale brakuje klucza lub nie znaleziono prawidłowych wartości. Surowe dane:" << responseData.trimmed();
                    emit sensorDataReady(data);
                }
            }
        } else {
            qWarning() << "Błąd sieci podczas pobierania danych czujnika:" << reply->errorString();
            emit networkError("Błąd sieci (dane pomiarowe): " + reply->errorString());
        }
    });
}

void ApiService::fetchAirQualityIndex(int stationId)
{
    QUrl url(m_baseUrl + QString("/aqindex/getIndex/%1").arg(stationId));
    qDebug() << "Żądanie pobrania indeksu AQI w wątku tła dla stacji" << stationId << "z:" << url.toString();

    (void)QtConcurrent::run([this, url]() {
        QNetworkAccessManager manager;
        QNetworkRequest request(url);
        QScopedPointer<QNetworkReply> reply(manager.get(request));

        QEventLoop loop;
        connect(reply.data(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "Otrzymano dane indeksu AQI w tle, rozmiar:" << responseData.size();
            DataParser parser;
            AirQualityIndex index = parser.parseAirQualityIndex(responseData);

            if (index.stationId != -1) {
                emit airQualityIndexReady(index);
            } else {
                QJsonDocument doc = QJsonDocument::fromJson(responseData);
                if (doc.isNull() && !responseData.isEmpty() && responseData != "[]") {
                    qWarning() << "Nie udało się sparsować danych AQI. Nieprawidłowy JSON. Surowe dane:" << responseData.trimmed();
                    emit networkError("Błąd przetwarzania danych AQI (nieprawidłowy format).");
                } else {
                    qWarning() << "Dane AQI sparsowane, ale wydają się nieprawidłowe (ID=-1) lub API zwróciło brak danych. Surowe dane:" << responseData.trimmed();
                    emit networkError("Indeks Jakości Powietrza niedostępny dla tej stacji (problem z danymi API lub parsowaniem).");
                }
            }
        } else {
            int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpStatusCode == 404) {
                qWarning() << "Błąd sieci podczas pobierania indeksu AQI: 404 Not Found dla URL:" << url.toString();
                emit networkError("Indeks Jakości Powietrza niedostępny dla tej stacji (nie znaleziono).");
            } else {
                qWarning() << "Błąd sieci podczas pobierania indeksu AQI:" << reply->errorString();
                emit networkError("Błąd sieci (indeks AQI): " + reply->errorString());
            }
        }
    });
}
