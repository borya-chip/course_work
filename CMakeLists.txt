cmake_minimum_required(VERSION 3.16)
project(ProductCalculationSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/msys64/mingw64/lib/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/Product.cpp
    src/AbstractProduct.cpp
    src/FoodProduct.cpp
    src/NonFoodProduct.cpp
    src/InventoryManager.cpp
    src/WriteOffCalculator.cpp
    src/FileManager.cpp
    src/DatabaseManager.cpp
    src/ProductModel.cpp
    src/MainWindow.cpp
    src/ProductDialog.cpp
    src/WriteOffDialog.cpp
    src/ReportDialog.cpp
    src/ActionsDelegate.cpp
    src/InventoryDialog.cpp
)

set(HEADERS
    include/Product.h
    include/AbstractProduct.h
    include/FoodProduct.h
    include/NonFoodProduct.h
    include/InventoryManager.h
    include/WriteOffCalculator.h
    include/ProductRepository.h
    include/Exceptions.h
    include/FileManager.h
    include/DatabaseManager.h
    include/ProductModel.h
    include/MainWindow.h
    include/ProductDialog.h
    include/WriteOffDialog.h
    include/ReportDialog.h
    include/ActionsDelegate.h
    include/InventoryDialog.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
)


