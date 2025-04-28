#include "testdataanalyzer.h"
#include <limits>

std::vector<MeasurementValue> TestDataAnalyzer::createTestData(const std::vector<double>& values, const QDateTime& startTime, int stepSeconds) {
    std::vector<MeasurementValue> data;
    QDateTime currentTime = startTime;
    for (double val : values) {
        MeasurementValue mv;
        mv.date = currentTime;
        mv.value = val;
        data.push_back(mv);
        currentTime = currentTime.addSecs(stepSeconds);
    }
    return data;
}


// Testy dla DataAnalyzer

void TestDataAnalyzer::analyze_EmptyData()
{
    std::vector<MeasurementValue> values;
    AnalysisResult result = analyzer.analyze(values);

    QVERIFY(!result.minVal.has_value());
    QVERIFY(!result.maxVal.has_value());
    QVERIFY(!result.average.has_value());
    QCOMPARE(result.trend, AnalysisResult::UNKNOWN);
    QCOMPARE(result.trendSlope, 0.0);
}

void TestDataAnalyzer::analyze_SingleValue()
{
    QDateTime time = QDateTime::currentDateTime();
    std::vector<MeasurementValue> values = createTestData({15.5}, time);
    AnalysisResult result = analyzer.analyze(values);

    QVERIFY(result.minVal.has_value());
    QCOMPARE(result.minVal->value, 15.5);
    QCOMPARE(result.minVal->date, time);

    QVERIFY(result.maxVal.has_value());
    QCOMPARE(result.maxVal->value, 15.5);
    QCOMPARE(result.maxVal->date, time);

    QVERIFY(result.average.has_value());
    QCOMPARE(result.average.value(), 15.5);

    QCOMPARE(result.trend, AnalysisResult::UNKNOWN);
}

void TestDataAnalyzer::analyze_AllNaNValues()
{
    double nan = std::numeric_limits<double>::quiet_NaN();
    QDateTime time = QDateTime::currentDateTime();
    std::vector<MeasurementValue> values = createTestData({nan, nan, nan}, time);
    AnalysisResult result = analyzer.analyze(values);

    QVERIFY(!result.minVal.has_value());
    QVERIFY(!result.maxVal.has_value());
    QVERIFY(!result.average.has_value());
    QCOMPARE(result.trend, AnalysisResult::UNKNOWN);
}

void TestDataAnalyzer::analyze_IncreasingTrend()
{
    QDateTime start = QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate);
    std::vector<MeasurementValue> values = createTestData({10, 12, 11, 14, 15}, start);
    AnalysisResult result = analyzer.analyze(values);

    QVERIFY(result.minVal.has_value());
    QCOMPARE(result.minVal->value, 10.0);
    QVERIFY(result.maxVal.has_value());
    QCOMPARE(result.maxVal->value, 15.0);
    QVERIFY(result.average.has_value());
    QVERIFY(std::abs(result.average.value() - (10.0+12.0+11.0+14.0+15.0)/5.0) < 1e-6);
    QCOMPARE(result.trend, AnalysisResult::INCREASING);
    QVERIFY(result.trendSlope > 1e-9);
}

void TestDataAnalyzer::analyze_DecreasingTrend()
{
    QDateTime start = QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate);
    std::vector<MeasurementValue> values = createTestData({30, 28, 29, 25, 24}, start);
    AnalysisResult result = analyzer.analyze(values);

    QCOMPARE(result.minVal->value, 24.0);
    QCOMPARE(result.maxVal->value, 30.0);
    QVERIFY(result.average.has_value());
    QCOMPARE(result.trend, AnalysisResult::DECREASING);
    QVERIFY(result.trendSlope < -1e-9);
}

void TestDataAnalyzer::analyze_StableTrend()
{
    QDateTime start = QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate);
    std::vector<MeasurementValue> values = createTestData({20.0, 20.1, 19.9, 20.0, 19.8, 20.2}, start);
    AnalysisResult result = analyzer.analyze(values);

    QCOMPARE(result.minVal->value, 19.8);
    QCOMPARE(result.maxVal->value, 20.2);
    QVERIFY(result.average.has_value());
    QCOMPARE(result.trend, AnalysisResult::STABLE);
    QVERIFY(std::abs(result.trendSlope) < 1e-5);
    QCOMPARE(result.trend, AnalysisResult::STABLE);
}

void TestDataAnalyzer::analyze_StableTrend_TwoPoints()
{
    QDateTime start = QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate);
    std::vector<MeasurementValue> values = createTestData({25.0, 25.0}, start);
    AnalysisResult result = analyzer.analyze(values);

    QCOMPARE(result.minVal->value, 25.0);
    QCOMPARE(result.maxVal->value, 25.0);
    QCOMPARE(result.average.value(), 25.0);
    QCOMPARE(result.trend, AnalysisResult::STABLE);
    QVERIFY(std::abs(result.trendSlope) < 1e-9);
}


void TestDataAnalyzer::analyze_MixedDataWithNaN()
{
    double nan = std::numeric_limits<double>::quiet_NaN();
    QDateTime start = QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate);
    std::vector<MeasurementValue> values = createTestData({10, nan, 12, 13, nan, 14}, start);
    AnalysisResult result = analyzer.analyze(values);

    QCOMPARE(result.minVal->value, 10.0);
    QCOMPARE(result.maxVal->value, 14.0);
    QVERIFY(result.average.has_value());
    QVERIFY(std::abs(result.average.value() - (10.0+12.0+13.0+14.0)/4.0) < 1e-6);
    QCOMPARE(result.trend, AnalysisResult::INCREASING);
}

void TestDataAnalyzer::analyze_DataWithZeroVariance()
{
    QDateTime start = QDateTime::fromString("2024-01-01T10:00:00", Qt::ISODate);
    std::vector<MeasurementValue> values = createTestData({50.0, 50.0, 50.0, 50.0}, start);
    AnalysisResult result = analyzer.analyze(values);

    QCOMPARE(result.minVal->value, 50.0);
    QCOMPARE(result.maxVal->value, 50.0);
    QCOMPARE(result.average.value(), 50.0);
    QCOMPARE(result.trend, AnalysisResult::STABLE);
    QVERIFY(std::abs(result.trendSlope) < 1e-9);
}
