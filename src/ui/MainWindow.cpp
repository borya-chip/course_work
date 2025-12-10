#include "ui/MainWindow.h"
#include "ui/dialogs/ProductDialog.h"
#include "ui/dialogs/WriteOffDialog.h"
#include "ui/dialogs/ReportDialog.h"
#include "services/InventoryService.h"
#include "services/ProductFilterService.h"
#include "services/ProductValidator.h"
#include "services/WriteOffService.h"
#include "services/InventoryAdjustmentService.h"
#include "services/OrderService.h"
#include "managers/FileManager.h"
#include "exceptions/Exceptions.h"
#include "ui/delegates/ActionsDelegate.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGroupBox>
#include <QFrame>
#include <QTextStream>
#include <QTableWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QStandardItemModel>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QStandardItem>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QBarLegendMarker>
#include <QLegend>
#include <QPainter>
#include <QMap>
#include <QList>
#include <QApplication>
#include <algorithm>
#include <cctype>
#include <string>
#include "entities/Order.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), inventoryManager(new InventoryService()) {
    
    dbManager = DatabaseManager::getInstance();
    dbManager->initializeDatabase();
    
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }
    dataFilePath = dataPath + "/inventory.dat";
    
    if (QFile::exists(dataFilePath)) {
        FileManager::loadFromBinary(*inventoryManager, dataFilePath.toStdString());
    }
    
    // Sync nextId with database to avoid ID conflicts
    auto products = inventoryManager->getAllProducts();
    int maxId = 0;
    for (const auto& product : products) {
        if (product && product->getId() > maxId) {
            maxId = product->getId();
        }
    }
    if (maxId > 0) {
        Product::setNextId(maxId);
    }
    
    writeOffsReportTextEdit = nullptr;
    setupUI();
    productModel->refresh();
}

MainWindow::~MainWindow() {
    FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
    delete inventoryManager;
}

QString MainWindow::updateWriteOffsReport() {
    if (!inventoryManager) {
        return QString();
    }

    QString writeOffsText = "=== WRITE-OFFS REPORT ===\n\n";
    auto writeOffHistory = inventoryManager->getWriteOffHistory();
    writeOffsText += QString("Total Write-offs: %1\n\n").arg(writeOffHistory.size());

    double totalValue = 0.0;
    for (const auto& product : writeOffHistory) {
        if (product) {
            writeOffsText += QString("ID: %1\n").arg(product->getId());
            writeOffsText += QString("Product: %1\n").arg(QString::fromStdString(product->getName()));
            writeOffsText += QString("Quantity: %1\n").arg(product->getQuantity());
            double value = product->calculateTotalValue();
            writeOffsText += QString("Value: $%1\n").arg(QString::number(value, 'f', 2));
            writeOffsText += "---\n\n";
            totalValue += value;
        }
    }

    writeOffsText += QString("Total Write-off Value: $%1\n").arg(QString::number(totalValue, 'f', 2));

    // Если текстовый виджет существует (раздел отчётов открыт) — обновим его,
    // иначе просто вернём текст, чтобы можно было, например, экспортировать в файл.
    if (writeOffsReportTextEdit) {
        writeOffsReportTextEdit->setPlainText(writeOffsText);
    }
    return writeOffsText;
}

void MainWindow::setupUI() {
    setWindowTitle("Product Calculation and Write-off System");
    setMinimumSize(1200, 700);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Main content area with sidebar
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    
    // Setup sidebar
    setupSidebar();
    splitter->addWidget(sidebarMenu);
    sidebarMenu->setMaximumWidth(250);
    sidebarMenu->setMinimumWidth(200);
    
    // Setup content area
    setupContentArea();
    splitter->addWidget(contentWidget);
    
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
}

void MainWindow::setupSidebar() {
    sidebarMenu = new QTreeWidget(this);
    sidebarMenu->setHeaderHidden(true);
    sidebarMenu->setRootIsDecorated(false);
    sidebarMenu->setIndentation(0);
    
    sidebarMenu->setStyleSheet(
        "QTreeWidget {"
        "    background-color: #f5f5f5;"
        "    border: none;"
        "    border-right: 1px solid #ddd;"
        "    font-size: 14px;"
        "    outline: none;"
        "}"
        "QTreeWidget::item {"
        "    padding: 12px 20px;"
        "    border: none;"
        "    background: transparent;"
        "    color: #333;"
        "    min-height: 20px;"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: #2196F3;"
        "    color: white;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: #e3f2fd;"
        "    color: #1976d2;"
        "}"
        "QTreeWidget::item:hover:selected {"
        "    background-color: #1976d2;"
        "    color: white;"
        "}"
    );
    
    // Warehouse section
    QTreeWidgetItem* warehouseItem = new QTreeWidgetItem(sidebarMenu);
    warehouseItem->setText(0, "Warehouse");
    warehouseItem->setData(0, Qt::UserRole, "warehouse");
    
    // Orders section
    QTreeWidgetItem* ordersItem = new QTreeWidgetItem(sidebarMenu);
    ordersItem->setText(0, "Orders");
    ordersItem->setData(0, Qt::UserRole, "orders");
    
    // Inventory section
    QTreeWidgetItem* inventoryItem = new QTreeWidgetItem(sidebarMenu);
    inventoryItem->setText(0, "Inventory");
    inventoryItem->setData(0, Qt::UserRole, "inventory");
    
    // Reports section
    QTreeWidgetItem* reportsItem = new QTreeWidgetItem(sidebarMenu);
    reportsItem->setText(0, "Reports");
    reportsItem->setData(0, Qt::UserRole, "reports");
    
    connect(sidebarMenu, &QTreeWidget::itemClicked, this, &MainWindow::onSidebarItemClicked);
}

