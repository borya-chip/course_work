QT += core widgets

CONFIG += c++17
TARGET = ProductCalculationSystem
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/Product.cpp \
    src/AbstractProduct.cpp \
    src/FoodProduct.cpp \
    src/NonFoodProduct.cpp \
    src/InventoryManager.cpp \
    src/WriteOffCalculator.cpp \
    src/FileManager.cpp \
    src/DatabaseManager.cpp \
    src/ProductModel.cpp \
    src/MainWindow.cpp \
    src/ProductDialog.cpp \
    src/WriteOffDialog.cpp \
    src/ReportDialog.cpp \
    src/ActionsDelegate.cpp \
    src/InventoryDialog.cpp

HEADERS += \
    include/Product.h \
    include/AbstractProduct.h \
    include/FoodProduct.h \
    include/NonFoodProduct.h \
    include/InventoryManager.h \
    include/WriteOffCalculator.h \
    include/ProductRepository.h \
    include/Exceptions.h \
    include/FileManager.h \
    include/DatabaseManager.h \
    include/ProductModel.h \
    include/MainWindow.h \
    include/ProductDialog.h \
    include/WriteOffDialog.h \
    include/ReportDialog.h \
    include/ActionsDelegate.h \
    include/InventoryDialog.h

# Template implementation file
OTHER_FILES += \
    src/ProductRepository.tpp

INCLUDEPATH += include src

CONFIG += debug
QMAKE_CXXFLAGS += -fPIC
QMAKE_LFLAGS += -static-libgcc -static-libstdc++
CONFIG += c++17
