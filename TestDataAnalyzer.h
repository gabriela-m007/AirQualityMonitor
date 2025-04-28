#ifndef TESTDATAANALYZER_H
#define TESTDATAANALYZER_H

#include <QObject>
#include <QtTest/QtTest>
#include "DataAnalyzer.h"
#include "DataStructures.h"
#include <vector>
#include <cmath>

class TestDataAnalyzer : public QObject
{
    Q_OBJECT

private:
    DataAnalyzer analyzer;

        std::vector<MeasurementValue> createTestData(const std::vector<double>& values,
                                                 const QDateTime& startTime,
                                                 int stepSeconds = 3600);

private slots:
    void analyze_EmptyData();
    void analyze_SingleValue();
    void analyze_AllNaNValues();
    void analyze_IncreasingTrend();
    void analyze_DecreasingTrend();
    void analyze_StableTrend();
    void analyze_StableTrend_TwoPoints();
    void analyze_MixedDataWithNaN();
    void analyze_DataWithZeroVariance();
};

#endif