void MainWindow::onSidebarItemClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    QString action = item->data(0, Qt::UserRole).toString();
    
    // Clear content area
    QLayoutItem* child;
    while ((child = contentWidget->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    // Любые виджеты отчётов (включая QTextEdit отчёта списаний) сейчас уничтожены,
    // поэтому сбросим указатель, чтобы не обращаться к висячему объекту.
    writeOffsReportTextEdit = nullptr;
    
    if (action == "warehouse") {
        contentWidget->layout()->addWidget(createWarehouseSection());
    } else if (action == "orders") {
        contentWidget->layout()->addWidget(createOrdersSection());
    } else if (action == "inventory") {
        contentWidget->layout()->addWidget(createInventorySection());
    } else if (action == "reports") {
        contentWidget->layout()->addWidget(createReportsSection());
    }
}

void MainWindow::setupContentArea() {
    contentWidget = new QWidget(this);
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(15);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    
    // Add warehouse section by default
    contentLayout->addWidget(createWarehouseSection());
}

QWidget* MainWindow::createWarehouseSection() {
    QWidget* sectionWidget = new QWidget(this);
    QVBoxLayout* sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setSpacing(15);
    sectionLayout->setContentsMargins(0, 0, 0, 0);
    
    // Section title
    QLabel* sectionTitle = new QLabel("PRODUCT MANAGEMENT", this);
    sectionTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "    color: #333;"
        "    padding: 10px 0;"
        "}"
    );
    sectionLayout->addWidget(sectionTitle);
    
    // Controls bar
    QWidget* controlsBar = new QWidget(this);
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlsBar);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(10);
    
    setupSearchBar();
    setupButtons();
    
    addButton->setText("Add Product");
    addButton->setMinimumHeight(35);
    addButton->setStyleSheet(getSuccessButtonStyle());
    controlsLayout->addWidget(addButton);
    
    searchLineEdit->setPlaceholderText("Search...");
    searchLineEdit->setMinimumHeight(35);
    searchLineEdit->setMinimumWidth(200);
    searchLineEdit->setStyleSheet(
        "QLineEdit {"
        "    border: 2px solid #ddd;"
        "    border-radius: 5px;"
        "    padding: 5px 10px;"
        "    font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #4CAF50;"
        "}"
    );
    controlsLayout->addWidget(searchLineEdit);
    
    categoryComboBox->setMinimumHeight(35);
    categoryComboBox->setMinimumWidth(150);
    categoryComboBox->setStyleSheet(
        "QComboBox {"
        "    border: 2px solid #ddd;"
        "    border-radius: 5px;"
        "    padding: 5px 10px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "}"
        "QComboBox:hover {"
        "    border: 2px solid #4CAF50;"
        "}"
    );
    controlsLayout->addWidget(categoryComboBox);
    
    controlsLayout->addStretch();
    
    sectionLayout->addWidget(controlsBar);
    
    // Table
    setupTable();
    sectionLayout->addWidget(tableView);

    // Подключаем сигналы к только что созданным элементам управления
    connectSignals();
    
    return sectionWidget;
}

void MainWindow::setupTable() {
    tableView = new QTableView(this);
    productModel = new ProductModel(inventoryManager, this);
    tableView->setModel(productModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::NoSelection);
    tableView->setAlternatingRowColors(false);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setSortingEnabled(false);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->verticalHeader()->setVisible(false);
    tableView->verticalHeader()->setMaximumWidth(0);
    
    tableView->setStyleSheet(
        "QTableView {"
        "    border: 1px solid #ddd;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: transparent;"
        "    selection-color: #333;"
        "    outline: none;"
        "}"
        "QTableView::item {"
        "    padding: 8px;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableView::item:selected {"
        "    background-color: transparent;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableView::item:hover {"
        "    background-color: transparent;"
        "}"
        "QTableView::item:focus {"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableView:focus {"
        "    outline: none;"
        "}"
        "QTableView::item:!focus {"
        "    border: none;"
        "    outline: none;"
        "}"
    );
    tableView->setFocusPolicy(Qt::NoFocus);
    
    tableView->setStyleSheet(tableView->styleSheet() +
        "QHeaderView::section {"
        "    background-color: #f5f5f5;"
        "    color: #333;"
        "    padding: 10px;"
        "    border: none;"
        "    border-bottom: 2px solid #ddd;"
        "    font-weight: bold;"
        "    font-size: 13px;"
        "}"
        "QHeaderView::section:hover {"
        "    background-color: #f5f5f5;"
        "}"
    );
    
    actionsDelegate = new ActionsDelegate(this);
    tableView->setItemDelegateForColumn(ProductModel::Actions, actionsDelegate);
    
    connect(actionsDelegate, &ActionsDelegate::editRequested, this, &MainWindow::editProductByRow);
    connect(actionsDelegate, &ActionsDelegate::deleteRequested, this, &MainWindow::deleteProductByRow);
    connect(actionsDelegate, &ActionsDelegate::writeOffRequested, this, &MainWindow::writeOffProductByRow);
}

void MainWindow::setupSearchBar() {
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Search by name...");
    
    categoryComboBox = new QComboBox(this);
    categoryComboBox->addItem("All Categories");
    categoryComboBox->addItem("Food");
    categoryComboBox->addItem("Electronics");
    categoryComboBox->addItem("Clothing");
    categoryComboBox->addItem("Books");
    categoryComboBox->addItem("Other");
}

void MainWindow::setupButtons() {
    addButton = new QPushButton("Add Product", this);
}

void MainWindow::connectSignals() {
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addProduct);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::searchProducts);
    connect(categoryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::filterByCategory);
    connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged);
}

void MainWindow::refreshTable() {
    if (productModel) {
        productModel->refresh();
    }
}

void MainWindow::addProduct() {
    ProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            Product product = dialog.getProduct();
            
            // Проверяем ID через ProductValidator (диапазон + уникальность)
            QString idError = ProductValidator::validateId(*inventoryManager, product.getId());
            if (!idError.isEmpty()) {
                QMessageBox::warning(this, "Error", idError);
                return;
            }
            
            // Create shared_ptr and add to inventory
            auto productPtr = std::make_shared<Product>(product);
            inventoryManager->addProduct(productPtr);
            
            // Also add to database
            dbManager->addProduct(product);
            
            // Refresh model and save
            productModel->refresh();
            FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
            
            QMessageBox::information(this, "Success", 
                QString("Product added successfully!\nID: %1").arg(product.getId()));
        } catch (const ProductException& e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Error", QString("Failed to add product: %1").arg(e.what()));
        } catch (...) {
            QMessageBox::critical(this, "Error", "An unexpected error occurred while adding the product.");
        }
    }
}


void MainWindow::generateReport() {
    ReportDialog dialog(*inventoryManager, this);
    dialog.exec();
}

void MainWindow::openInventory() {
    InventoryDialog dialog(inventoryManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        productModel->refresh();
        
        FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
    }
}

void MainWindow::createOrder() {
    // Get all products from InventoryService
    auto productPtrs = inventoryManager->getAllProducts();
    std::vector<Product> products;
    products.reserve(productPtrs.size());
    
    for (const auto& productPtr : productPtrs) {
        if (productPtr) {
            products.push_back(*productPtr);
        }
    }
    
    if (products.empty()) {
        QMessageBox::warning(this, "Error", "No products available to create an order!");
        return;
    }
    
    OrderDialog dialog(products, this);
    if (dialog.exec() == QDialog::Accepted) {
        Order order = dialog.getOrder();

        // Создаём заказ через сервис бизнес-логики
        OrderService::Result result = OrderService::createOrder(
            *inventoryManager,
            *dbManager,
            order
        );

        if (!result.saved) {
            QMessageBox::warning(this, "Error", "Failed to save order!");
            return;
        }

        // Save updated inventory
        FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
        
        // Refresh product model
        if (productModel) {
            productModel->refresh();
        }
        
        // Save order history to txt file
        saveOrderHistoryToTxt(order);
        
        QMessageBox::information(this, "Success", 
            QString("Order #%1 created successfully!\nAmount: $%2")
            .arg(order.getId())
            .arg(result.totalAmount, 0, 'f', 2));
    }
}

