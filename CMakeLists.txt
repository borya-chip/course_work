cmake_minimum_required(VERSION 3.16)
project(ProductCalculationSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/msys64/mingw64/lib/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Entities
set(ENTITY_SOURCES
    src/entities/Product.cpp
    src/entities/AbstractProduct.cpp
    src/entities/Order.cpp
)

set(ENTITY_HEADERS
    include/entities/Product.h
    include/entities/AbstractProduct.h
    include/entities/Order.h
    include/entities/OrderItem.h
    include/entities/ProductRepository.h
    include/entities/ProductRepository.tpp
)

# Services
set(SERVICE_SOURCES
    src/services/InventoryService.cpp
    src/services/WriteOffCalculator.cpp
)

set(SERVICE_HEADERS
    include/services/InventoryService.h
    include/services/WriteOffCalculator.h
    include/services/DiscountCalculator.h
)

# Managers
set(MANAGER_SOURCES
    src/managers/FileManager.cpp
    src/managers/DatabaseManager.cpp
)

set(MANAGER_HEADERS
    include/managers/FileManager.h
    include/managers/DatabaseManager.h
)

# UI - Main Window
set(UI_MAIN_SOURCES
    src/ui/MainWindow.cpp
)

set(UI_MAIN_HEADERS
    include/ui/MainWindow.h
)

# UI - Models
set(UI_MODEL_SOURCES
    src/ui/models/ProductModel.cpp
)

set(UI_MODEL_HEADERS
    include/ui/models/ProductModel.h
)

# UI - Delegates
set(UI_DELEGATE_SOURCES
    src/ui/delegates/ActionsDelegate.cpp
)

set(UI_DELEGATE_HEADERS
    include/ui/delegates/ActionsDelegate.h
)

# UI - Dialogs
set(UI_DIALOG_SOURCES
    src/ui/dialogs/ProductDialog.cpp
    src/ui/dialogs/OrderDialog.cpp
    src/ui/dialogs/AddProductToOrderDialog.cpp
    src/ui/dialogs/WriteOffDialog.cpp
    src/ui/dialogs/ReportDialog.cpp
    src/ui/dialogs/SalesReportDialog.cpp
    src/ui/dialogs/InventoryDialog.cpp
)

set(UI_DIALOG_HEADERS
    include/ui/dialogs/ProductDialog.h
    include/ui/dialogs/OrderDialog.h
    include/ui/dialogs/AddProductToOrderDialog.h
    include/ui/dialogs/WriteOffDialog.h
    include/ui/dialogs/ReportDialog.h
    include/ui/dialogs/SalesReportDialog.h
    include/ui/dialogs/InventoryDialog.h
)

# Exceptions
set(EXCEPTION_HEADERS
    include/exceptions/Exceptions.h
)

# Combine all sources and headers
set(SOURCES
    src/main.cpp
    ${ENTITY_SOURCES}
    ${SERVICE_SOURCES}
    ${MANAGER_SOURCES}
    ${UI_MAIN_SOURCES}
    ${UI_MODEL_SOURCES}
    ${UI_DELEGATE_SOURCES}
    ${UI_DIALOG_SOURCES}
)

set(HEADERS
    ${ENTITY_HEADERS}
    ${SERVICE_HEADERS}
    ${MANAGER_HEADERS}
    ${UI_MAIN_HEADERS}
    ${UI_MODEL_HEADERS}
    ${UI_DELEGATE_HEADERS}
    ${UI_DIALOG_HEADERS}
    ${EXCEPTION_HEADERS}
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
)
