/**
 * @file DataAnalyzer.h
 * @brief Definicja klasy DataAnalyzer oraz struktury AnalysisResult do przeprowadzania prostej analizy danych pomiarowych.
 */
#ifndef DATAANALYZER_H
#define DATAANALYZER_H

#include <vector>
#include <optional> // Dla opcjonalnych wyników
#include "DataStructures.h" // Potrzebuje MeasurementValue

/**
 * @struct AnalysisResult
 * @brief Przechowuje wyniki prostej analizy statystycznej serii danych pomiarowych.
 *
 * Zawiera opcjonalne wartości minimalną i maksymalną (wraz z datą), średnią,
 * oraz informacje o trendzie (rosnący, malejący, stabilny) obliczonym na podstawie regresji liniowej.
 */
struct AnalysisResult {
    /**
     * @brief Minimalna znaleziona wartość pomiarowa (wraz z datą).
     * Jest `std::nullopt`, jeśli wejściowe dane były puste lub zawierały tylko NaN.
     */
    std::optional<MeasurementValue> minVal;

    /**
     * @brief Maksymalna znaleziona wartość pomiarowa (wraz z datą).
      * Jest `std::nullopt`, jeśli wejściowe dane były puste lub zawierały tylko NaN.
    */
    std::optional<MeasurementValue> maxVal;

    /**
     * @brief Średnia arytmetyczna z ważnych wartości pomiarowych.
     * Jest `std::nullopt`, jeśli wejściowe dane były puste lub zawierały tylko NaN.
     */
    std::optional<double> average;

    /**
     * @enum Trend
     * @brief Typ wyliczeniowy reprezentujący kierunek trendu w danych.
     */
    enum Trend {
        INCREASING, ///< Trend rosnący (nachylenie > progu).
        DECREASING, ///< Trend malejący (nachylenie < -progu).
        STABLE,     ///< Trend stabilny (nachylenie bliskie zeru).
        UNKNOWN     ///< Trend nieznany (np. za mało danych).
    };

    ///< Wykryty trend w danych. Domyślnie UNKNOWN.
    Trend trend = Trend::UNKNOWN;

    /**
     * @brief Nachylenie linii trendu obliczone metodą regresji liniowej.
     * Wartość 0.0, jeśli trend jest UNKNOWN lub idealnie stabilny.
     * Czas jest traktowany jako oś X (w sekundach od pierwszego pomiaru), wartość jako oś Y.
     */
    double trendSlope = 0.0;
};

/**
 * @class DataAnalyzer
 * @brief Klasa odpowiedzialna za przeprowadzanie prostej analizy statystycznej danych pomiarowych.
 *
 * Główna metoda `analyze` przyjmuje wektor `MeasurementValue` i zwraca obiekt `AnalysisResult`.
 * Analiza obejmuje filtrowanie nieprawidłowych danych (NaN), obliczenie min/max, średniej oraz trendu.
 */
class DataAnalyzer
{
public:
    /**
     * @brief Domyślny konstruktor.
     */
    DataAnalyzer();

    /**
     * @brief Analizuje podany wektor danych pomiarowych.
     * @param values Wektor obiektów MeasurementValue do analizy.
     * @return Obiekt AnalysisResult zawierający wyniki analizy (min, max, średnia, trend).
     * @note Wartości NaN oraz wpisy z nieprawidłową datą są ignorowane podczas obliczeń.
     *       Trend jest obliczany tylko jeśli dostępne są co najmniej 2 prawidłowe punkty danych.
     */
    AnalysisResult analyze(const std::vector<MeasurementValue>& values);

private:
    /**
     * @brief Filtruje wejściowy wektor danych, zwracając tylko te pomiary, które mają prawidłową datę i wartość niebędącą NaN.
     * @param values Oryginalny wektor danych pomiarowych.
     * @return Wektor zawierający tylko prawidłowe obiekty MeasurementValue.
     */
    std::vector<MeasurementValue> filterValidValues(const std::vector<MeasurementValue>& values);
};

#endif // DATAANALYZER_H