void MainWindow::saveOrderHistoryToTxt(const Order& order) {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }
    
    QString historyFilePath = dataPath + "/order_history.txt";
    QFile file(historyFilePath);
    
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        
        out << "=== ORDER #" << order.getId() << " ===\n";
        out << "Date: " << order.getOrderDate().toString("yyyy-MM-dd") << "\n";
        out << "Company: " << order.getCompanyName() << "\n";
        out << "Contact Person: " << order.getContactPerson() << "\n";
        out << "Phone: " << order.getPhone() << "\n";
        out << "Type: " << order.getOrderTypeString() << "\n";
        out << "Items:\n";
        
        for (const auto& item : order.getItems()) {
            out << "  - " << item.productName << " (ID: " << item.productId << ")\n";
            out << "    Quantity: " << item.quantity << "\n";
            out << "    Unit Price: $" << QString::number(item.unitPrice, 'f', 2) << "\n";
            out << "    Discount: " << QString::number(item.discountPercent, 'f', 1) << "%\n";
            out << "    Total: $" << QString::number(item.totalPrice, 'f', 2) << "\n";
        }
        
        out << "Total Amount: $" << QString::number(order.getTotalAmount(), 'f', 2) << "\n";
        out << "Total Discount: $" << QString::number(order.getTotalDiscount(), 'f', 2) << "\n";
        out << "---\n\n";
        
        file.close();
    }
}

