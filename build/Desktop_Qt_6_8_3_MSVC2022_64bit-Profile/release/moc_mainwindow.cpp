/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10MainWindowE = QtMocHelpers::stringData(
    "MainWindow",
    "on_fetchStationsButton_clicked",
    "",
    "on_stationsListWidget_itemClicked",
    "QListWidgetItem*",
    "item",
    "on_sensorsListWidget_itemClicked",
    "on_loadStationsButton_clicked",
    "on_saveStationsButton_clicked",
    "on_loadSensorDataButton_clicked",
    "on_saveSensorDataButton_clicked",
    "on_analyzeButton_clicked",
    "handleStationsReady",
    "std::vector<MeasuringStation>",
    "stations",
    "handleSensorsReady",
    "std::vector<Sensor>",
    "sensors",
    "handleSensorDataReady",
    "SensorData",
    "data",
    "handleAirQualityIndexReady",
    "AirQualityIndex",
    "index",
    "handleNetworkError",
    "errorMsg"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    1,   93,    2, 0x08,    2 /* Private */,
       6,    1,   96,    2, 0x08,    4 /* Private */,
       7,    0,   99,    2, 0x08,    6 /* Private */,
       8,    0,  100,    2, 0x08,    7 /* Private */,
       9,    0,  101,    2, 0x08,    8 /* Private */,
      10,    0,  102,    2, 0x08,    9 /* Private */,
      11,    0,  103,    2, 0x08,   10 /* Private */,
      12,    1,  104,    2, 0x08,   11 /* Private */,
      15,    1,  107,    2, 0x08,   13 /* Private */,
      18,    1,  110,    2, 0x08,   15 /* Private */,
      21,    1,  113,    2, 0x08,   17 /* Private */,
      24,    1,  116,    2, 0x08,   19 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void, QMetaType::QString,   25,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN10MainWindowE.offsetsAndSizes,
    qt_meta_data_ZN10MainWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10MainWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_fetchStationsButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_stationsListWidget_itemClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'on_sensorsListWidget_itemClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'on_loadStationsButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_saveStationsButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_loadSensorDataButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_saveSensorDataButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_analyzeButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleStationsReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::vector<MeasuringStation> &, std::false_type>,
        // method 'handleSensorsReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const std::vector<Sensor> &, std::false_type>,
        // method 'handleSensorDataReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const SensorData &, std::false_type>,
        // method 'handleAirQualityIndexReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AirQualityIndex &, std::false_type>,
        // method 'handleNetworkError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->on_fetchStationsButton_clicked(); break;
        case 1: _t->on_stationsListWidget_itemClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 2: _t->on_sensorsListWidget_itemClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 3: _t->on_loadStationsButton_clicked(); break;
        case 4: _t->on_saveStationsButton_clicked(); break;
        case 5: _t->on_loadSensorDataButton_clicked(); break;
        case 6: _t->on_saveSensorDataButton_clicked(); break;
        case 7: _t->on_analyzeButton_clicked(); break;
        case 8: _t->handleStationsReady((*reinterpret_cast< std::add_pointer_t<std::vector<MeasuringStation>>>(_a[1]))); break;
        case 9: _t->handleSensorsReady((*reinterpret_cast< std::add_pointer_t<std::vector<Sensor>>>(_a[1]))); break;
        case 10: _t->handleSensorDataReady((*reinterpret_cast< std::add_pointer_t<SensorData>>(_a[1]))); break;
        case 11: _t->handleAirQualityIndexReady((*reinterpret_cast< std::add_pointer_t<AirQualityIndex>>(_a[1]))); break;
        case 12: _t->handleNetworkError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10MainWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
