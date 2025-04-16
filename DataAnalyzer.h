#ifndef DATAANALYZER_H
#define DATAANALYZER_H

#include <vector>
#include "DataStructures.h" // For MeasurementValue
#include <optional> // For returning optional results

struct AnalysisResult {
    std::optional<MeasurementValue> minVal;
    std::optional<MeasurementValue> maxVal;
    std::optional<double> average;
    // Trend could be simple linear regression slope, or just "increasing"/"decreasing"
    enum Trend { INCREASING, DECREASING, STABLE, UNKNOWN };
    Trend trend = Trend::UNKNOWN;
    double trendSlope = 0.0; // More specific trend indicator
};

class DataAnalyzer
{
public:
    DataAnalyzer();

    AnalysisResult analyze(const std::vector<MeasurementValue>& values);

private:
    // Helper to filter out invalid (NaN) values before analysis
    std::vector<MeasurementValue> filterValidValues(const std::vector<MeasurementValue>& values);
};

#endif // DATAANALYZER_H