void MainWindow::editOrder(int orderId) {
    Order order = dbManager->getOrder(orderId);
    if (order.getId() == 0) {
        QMessageBox::warning(this, "Error", "Order not found!");
        return;
    }
    
    // Get all products from InventoryService
    auto productPtrs = inventoryManager->getAllProducts();
    std::vector<Product> products;
    products.reserve(productPtrs.size());
    
    for (const auto& productPtr : productPtrs) {
        if (productPtr) {
            products.push_back(*productPtr);
        }
    }
    
    if (products.empty()) {
        QMessageBox::warning(this, "Error", "No products available!");
        return;
    }
    
    OrderDialog dialog(products, this);
    // Set existing order data
    dialog.setOrder(order);
    
    if (dialog.exec() == QDialog::Accepted) {
        Order updatedOrder = dialog.getOrder();
        updatedOrder.setId(orderId); // Keep original ID
        
        if (dbManager->updateOrder(updatedOrder)) {
            QMessageBox::information(this, "Success", 
                QString("Order #%1 updated successfully!")
                .arg(updatedOrder.getId()));
            
            // Refresh orders section
            QLayoutItem* child;
            while ((child = contentWidget->layout()->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }
            contentWidget->layout()->addWidget(createOrdersSection());
        } else {
            QMessageBox::warning(this, "Error", "Failed to update order!");
        }
    }
}

void MainWindow::showSalesReport() {
    SalesReportDialog dialog(this);
    dialog.exec();
}


void MainWindow::exportReport() {
    QString filename = QFileDialog::getSaveFileName(this, "Export Report",
                                                    "", "Text Files (*.txt)");
    if (!filename.isEmpty()) {
        if (FileManager::exportReportToText(*inventoryManager, filename.toStdString())) {
            QMessageBox::information(this, "Success", "Report exported successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to export report.");
        }
    }
}

void MainWindow::searchProducts() {
    applyFilters();
}

void MainWindow::filterByCategory() {
    applyFilters();
}

void MainWindow::applyFilters() {
    if (!inventoryManager || !productModel || !categoryComboBox || !searchLineEdit) {
        return;
    }
    
    QString category = categoryComboBox->currentText();
    QString searchText = searchLineEdit->text().trimmed();
    
    // Получаем отфильтрованный список через сервис
    auto products = ProductFilterService::filterProducts(*inventoryManager, category, searchText);
    
    // Convert to Product list and update model
    std::vector<Product> productList;
    productList.reserve(products.size());
    for (const auto& p : products) {
        if (p) {
            productList.push_back(*p);
        }
    }
    productModel->setProducts(productList);
}


void MainWindow::onSelectionChanged() {
    Q_UNUSED(tableView->selectionModel()->selectedRows());
}


void MainWindow::editProductByRow(int row) {
    if (row < 0 || row >= productModel->rowCount()) {
        return;
    }
    
    Product product = productModel->getProduct(row);
    ProductDialog dialog(product, this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            Product updatedProduct = dialog.getProduct();
            
            auto productPtr = std::make_shared<Product>(updatedProduct);
            inventoryManager->updateProduct(product.getId(), productPtr);
            
            productModel->refresh();
            
            FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
            
            QMessageBox::information(this, "Success", "Product updated successfully!");
        } catch (const ProductException& e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::deleteProductByRow(int row) {
    if (row < 0 || row >= productModel->rowCount()) {
        return;
    }
    
    Product product = productModel->getProduct(row);
    
    int ret = QMessageBox::question(this, "Confirm Delete",
                                    QString("Are you sure you want to delete product '%1'?")
                                    .arg(QString::fromStdString(product.getName())),
                                    QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        try {
            inventoryManager->deleteProduct(product.getId());
            
            productModel->refresh();
            
            FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
            
            QMessageBox::information(this, "Success", "Product deleted successfully!");
        } catch (const ProductException& e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::writeOffProductByRow(int row) {
    if (row < 0 || row >= productModel->rowCount()) {
        return;
    }
    
    if (!productModel || !inventoryManager || !dbManager) {
        QMessageBox::critical(this, "Error", "Internal error: Missing required components.");
        return;
    }
    
    Product product = productModel->getProduct(row);
    
    WriteOffDialog dialog(product, this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            int quantity = dialog.getWriteOffQuantity();
            QString reason = dialog.getReason();
            
            // Validate quantity
            if (quantity <= 0 || quantity > product.getQuantity()) {
                QMessageBox::warning(this, "Error", "Invalid write-off quantity.");
                return;
            }
            
            // Calculate write-off value
            double writeOffValue = quantity * product.getUnitPrice();
            
            // Get product name before write-off
            QString productName = QString::fromStdString(product.getName());
            if (productName.isEmpty()) {
                productName = "Unknown Product";
            }
            
            // Выполняем списание через сервис бизнес-логики
            try {
                auto result = WriteOffService::writeOffProduct(
                    *inventoryManager,
                    dbManager,
                    product.getId(),
                    quantity,
                    reason,
                    productName
                );
                Q_UNUSED(result);
            } catch (const ProductException& e) {
                QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
                return;
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", QString("Failed to write off product: %1").arg(e.what()));
                return;
            } catch (...) {
                QMessageBox::critical(this, "Error", "Failed to write off product: unknown error");
                return;
            }
            
            // Save to file first (critical operation)
            bool saveSuccess = false;
            try {
                if (inventoryManager) {
                    saveSuccess = FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
                    if (!saveSuccess) {
                        QMessageBox::warning(this, "Warning", "Product written off, but failed to save to file.");
                    }
                }
            } catch (const std::exception& e) {
                qDebug() << "Error saving to file:" << e.what();
                QMessageBox::critical(this, "Error", QString("Failed to save data to file: %1").arg(e.what()));
            } catch (...) {
                qDebug() << "Unknown error saving to file";
                QMessageBox::critical(this, "Error", "Failed to save data to file: unknown error");
            }
            
            // Refresh model and update view (only if save was successful or at least attempted)
            try {
                if (productModel && inventoryManager) {
                    productModel->refresh();
                }
                if (tableView) {
                    tableView->viewport()->update();
                    tableView->update();
                    tableView->resizeColumnsToContents();
                    // Force UI update
                    QApplication::processEvents();
                }
                // Update write-offs report if it is currently available
                updateWriteOffsReport();
            } catch (const std::exception& e) {
                qDebug() << "Error refreshing model:" << e.what();
                // Continue - model refresh error is not critical, but try to show success message
            } catch (...) {
                qDebug() << "Unknown error refreshing model";
                // Continue - model refresh error is not critical
            }
            
            // Show success message
            try {
                QMessageBox::information(this, "Success", 
                    QString("Product '%1' written off successfully!\nQuantity reduced by %2.")
                    .arg(productName).arg(quantity));
            } catch (...) {
                // If message box fails, at least the operation completed
                qDebug() << "Failed to show success message";
            }
        } catch (const ProductException& e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("An error occurred: %1").arg(e.what()));
        } catch (...) {
            QMessageBox::critical(this, "Error", "An unexpected error occurred while writing off the product.");
        }
    }
}

QWidget* MainWindow::createOrdersSection() {
    QWidget* sectionWidget = new QWidget(this);
    QVBoxLayout* sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setSpacing(15);
    sectionLayout->setContentsMargins(20, 20, 20, 20);
    
    // Section title
    QLabel* sectionTitle = new QLabel("ORDERS", sectionWidget);
    sectionTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "    color: #333;"
        "    padding: 10px 0;"
        "}"
    );
    sectionLayout->addWidget(sectionTitle);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* createOrderBtn = new QPushButton("Create Order", sectionWidget);
    createOrderBtn->setStyleSheet(getSuccessButtonStyle());
    connect(createOrderBtn, &QPushButton::clicked, this, &MainWindow::createOrder);
    
    QPushButton* viewHistoryBtn = new QPushButton("View Order History", sectionWidget);
    viewHistoryBtn->setStyleSheet(getPrimaryButtonStyle());
    connect(viewHistoryBtn, &QPushButton::clicked, this, [this]() {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString historyFilePath = dataPath + "/order_history.txt";
        QFile file(historyFilePath);
        if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            file.close();
            
            QDialog* historyDialog = new QDialog(this);
            historyDialog->setWindowTitle("Order History");
            historyDialog->setMinimumSize(800, 600);
            QVBoxLayout* layout = new QVBoxLayout(historyDialog);
            QTextEdit* textEdit = new QTextEdit(historyDialog);
            textEdit->setReadOnly(true);
            textEdit->setPlainText(content);
            layout->addWidget(textEdit);
            historyDialog->exec();
            delete historyDialog;
        } else {
            QMessageBox::information(this, "Info", "No order history found.");
        }
    });
    
    buttonLayout->addWidget(createOrderBtn);
    buttonLayout->addWidget(viewHistoryBtn);
    buttonLayout->addStretch();
    sectionLayout->addLayout(buttonLayout);
    
    // Orders table
    QTableWidget* ordersTable = new QTableWidget(sectionWidget);
    ordersTable->setColumnCount(7);
    ordersTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Company" << "Contact" << "Type" << "Date" << "Total" << "Actions");
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->verticalHeader()->setMaximumWidth(0);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ordersTable->setAlternatingRowColors(true);
    
    // Helper function to populate orders table
    auto populateOrdersTable = [this, ordersTable](QTableWidget* table) {
        table->setRowCount(0);
        auto orders = dbManager->getAllOrders();
        table->setRowCount(orders.size());
        for (size_t i = 0; i < orders.size(); ++i) {
            const auto& order = orders[i];
            table->setItem(i, 0, new QTableWidgetItem(QString::number(order.getId())));
            table->setItem(i, 1, new QTableWidgetItem(order.getCompanyName()));
            table->setItem(i, 2, new QTableWidgetItem(order.getContactPerson()));
            table->setItem(i, 3, new QTableWidgetItem(order.getOrderTypeString()));
            table->setItem(i, 4, new QTableWidgetItem(order.getOrderDate().toString("yyyy-MM-dd")));
            table->setItem(i, 5, new QTableWidgetItem(QString("$%1").arg(order.getTotalAmount(), 0, 'f', 2)));
            
            // Actions column
            QWidget* actionsWidget = new QWidget();
            QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
            actionsLayout->setContentsMargins(5, 2, 5, 2);
            actionsLayout->setSpacing(5);
            
            QPushButton* editBtn = new QPushButton("Edit", actionsWidget);
            QString editBtnStyle = getPrimaryButtonStyle();
            editBtnStyle.replace("padding: 10px 20px;", "padding: 5px 10px;");
            editBtnStyle.replace("font-size: 14px;", "font-size: 12px;");
            editBtnStyle.replace("border-radius: 5px;", "border-radius: 3px;");
            editBtn->setStyleSheet(editBtnStyle);
            
            QPushButton* deleteBtn = new QPushButton("Delete", actionsWidget);
            QString deleteBtnStyle = getDangerButtonStyle();
            deleteBtnStyle.replace("padding: 10px 20px;", "padding: 5px 10px;");
            deleteBtnStyle.replace("font-size: 14px;", "font-size: 12px;");
            deleteBtnStyle.replace("border-radius: 5px;", "border-radius: 3px;");
            deleteBtn->setStyleSheet(deleteBtnStyle);
            
            actionsLayout->addWidget(editBtn);
            actionsLayout->addWidget(deleteBtn);
            actionsLayout->addStretch();
            
            int orderId = order.getId();
            connect(editBtn, &QPushButton::clicked, this, [this, orderId]() {
                editOrder(orderId);
            });
            
            connect(deleteBtn, &QPushButton::clicked, this, [this, orderId]() {
                int ret = QMessageBox::question(this, "Confirm Delete",
                    QString("Are you sure you want to delete order #%1?").arg(orderId),
                    QMessageBox::Yes | QMessageBox::No);
                
                if (ret == QMessageBox::Yes) {
                    if (dbManager->deleteOrder(orderId)) {
                        QMessageBox::information(this, "Success", "Order deleted successfully!");
                        // Refresh orders section
                        QLayoutItem* child;
                        while ((child = contentWidget->layout()->takeAt(0)) != nullptr) {
                            delete child->widget();
                            delete child;
                        }
                        contentWidget->layout()->addWidget(createOrdersSection());
                    } else {
                        QMessageBox::warning(this, "Error", "Failed to delete order!");
                    }
                }
            });
            
            table->setCellWidget(i, 6, actionsWidget);
        }
    };
    
    // Load orders initially
    populateOrdersTable(ordersTable);
    
    // Configure table to stretch columns properly
    ordersTable->horizontalHeader()->setStretchLastSection(false);
    ordersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ordersTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents); // Actions column
    ordersTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    sectionLayout->addWidget(ordersTable);
    
    return sectionWidget;
}

QWidget* MainWindow::createInventorySection() {
    // Create inventory widget directly (similar to InventoryDialog content)
    QWidget* sectionWidget = new QWidget(this);
    QVBoxLayout* sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setSpacing(15);
    sectionLayout->setContentsMargins(20, 20, 20, 20);
    
    // Title - "Inventory Check" (Инвентаризация)
    QLabel* sectionTitle = new QLabel("Inventory Check", sectionWidget);
    sectionTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 20pt;"
        "    font-weight: bold;"
        "    color: #333;"
        "    padding: 10px 0;"
        "}"
    );
    sectionLayout->addWidget(sectionTitle);
    
    // Create table similar to InventoryDialog
    QTableView* inventoryTableView = new QTableView(sectionWidget);
    QStandardItemModel* inventoryModel = new QStandardItemModel(sectionWidget);
    inventoryModel->setColumnCount(6);
    inventoryModel->setHeaderData(0, Qt::Horizontal, "ID");
    inventoryModel->setHeaderData(1, Qt::Horizontal, "Name");
    inventoryModel->setHeaderData(2, Qt::Horizontal, "Category");
    inventoryModel->setHeaderData(3, Qt::Horizontal, "Current Quantity");
    inventoryModel->setHeaderData(4, Qt::Horizontal, "Actual Quantity");
    inventoryModel->setHeaderData(5, Qt::Horizontal, "Difference");
    
    auto products = inventoryManager->getAllProducts();
    inventoryModel->setRowCount(products.size());
    for (size_t i = 0; i < products.size(); ++i) {
        const auto& product = products[i];
        if (product) {
            inventoryModel->setItem(i, 0, new QStandardItem(QString::number(product->getId())));
            inventoryModel->setItem(i, 1, new QStandardItem(QString::fromStdString(product->getName())));
            inventoryModel->setItem(i, 2, new QStandardItem(QString::fromStdString(product->getCategory())));
            QStandardItem* currentQtyItem = new QStandardItem(QString::number(product->getQuantity()));
            currentQtyItem->setEditable(false);
            inventoryModel->setItem(i, 3, currentQtyItem);
            QStandardItem* actualItem = new QStandardItem(QString::number(product->getQuantity()));
            actualItem->setEditable(true);
            inventoryModel->setItem(i, 4, actualItem);
            QStandardItem* diffItem = new QStandardItem("0");
            diffItem->setEditable(false);
            inventoryModel->setItem(i, 5, diffItem);
        }
    }
    
    inventoryTableView->setModel(inventoryModel);
    inventoryTableView->setItemDelegateForColumn(4, new QStyledItemDelegate(sectionWidget));
    inventoryTableView->verticalHeader()->setVisible(false);
    inventoryTableView->verticalHeader()->setMaximumWidth(0);
    inventoryTableView->horizontalHeader()->setStretchLastSection(true);
    inventoryTableView->setAlternatingRowColors(true);
    
    // Connect dataChanged signal to update difference in real-time
    connect(inventoryModel, &QStandardItemModel::dataChanged, this, [inventoryModel](const QModelIndex& topLeft, const QModelIndex& bottomRight) {
        if (topLeft.column() == 4) { // Actual Quantity column
            for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
                QStandardItem* currentQtyItem = inventoryModel->item(row, 3);
                QStandardItem* actualQtyItem = inventoryModel->item(row, 4);
                QStandardItem* diffItem = inventoryModel->item(row, 5);
                
                if (currentQtyItem && actualQtyItem && diffItem) {
                    int currentQty = currentQtyItem->text().toInt();
                    int actualQty = actualQtyItem->text().toInt();
                    int difference = actualQty - currentQty;
                    diffItem->setText(QString::number(difference));
                    
                    // Color code the difference
                    if (difference > 0) {
                        diffItem->setForeground(QBrush(QColor("#4CAF50"))); // Green for positive
                    } else if (difference < 0) {
                        diffItem->setForeground(QBrush(QColor("#f44336"))); // Red for negative
                    } else {
                        diffItem->setForeground(QBrush(QColor("#333"))); // Black for zero
                    }
                }
            }
        }
    });
    
    sectionLayout->addWidget(inventoryTableView);
    
    // Buttons layout at the bottom
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    // Cancel button
    QPushButton* cancelBtn = new QPushButton("Cancel", sectionWidget);
    cancelBtn->setStyleSheet(getSecondaryButtonStyle());
    cancelBtn->setMinimumHeight(40);
    cancelBtn->setMinimumWidth(120);
    connect(cancelBtn, &QPushButton::clicked, this, [this, inventoryModel]() {
        // Reset actual quantities to current quantities
        if (inventoryManager) {
            auto products = inventoryManager->getAllProducts();
            for (size_t i = 0; i < products.size() && i < static_cast<size_t>(inventoryModel->rowCount()); ++i) {
                const auto& product = products[i];
                if (product) {
                    QStandardItem* actualItem = inventoryModel->item(static_cast<int>(i), 4);
                    if (actualItem) {
                        actualItem->setText(QString::number(product->getQuantity()));
                    }
                }
            }
        }
    });
    
    // Save Inventory button
    QPushButton* saveBtn = new QPushButton("Save Inventory", sectionWidget);
    saveBtn->setStyleSheet(getSuccessButtonStyle());
    saveBtn->setMinimumHeight(40);
    saveBtn->setMinimumWidth(150);
    connect(saveBtn, &QPushButton::clicked, this, [this, inventoryModel, inventoryTableView]() {
        if (!inventoryManager) {
            QMessageBox::warning(this, "Error", "Inventory manager is not available.");
            return;
        }
        
        InventoryAdjustmentService::Result result;
        
        // Save inventory changes
        for (int i = 0; i < inventoryModel->rowCount(); ++i) {
            int id = inventoryModel->item(i, 0)->text().toInt();
            int currentQty = inventoryModel->item(i, 3)->text().toInt();
            int actualQty = inventoryModel->item(i, 4)->text().toInt();

            try {
                InventoryAdjustmentService::applyAdjustment(
                    *inventoryManager,
                    dbManager,
                    id,
                    currentQty,
                    actualQty,
                    result
                );
            } catch (const ProductException& e) {
                QMessageBox::warning(this, "Error", 
                    QString("Failed to update product ID %1: %2")
                    .arg(id).arg(QString::fromStdString(e.what())));
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Error", 
                    QString("Failed to update product ID %1: %2")
                    .arg(id).arg(e.what()));
            }
        }
        
        if (result.itemsUpdated > 0) {
            // Save to file
            FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
            
            // Refresh product model
            if (productModel) {
                productModel->refresh();
            }
            
            QMessageBox::information(this, "Success", 
                QString("Inventory saved successfully!\n"
                       "Items updated: %1\n"
                       "Items added (units): %2\n"
                       "Items written off (units): %3")
                .arg(result.itemsUpdated)
                .arg(result.quantityAdded)
                .arg(result.quantityWrittenOff));
            
            // Refresh the inventory table to show updated current quantities
            auto products = inventoryManager->getAllProducts();
            for (size_t i = 0; i < products.size() && i < static_cast<size_t>(inventoryModel->rowCount()); ++i) {
                const auto& product = products[i];
                if (product) {
                    QStandardItem* currentQtyItem = inventoryModel->item(static_cast<int>(i), 3);
                    QStandardItem* actualQtyItem = inventoryModel->item(static_cast<int>(i), 4);
                    if (currentQtyItem) {
                        currentQtyItem->setText(QString::number(product->getQuantity()));
                    }
                    if (actualQtyItem) {
                        actualQtyItem->setText(QString::number(product->getQuantity()));
                    }
                }
            }
        } else {
            QMessageBox::information(this, "Info", "No changes to save.");
        }
    });
    
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(saveBtn);
    sectionLayout->addLayout(buttonLayout);
    
    return sectionWidget;
}

