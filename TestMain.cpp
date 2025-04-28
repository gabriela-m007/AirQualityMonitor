#include <QtTest/QtTest>
#include <QApplication>

#include "testdataparser.h"
#include "testdataanalyzer.h"
#include "testdatastorage.h"

int main(int argc, char** argv) {

    Q_UNUSED(argc);
    Q_UNUSED(argv);

    int status = 0;

    qInfo() << "Uruchamianie testów dla DataParser...";
    {
        TestDataParser tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    qInfo() << "Uruchamianie testów dla DataAnalyzer...";
    {
        TestDataAnalyzer tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    qInfo() << "Uruchamianie testów dla DataStorage...";
    {
        TestDataStorage tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    qInfo() << "Zakończono wszystkie testy.";
    return status;
}
