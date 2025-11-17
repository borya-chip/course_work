#include "MainWindow.h"
#include "ProductDialog.h"
#include "WriteOffDialog.h"
#include "ReportDialog.h"
#include "FileManager.h"
#include "Exceptions.h"
#include "ActionsDelegate.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), inventoryManager(new InventoryManager()) {
    
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }
    dataFilePath = dataPath + "/inventory.dat";
    
    if (QFile::exists(dataFilePath)) {
        FileManager::loadFromBinary(*inventoryManager, dataFilePath.toStdString());
    }
    
    setupUI();
    connectSignals();
    productModel->refresh();
}

MainWindow::~MainWindow() {
    FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
    delete inventoryManager;
}

void MainWindow::setupUI() {
    setWindowTitle("Product Calculation and Write-off System");
    setMinimumSize(1000, 600);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    setupSearchBar();
    setupButtons();
    mainLayout->addWidget(createTopBar());
    
    setupTable();
    mainLayout->addWidget(tableView);
    
}

QWidget* MainWindow::createTopBar() {
    QWidget* topBar = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(topBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    
    QPushButton* inventoryButton = new QPushButton("Inventory", this);
    inventoryButton->setMinimumHeight(35);
    inventoryButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0b7dda;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0a6bc2;"
        "}"
    );
    layout->addWidget(inventoryButton);
    connect(inventoryButton, &QPushButton::clicked, this, &MainWindow::openInventory);
    
    addButton->setMinimumHeight(35);
    addButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0b7dda;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0a6bc2;"
        "}"
    );
    layout->addWidget(addButton);
    
    layout->addStretch();
    
    QLabel* searchLabel = new QLabel("Search:", this);
    searchLabel->setStyleSheet("font-weight: bold; color: #333;");
    layout->addWidget(searchLabel);
    
    searchLineEdit->setMinimumHeight(35);
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
    layout->addWidget(searchLineEdit);
    
    QLabel* categoryLabel = new QLabel("Category:", this);
    categoryLabel->setStyleSheet("font-weight: bold; color: #333;");
    layout->addWidget(categoryLabel);
    
    categoryComboBox->setMinimumHeight(35);
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
        "QComboBox::drop-down {"
        "    border: none;"
        "    width: 0px;"
        "}"
        "QComboBox::down-arrow {"
        "    image: none;"
        "    width: 0px;"
        "    height: 0px;"
        "}"
    );
    layout->addWidget(categoryComboBox);
    
    searchButton->setMinimumHeight(35);
    searchButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0b7dda;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0a6bc2;"
        "}"
    );
    layout->addWidget(searchButton);
    
    return topBar;
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
    connect(actionsDelegate, &ActionsDelegate::reportRequested, this, &MainWindow::reportProductByRow);
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
    
    searchButton = new QPushButton("Search", this);
}

void MainWindow::setupButtons() {
    addButton = new QPushButton("Add Product", this);
}

void MainWindow::connectSignals() {
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addProduct);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchProducts);
    connect(categoryComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(filterByCategory()));
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
            
            auto existingProduct = inventoryManager->getProduct(product.getId());
            if (existingProduct) {
                QMessageBox::warning(this, "Error", 
                    QString("Product with ID %1 already exists!").arg(product.getId()));
                return;
            }
            
            auto productPtr = std::make_shared<Product>(product);
            inventoryManager->addProduct(productPtr);
            
            productModel->refresh();
            
            FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
            
            QMessageBox::information(this, "Success", "Product added successfully!");
        } catch (const ProductException& e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
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
    QString searchText = searchLineEdit->text().trimmed();
    if (searchText.isEmpty()) {
        productModel->refresh();
        return;
    }
    
    std::vector<std::shared_ptr<Product>> products = 
        inventoryManager->searchProducts(searchText.toStdString());
    std::vector<Product> productList;
    for (const auto& p : products) {
        if (p) {
            productList.push_back(*p);
        }
    }
    productModel->setProducts(productList);
}

void MainWindow::filterByCategory() {
    QString category = categoryComboBox->currentText();
    if (category == "All Categories") {
        inventoryManager->sortProductsByCategory();
        productModel->refresh();
        return;
    }
    
    std::vector<std::shared_ptr<Product>> products = 
        inventoryManager->filterByCategory(category.toStdString());
    std::vector<Product> productList;
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
    
    Product product = productModel->getProduct(row);
    
    WriteOffDialog dialog(product, this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            int quantity = dialog.getWriteOffQuantity();
            
            inventoryManager->writeOffProduct(product.getId(), quantity, "");
            
            productModel->refresh();
            
            FileManager::saveToBinary(*inventoryManager, dataFilePath.toStdString());
            
            QMessageBox::information(this, "Success", "Product written off successfully!");
        } catch (const ProductException& e) {
            QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
        }
    }
}

void MainWindow::reportProductByRow(int row) {
    if (row < 0 || row >= productModel->rowCount()) {
        return;
    }
    
    Product product = productModel->getProduct(row);
    generateReport();
}