QWidget* MainWindow::createReportsSection() {
    QWidget* sectionWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(sectionWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Left sidebar for report types
    QWidget* reportsSidebar = new QWidget(sectionWidget);
    reportsSidebar->setFixedWidth(200);
    reportsSidebar->setStyleSheet(
        "QWidget {"
        "    background-color: #f9f9f9;"
        "    border-right: 1px solid #ddd;"
        "}"
    );
    QVBoxLayout* sidebarLayout = new QVBoxLayout(reportsSidebar);
    sidebarLayout->setSpacing(0);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    
    QPushButton* writeOffsBtn = new QPushButton("Write-offs Report", reportsSidebar);
    QPushButton* inventoryReportBtn = new QPushButton("Current Inventory", reportsSidebar);
    QPushButton* financialReportBtn = new QPushButton("Financial Report", reportsSidebar);
    
    QString buttonStyle = 
        "QPushButton {"
        "    text-align: left;"
        "    padding: 12px 20px;"
        "    border: none;"
        "    background-color: transparent;"
        "    color: #333;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e3f2fd;"
        "    color: #1976d2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2196F3;"
        "    color: white;"
        "}";
    
    writeOffsBtn->setStyleSheet(buttonStyle);
    inventoryReportBtn->setStyleSheet(buttonStyle);
    financialReportBtn->setStyleSheet(buttonStyle);
    
    sidebarLayout->addWidget(writeOffsBtn);
    sidebarLayout->addWidget(inventoryReportBtn);
    sidebarLayout->addWidget(financialReportBtn);
    sidebarLayout->addStretch();
    
    // Right content area
    QWidget* reportsContent = new QWidget(sectionWidget);
    QVBoxLayout* contentLayout = new QVBoxLayout(reportsContent);
    contentLayout->setSpacing(15);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    
    QStackedWidget* reportsStack = new QStackedWidget(reportsContent);
    
    // Write-offs report page
    QWidget* writeOffsPage = new QWidget();
    QVBoxLayout* writeOffsLayout = new QVBoxLayout(writeOffsPage);
    
    QHBoxLayout* writeOffsTitleLayout = new QHBoxLayout();
    QLabel* writeOffsTitle = new QLabel("Write-offs Report", writeOffsPage);
    writeOffsTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "    color: #333;"
        "    padding: 10px 0;"
        "}"
    );
    QPushButton* exportWriteOffsBtn = new QPushButton("Export to TXT", writeOffsPage);
    exportWriteOffsBtn->setStyleSheet(getSuccessButtonStyle());
    
    writeOffsTitleLayout->addWidget(writeOffsTitle);
    writeOffsTitleLayout->addStretch();
    writeOffsTitleLayout->addWidget(exportWriteOffsBtn);
    writeOffsLayout->addLayout(writeOffsTitleLayout);
    
    writeOffsReportTextEdit = new QTextEdit(writeOffsPage);
    writeOffsReportTextEdit->setReadOnly(true);
    writeOffsReportTextEdit->setFont(QFont("Courier", 10));
    
    // Generate initial report
    QString writeOffsText = updateWriteOffsReport();
    
    connect(exportWriteOffsBtn, &QPushButton::clicked, this, [this]() {
        QString writeOffsText = updateWriteOffsReport();
        QString fileName = QFileDialog::getSaveFileName(this, "Export Write-offs Report", "", "Text Files (*.txt);;All Files (*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << writeOffsText;
                file.close();
                QMessageBox::information(this, "Success", "Report exported successfully!");
            } else {
                QMessageBox::warning(this, "Error", "Failed to export report!");
            }
        }
    });
    
    writeOffsLayout->addWidget(writeOffsReportTextEdit);
    reportsStack->addWidget(writeOffsPage);
    
    // Current inventory report page
    QWidget* inventoryReportPage = new QWidget();
    QVBoxLayout* inventoryReportLayout = new QVBoxLayout(inventoryReportPage);
    
    QHBoxLayout* inventoryTitleLayout = new QHBoxLayout();
    QLabel* inventoryTitle = new QLabel("Current Inventory Report", inventoryReportPage);
    inventoryTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "    color: #333;"
        "    padding: 10px 0;"
        "}"
    );
    QPushButton* exportInventoryBtn = new QPushButton("Export to TXT", inventoryReportPage);
    exportInventoryBtn->setStyleSheet(getSuccessButtonStyle());
    
    inventoryTitleLayout->addWidget(inventoryTitle);
    inventoryTitleLayout->addStretch();
    inventoryTitleLayout->addWidget(exportInventoryBtn);
    inventoryReportLayout->addLayout(inventoryTitleLayout);
    
    QTextEdit* inventoryTextEdit = new QTextEdit(inventoryReportPage);
    inventoryTextEdit->setReadOnly(true);
    inventoryTextEdit->setFont(QFont("Courier", 10));
    
    // Generate inventory report text
    QString inventoryText = "=== CURRENT INVENTORY REPORT ===\n\n";
    auto products = inventoryManager->getAllProducts();
    inventoryText += QString("Total Products: %1\n\n").arg(products.size());
    
    double totalInventoryValue = 0.0;
    for (const auto& productPtr : products) {
        if (productPtr) {
            inventoryText += QString("ID: %1\n").arg(productPtr->getId());
            inventoryText += QString("Name: %1\n").arg(QString::fromStdString(productPtr->getName()));
            inventoryText += QString("Category: %1\n").arg(QString::fromStdString(productPtr->getCategory()));
            inventoryText += QString("Quantity: %1\n").arg(productPtr->getQuantity());
            inventoryText += QString("Unit Price: $%1\n").arg(productPtr->getUnitPrice(), 0, 'f', 2);
            inventoryText += QString("Total Value: $%1\n").arg(productPtr->calculateTotalValue(), 0, 'f', 2);
            inventoryText += "---\n\n";
            totalInventoryValue += productPtr->calculateTotalValue();
        }
    }
    inventoryText += QString("Total Inventory Value: $%1\n").arg(totalInventoryValue, 0, 'f', 2);
    
    inventoryTextEdit->setPlainText(inventoryText);
    
    connect(exportInventoryBtn, &QPushButton::clicked, this, [inventoryText, this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Export Inventory Report", "", "Text Files (*.txt);;All Files (*)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << inventoryText;
                file.close();
                QMessageBox::information(this, "Success", "Report exported successfully!");
            } else {
                QMessageBox::warning(this, "Error", "Failed to export report!");
            }
        }
    });
    
    inventoryReportLayout->addWidget(inventoryTextEdit);
    reportsStack->addWidget(inventoryReportPage);
    
    // Financial report page with charts
    QWidget* financialReportPage = new QWidget();
    QVBoxLayout* financialLayout = new QVBoxLayout(financialReportPage);
    financialLayout->setSpacing(25);
    financialLayout->setContentsMargins(20, 20, 20, 20);
    
    // Header section with total sales
    QWidget* headerWidget = new QWidget(financialReportPage);
    headerWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f5f5;"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "}"
    );
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 10, 10, 10);
    
    QLabel* salesTitleLabel = new QLabel("Total Sales:", headerWidget);
    salesTitleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 16pt;"
        "    font-weight: normal;"
        "    color: #666;"
        "}"
    );
    
    QLabel* salesLabel = new QLabel("$" + QString::number(calculateTotalSales(), 'f', 2));
    salesLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 24pt;"
        "    font-weight: bold;"
        "    color: #2196F3;"
        "}"
    );
    
    headerLayout->addWidget(salesTitleLabel);
    headerLayout->addWidget(salesLabel);
    headerLayout->addStretch();
    financialLayout->addWidget(headerWidget);
    
    // Charts section - horizontal layout for two charts side by side
    QHBoxLayout* chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(20);
    
    // Category pie chart (left side)
    QWidget* categoryChartWidget = new QWidget(financialReportPage);
    categoryChartWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "    border: 1px solid #e0e0e0;"
        "}"
    );
    QVBoxLayout* categoryChartLayout = new QVBoxLayout(categoryChartWidget);
    categoryChartLayout->setContentsMargins(15, 15, 15, 15);
    categoryChartLayout->setSpacing(10);
    
    QLabel* categoryChartTitle = new QLabel("Category Distribution", categoryChartWidget);
    categoryChartTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "    color: #333;"
        "    padding: 5px 0;"
        "}"
    );
    categoryChartLayout->addWidget(categoryChartTitle);
    
    QMap<QString, double> categoryData = getCategorySalesData();
    if (!categoryData.isEmpty()) {
        QPieSeries* categorySeries = new QPieSeries();
        
        // Color palette for pie slices
        QList<QColor> colors = {
            QColor("#2196F3"), QColor("#4CAF50"), QColor("#FF9800"), 
            QColor("#F44336"), QColor("#9C27B0"), QColor("#00BCD4"),
            QColor("#8BC34A"), QColor("#FFC107"), QColor("#E91E63")
        };
        
        int colorIndex = 0;
        for (auto it = categoryData.begin(); it != categoryData.end(); ++it) {
            QPieSlice* slice = categorySeries->append(it.key(), it.value());
            slice->setColor(colors[colorIndex % colors.size()]);
            slice->setLabelVisible(true);
            slice->setLabel(QString("%1\n$%2").arg(it.key()).arg(it.value(), 0, 'f', 2));
            slice->setLabelPosition(QPieSlice::LabelOutside);
            colorIndex++;
        }
        
        QChart* categoryChart = new QChart();
        categoryChart->addSeries(categorySeries);
        categoryChart->setTitle("");
        categoryChart->legend()->setAlignment(Qt::AlignRight);
        categoryChart->legend()->setFont(QFont("Arial", 10));
        categoryChart->setAnimationOptions(QChart::SeriesAnimations);
        categoryChart->setBackgroundBrush(QBrush(QColor("#ffffff")));
        
        QChartView* categoryChartView = new QChartView(categoryChart);
        categoryChartView->setRenderHint(QPainter::Antialiasing);
        categoryChartView->setMinimumHeight(450);
        categoryChartView->setMinimumWidth(500);
        categoryChartLayout->addWidget(categoryChartView);
    } else {
        QLabel* noCategoryDataLabel = new QLabel("No category data available.\nCreate orders with products to see the chart.", categoryChartWidget);
        noCategoryDataLabel->setAlignment(Qt::AlignCenter);
        noCategoryDataLabel->setStyleSheet(
            "QLabel {"
            "    font-size: 14px;"
            "    color: #999;"
            "    padding: 40px;"
            "}"
        );
        noCategoryDataLabel->setMinimumHeight(450);
        categoryChartLayout->addWidget(noCategoryDataLabel);
    }
    
    chartsLayout->addWidget(categoryChartWidget);
    
    // Top 5 companies bar chart (right side)
    QWidget* companiesChartWidget = new QWidget(financialReportPage);
    companiesChartWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "    border: 1px solid #e0e0e0;"
        "}"
    );
    QVBoxLayout* companiesChartLayout = new QVBoxLayout(companiesChartWidget);
    companiesChartLayout->setContentsMargins(15, 15, 15, 15);
    companiesChartLayout->setSpacing(10);
    
    QLabel* topCompaniesTitle = new QLabel("Top 5 Companies by Purchases", companiesChartWidget);
    topCompaniesTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 16pt;"
        "    font-weight: bold;"
        "    color: #333;"
        "    padding: 5px 0;"
        "}"
    );
    companiesChartLayout->addWidget(topCompaniesTitle);
    
    QList<QPair<QString, double>> topCompanies = getTopCompaniesData(5);
    if (!topCompanies.isEmpty()) {
        QBarSet* barSet = new QBarSet("Sales");
        QStringList categories;
        
        // Find max value for better axis scaling
        double maxValue = 0.0;
        for (const auto& company : topCompanies) {
            *barSet << company.second;
            categories << company.first;
            if (company.second > maxValue) {
                maxValue = company.second;
            }
        }
        
        QBarSeries* barSeries = new QBarSeries();
        barSeries->append(barSet);
        barSeries->setLabelsVisible(true);
        barSeries->setLabelsFormat("$@value");
        barSeries->setLabelsPosition(QAbstractBarSeries::LabelsCenter);
        
        // Set color for all bars
        barSet->setColor(QColor("#2196F3"));
        
        QChart* companiesChart = new QChart();
        companiesChart->addSeries(barSeries);
        companiesChart->setTitle("");
        companiesChart->setAnimationOptions(QChart::SeriesAnimations);
        companiesChart->setBackgroundBrush(QBrush(QColor("#ffffff")));
        
        QBarCategoryAxis* axisX = new QBarCategoryAxis();
        axisX->append(categories);
        axisX->setLabelsFont(QFont("Arial", 10));
        companiesChart->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);
        
        QValueAxis* axisY = new QValueAxis();
        axisY->setTitleText("Amount ($)");
        axisY->setTitleFont(QFont("Arial", 11, QFont::Bold));
        axisY->setLabelsFont(QFont("Arial", 10));
        axisY->setMin(0);
        axisY->setMax(maxValue * 1.1); // Add 10% padding at top
        axisY->applyNiceNumbers();
        companiesChart->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);
        
        companiesChart->legend()->setVisible(false);
        
        QChartView* companiesChartView = new QChartView(companiesChart);
        companiesChartView->setRenderHint(QPainter::Antialiasing);
        companiesChartView->setMinimumHeight(450);
        companiesChartView->setMinimumWidth(500);
        companiesChartLayout->addWidget(companiesChartView);
    } else {
        QLabel* noCompaniesDataLabel = new QLabel("No company data available.\nCreate orders to see the chart.", companiesChartWidget);
        noCompaniesDataLabel->setAlignment(Qt::AlignCenter);
        noCompaniesDataLabel->setStyleSheet(
            "QLabel {"
            "    font-size: 14px;"
            "    color: #999;"
            "    padding: 40px;"
            "}"
        );
        noCompaniesDataLabel->setMinimumHeight(450);
        companiesChartLayout->addWidget(noCompaniesDataLabel);
    }
    
    chartsLayout->addWidget(companiesChartWidget);
    financialLayout->addLayout(chartsLayout);
    
    financialLayout->addStretch();
    
    reportsStack->addWidget(financialReportPage);
    
    contentLayout->addWidget(reportsStack);
    
    // Connect buttons to switch pages and refresh report
    connect(writeOffsBtn, &QPushButton::clicked, this, [this, reportsStack]() { 
        reportsStack->setCurrentIndex(0);
        // Refresh report when switching to write-offs page
        updateWriteOffsReport();
    });
    connect(inventoryReportBtn, &QPushButton::clicked, [reportsStack]() { reportsStack->setCurrentIndex(1); });
    connect(financialReportBtn, &QPushButton::clicked, [reportsStack]() { reportsStack->setCurrentIndex(2); });
    
    mainLayout->addWidget(reportsSidebar);
    mainLayout->addWidget(reportsContent, 1);
    
    return sectionWidget;
}

