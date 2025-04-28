#include "DataAnalyzer.h"
#include <numeric>
#include <limits>
#include <cmath>
#include <algorithm>

DataAnalyzer::DataAnalyzer() {}


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
        return result;
    }


    auto minmax = std::minmax_element(validValues.begin(), validValues.end(),
                                      [](const MeasurementValue& a, const MeasurementValue& b) {
                                          return a.value < b.value;
                                      });
    result.minVal = *minmax.first;
    result.maxVal = *minmax.second;



    double sum = 0.0;
    for(const auto& mv : validValues) {
        sum += mv.value;
    }
    result.average = sum / validValues.size();


    if (validValues.size() >= 2) {
        double n = static_cast<double>(validValues.size());
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;

        qint64 firstTime = validValues.front().date.toSecsSinceEpoch();

        for (size_t i = 0; i < validValues.size(); ++i) {

            double x = static_cast<double>(validValues[i].date.toSecsSinceEpoch() - firstTime);
            double y = validValues[i].value;
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }

        double denominator = (n * sumX2 - sumX * sumX);
        if (std::abs(denominator) > 1e-9) {
            result.trendSlope = (n * sumXY - sumX * sumY) / denominator;


            if (result.trendSlope > 1e-5) {
                result.trend = AnalysisResult::INCREASING;
            } else if (result.trendSlope < -1e-5) {
                result.trend = AnalysisResult::DECREASING;
            } else {
                result.trend = AnalysisResult::STABLE;
            }
        } else {

            result.trend = AnalysisResult::UNKNOWN;
            result.trendSlope = 0.0;
        }

    } else {
        result.trend = AnalysisResult::UNKNOWN;
    }


    return result;
}
