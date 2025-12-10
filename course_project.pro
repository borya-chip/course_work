QT += core widgets charts

CONFIG += c++17
TARGET = ProductCalculationSystems
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/Product.cpp \
    src/AbstractProduct.cpp \
    src/InventoryManager.cpp \
    src/WriteOffCalculator.cpp \
    src/FileManager.cpp \
    src/DatabaseManager.cpp \
    src/Order.cpp \
    src/OrderDialog.cpp \
    src/AddProductToOrderDialog.cpp \
    src/SalesReportDialog.cpp \
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
    include/InventoryManager.h \
    include/WriteOffCalculator.h \
    include/ProductRepository.h \
    include/Exceptions.h \
    include/FileManager.h \
    include/DatabaseManager.h \
    include/Order.h \
    include/OrderItem.h \
    include/OrderDialog.h \
    include/AddProductToOrderDialog.h \
    include/DiscountCalculator.h \
    include/SalesReportDialog.h \
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