double MainWindow::calculateTotalSales() {
    auto orders = dbManager->getAllOrders();
    double total = 0.0;
    for (const auto& order : orders) {
        total += order.getTotalAmount();
    }
    return total;
}

QMap<QString, double> MainWindow::getCategorySalesData() {
    QMap<QString, double> categorySales;
    auto orders = dbManager->getAllOrders();
    
    for (const auto& order : orders) {
        const auto& items = order.getItems();
        for (const auto& item : items) {
            QString category = item.category;
            if (category.isEmpty()) {
                category = "Unknown";
            }
            categorySales[category] += item.totalPrice;
        }
    }
    
    return categorySales;
}

QList<QPair<QString, double>> MainWindow::getTopCompaniesData(int topCount) {
    QMap<QString, double> companySales;
    auto orders = dbManager->getAllOrders();
    
    for (const auto& order : orders) {
        QString companyName = order.getCompanyName();
        if (companyName.isEmpty()) {
            companyName = "Unknown";
        }
        companySales[companyName] += order.getTotalAmount();
    }
    
    // Convert to list and sort by sales amount
    QList<QPair<QString, double>> companyList;
    for (auto it = companySales.begin(); it != companySales.end(); ++it) {
        companyList.append(qMakePair(it.key(), it.value()));
    }
    
    // Sort by sales amount (descending)
    std::sort(companyList.begin(), companyList.end(), 
        [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
            return a.second > b.second;
        });
    
    // Return top N companies
    if (companyList.size() > topCount) {
        companyList = companyList.mid(0, topCount);
    }
    
    return companyList;
}

QString MainWindow::getPrimaryButtonStyle() const {
    return 
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0b7dda;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0a6bc2;"
        "}";
}

QString MainWindow::getSecondaryButtonStyle() const {
    return 
        "QPushButton {"
        "    background-color: #757575;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #616161;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #424242;"
        "}";
}

QString MainWindow::getDangerButtonStyle() const {
    return 
        "QPushButton {"
        "    background-color: #f44336;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #da190b;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #c62828;"
        "}";
}

QString MainWindow::getSuccessButtonStyle() const {
    return 
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #388e3c;"
        "}";
}


