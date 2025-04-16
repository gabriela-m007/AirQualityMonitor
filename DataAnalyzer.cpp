#include "DataAnalyzer.h"
#include <numeric> // For std::accumulate
#include <limits>
#include <cmath>   // For std::isnan
#include <algorithm> // For std::minmax_element

DataAnalyzer::DataAnalyzer() {}

// Helper to filter out invalid (NaN) values
std::vector<MeasurementValue> DataAnalyzer::filterValidValues(const std::vector<MeasurementValue>& values) {
    std::vector<MeasurementValue> validValues;
    for (const auto& mv : values) {
        if (mv.date.isValid() && !std::isnan(mv.value)) {
            validValues.push_back(mv);
        }
    }
    return validValues;
}


AnalysisResult DataAnalyzer::analyze(const std::vector<MeasurementValue>& values) {
    AnalysisResult result;
    std::vector<MeasurementValue> validValues = filterValidValues(values);


    if (validValues.empty()) {
        return result; // Return empty result if no valid data
    }

    // Calculate Min/Max
    auto minmax = std::minmax_element(validValues.begin(), validValues.end(),
                                      [](const MeasurementValue& a, const MeasurementValue& b) {
                                          return a.value < b.value;
                                      });
    result.minVal = *minmax.first;
    result.maxVal = *minmax.second;


    // Calculate Average
    double sum = 0.0;
    for(const auto& mv : validValues) {
        sum += mv.value;
    }
    result.average = sum / validValues.size();

    // Calculate Trend (Simple Linear Regression Slope)
    if (validValues.size() >= 2) {
        double n = static_cast<double>(validValues.size());
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
        // Use seconds since epoch as X for simplicity, assumes somewhat regular intervals
        // A better approach might use proper time difference weighting if intervals vary wildly
        qint64 firstTime = validValues.front().date.toSecsSinceEpoch();

        for (size_t i = 0; i < validValues.size(); ++i) {
            // Using index 'i' as X is simpler if intervals are regular
            // double x = static_cast<double>(i);
            // Using time delta as X
            double x = static_cast<double>(validValues[i].date.toSecsSinceEpoch() - firstTime);
            double y = validValues[i].value;
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }

        double denominator = (n * sumX2 - sumX * sumX);
        if (std::abs(denominator) > 1e-9) { // Avoid division by zero
            result.trendSlope = (n * sumXY - sumX * sumY) / denominator;

            // Determine qualitative trend based on slope
            if (result.trendSlope > 1e-6) { // Threshold to consider increasing
                result.trend = AnalysisResult::INCREASING;
            } else if (result.trendSlope < -1e-6) { // Threshold to consider decreasing
                result.trend = AnalysisResult::DECREASING;
            } else {
                result.trend = AnalysisResult::STABLE;
            }
        } else {
            // Not enough variation in time or only one point after filtering time
            result.trend = AnalysisResult::UNKNOWN;
            result.trendSlope = 0.0; // Or NaN
        }

    } else {
        result.trend = AnalysisResult::UNKNOWN; // Not enough data for trend
    }


    return result;
}
