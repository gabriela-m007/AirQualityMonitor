Monitor Jakości Powietrza ("AirQualityMonitor") - Projekt JPO 2024/2025


Aplikacja desktopowa (C++/Qt) do monitorowania jakości powietrza w Polsce, wykorzystująca API Głównego Inspektoratu Ochrony Środowiska (GIOS).


Główne funkcje


* Pobieranie danych z API GIOS:
   * Lista stacji pomiarowych (z możliwością filtrowania po mieście).
   * Lista czujników dla wybranej stacji.
   * Dane pomiarowe (serie czasowe) dla czujnika.
   * Aktualny Indeks Jakości Powietrza (AQI) dla stacji.
* Lokalny cache: Zapisywanie i wczytywanie pobranych danych (JSON) w celu optymalizacji i pracy offline.
* Wizualizacja i analiza:
   * Interaktywny wykres danych pomiarowych (QtCharts) z filtrowaniem zakresu dat.
   * Podstawowe statystyki (min, max, średnia, trend liniowy).
* Asynchroniczne operacje: Pobieranie danych w tle (wielowątkowość), aby nie blokować interfejsu użytkownika.
* Obsługa błędów: Zarządzanie problemami sieciowymi, z opcją użycia danych z cache.
* Dokumentacja: Generowana za pomocą Doxygen.
* Testy: Testy jednostkowe z użyciem Qt Test.


Wymagania


* System: Windows/Linux/macOS
* Kompilator: Wspierający C++17
* Biblioteka Qt: Wersja 6.x (moduły: Core, Gui, Widgets, Network, Charts, Testlib)
* Doxygen: (Opcjonalnie) Do generowania dokumentacji.
* Połączenie internetowe: Do pobierania aktualnych danych.


Budowanie i Uruchamianie


Projekt wykorzystuje QMake. Zalecane jest użycie Qt Creator.
1. Otwórz plik AirQualityMonitor.pro w Qt Creator. (Upewnij się, że nazwa pliku jest poprawna)
2. Skonfiguruj odpowiedni zestaw Qt (Kit).
3. Zbuduj projekt (Ctrl+B).
4. Uruchom aplikację.
