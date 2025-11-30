#include "ui/dialogs/SalesReportDialog.h"
#include "managers/DatabaseManager.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QTextStream>
#include <QDate>
#include <QGroupBox>
#include <QFormLayout>

SalesReportDialog::SalesReportDialog(QWidget* parent)
    : QDialog(parent), dbManager(DatabaseManager::getInstance()) {
    setupUI();
    generateReport();
}

void SalesReportDialog::setupUI() {
    setWindowTitle("Sales Reports");
    setMinimumSize(1000, 700);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Filters
    setupFilters();
    mainLayout->addWidget(createFiltersGroupBox());
    
    // Report table
    setupReportTable();
    mainLayout->addWidget(createReportTableGroupBox());
    
    // Statistics
    setupStatistics();
    mainLayout->addWidget(createStatisticsGroupBox());
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    exportButton = new QPushButton("Export to File", this);
    QPushButton* closeButton = new QPushButton("Close", this);
    
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
    
    connect(exportButton, &QPushButton::clicked, this, &SalesReportDialog::exportReport);
    connect(closeButton, &QPushButton::clicked, this, &SalesReportDialog::accept);
    connect(reportTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SalesReportDialog::onReportTypeChanged);
    connect(startDateEdit, &QDateEdit::dateChanged, this, &SalesReportDialog::generateReport);
    connect(endDateEdit, &QDateEdit::dateChanged, this, &SalesReportDialog::generateReport);
}

QGroupBox* SalesReportDialog::createFiltersGroupBox() {
    QGroupBox* groupBox = new QGroupBox("Filters", this);
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    
    layout->addWidget(new QLabel("Report Type:", this));
    reportTypeComboBox = new QComboBox(this);
    reportTypeComboBox->addItem("All Orders");
    reportTypeComboBox->addItem("Retail Sales");
    reportTypeComboBox->addItem("Wholesale Sales");
    reportTypeComboBox->addItem("By Company");
    reportTypeComboBox->addItem("By Date");
    layout->addWidget(reportTypeComboBox);
    
    layout->addWidget(new QLabel("From:", this));
    startDateEdit = new QDateEdit(this);
    startDateEdit->setDate(QDate::currentDate().addDays(-30));
    startDateEdit->setCalendarPopup(true);
    layout->addWidget(startDateEdit);
    
    layout->addWidget(new QLabel("To:", this));
    endDateEdit = new QDateEdit(this);
    endDateEdit->setDate(QDate::currentDate());
    endDateEdit->setCalendarPopup(true);
    layout->addWidget(endDateEdit);
    
    layout->addStretch();
    
    return groupBox;
}

QGroupBox* SalesReportDialog::createReportTableGroupBox() {
    QGroupBox* groupBox = new QGroupBox("Orders", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    
    reportTable = new QTableWidget(0, 7, this);
    reportTable->setHorizontalHeaderLabels({"ID", "Company", "Contact", "Type", "Date", "Items", "Amount"});
    reportTable->verticalHeader()->setVisible(false);
    reportTable->verticalHeader()->setMaximumWidth(0);
    reportTable->horizontalHeader()->setStretchLastSection(true);
    reportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reportTable->setAlternatingRowColors(true);
    
    layout->addWidget(reportTable);
    
    return groupBox;
}

QGroupBox* SalesReportDialog::createStatisticsGroupBox() {
    QGroupBox* groupBox = new QGroupBox("Statistics", this);
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    
    totalRetailLabel = new QLabel("Retail: $0.00", this);
    totalRetailLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(totalRetailLabel);
    
    totalWholesaleLabel = new QLabel("Wholesale: $0.00", this);
    totalWholesaleLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(totalWholesaleLabel);
    
    totalAmountLabel = new QLabel("Total: $0.00", this);
    totalAmountLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    layout->addWidget(totalAmountLabel);
    
    totalDiscountLabel = new QLabel("Discounts: $0.00", this);
    totalDiscountLabel->setStyleSheet("font-weight: bold; color: green;");
    layout->addWidget(totalDiscountLabel);
    
    ordersCountLabel = new QLabel("Orders: 0", this);
    ordersCountLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(ordersCountLabel);
    
    layout->addStretch();
    
    return groupBox;
}

void SalesReportDialog::setupFilters() {
    // Создано в createFiltersGroupBox
}

void SalesReportDialog::setupReportTable() {
    // Создано в createReportTableGroupBox
}

void SalesReportDialog::setupStatistics() {
    // Создано в createStatisticsGroupBox
}

void SalesReportDialog::onReportTypeChanged(int index) {
    Q_UNUSED(index);
    generateReport();
}

void SalesReportDialog::generateReport() {
    int reportType = reportTypeComboBox->currentIndex();
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();
    
    currentOrders.clear();
    
    switch (reportType) {
        case 0: // All orders
            currentOrders = dbManager->getOrdersByDateRange(startDate, endDate);
            break;
        case 1: // Retail sales
            currentOrders = dbManager->getOrdersByType(OrderType::RETAIL);
            break;
        case 2: // Wholesale sales
            currentOrders = dbManager->getOrdersByType(OrderType::WHOLESALE);
            break;
        case 3: // By company (show all for now, company filtering will be added later)
            currentOrders = dbManager->getAllOrders();
            break;
        case 4: // By date
            currentOrders = dbManager->getOrdersByDateRange(startDate, endDate);
            break;
        default:
            currentOrders = dbManager->getAllOrders();
            break;
    }
    
    refreshReport();
    calculateStatistics();
}

void SalesReportDialog::refreshReport() {
    reportTable->setRowCount(0);
    
    for (const auto& order : currentOrders) {
        int row = reportTable->rowCount();
        reportTable->insertRow(row);
        
        reportTable->setItem(row, 0, new QTableWidgetItem(QString::number(order.getId())));
        reportTable->setItem(row, 1, new QTableWidgetItem(order.getCompanyName()));
        reportTable->setItem(row, 2, new QTableWidgetItem(order.getContactPerson()));
        reportTable->setItem(row, 3, new QTableWidgetItem(order.getOrderTypeString()));
        reportTable->setItem(row, 4, new QTableWidgetItem(order.getOrderDate().toString("MM/dd/yyyy")));
        reportTable->setItem(row, 5, new QTableWidgetItem(QString::number(order.getItems().size())));
        reportTable->setItem(row, 6, new QTableWidgetItem(QString("$%1").arg(order.getTotalAmount(), 0, 'f', 2)));
    }
    
    reportTable->resizeColumnsToContents();
}

void SalesReportDialog::calculateStatistics() {
    double totalRetail = 0.0;
    double totalWholesale = 0.0;
    double totalAmount = 0.0;
    double totalDiscount = 0.0;
    int retailCount = 0;
    int wholesaleCount = 0;
    
    for (const auto& order : currentOrders) {
        if (order.getOrderType() == OrderType::RETAIL) {
            totalRetail += order.getTotalAmount();
            retailCount++;
        } else {
            totalWholesale += order.getTotalAmount();
            wholesaleCount++;
        }
        totalAmount += order.getTotalAmount();
        totalDiscount += order.getTotalDiscount();
    }
    
    totalRetailLabel->setText(QString("Retail: $%1 (%2 orders)").arg(totalRetail, 0, 'f', 2).arg(retailCount));
    totalWholesaleLabel->setText(QString("Wholesale: $%1 (%2 orders)").arg(totalWholesale, 0, 'f', 2).arg(wholesaleCount));
    totalAmountLabel->setText(QString("Total: $%1").arg(totalAmount, 0, 'f', 2));
    totalDiscountLabel->setText(QString("Discounts: $%1").arg(totalDiscount, 0, 'f', 2));
    ordersCountLabel->setText(QString("Orders: %1").arg(currentOrders.size()));
}

void SalesReportDialog::exportReport() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Report", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Failed to open file for writing!");
        return;
    }
    
    QTextStream out(&file);
    
    out << "=== SALES REPORT ===\n\n";
    out << QString("Period: %1 - %2\n").arg(startDateEdit->date().toString("MM/dd/yyyy"))
                                        .arg(endDateEdit->date().toString("MM/dd/yyyy"));
    out << QString("Total Orders: %1\n\n").arg(currentOrders.size());
    
    out << "=== STATISTICS ===\n";
    out << totalRetailLabel->text() << "\n";
    out << totalWholesaleLabel->text() << "\n";
    out << totalAmountLabel->text() << "\n";
    out << totalDiscountLabel->text() << "\n\n";
    
    out << "=== ORDER DETAILS ===\n\n";
    for (const auto& order : currentOrders) {
        out << QString("Order #%1\n").arg(order.getId());
        out << QString("Company: %1\n").arg(order.getCompanyName());
        out << QString("Contact: %1\n").arg(order.getContactPerson());
        out << QString("Type: %1\n").arg(order.getOrderTypeString());
        out << QString("Date: %1\n").arg(order.getOrderDate().toString("MM/dd/yyyy"));
        out << "Products:\n";
        
        for (const auto& item : order.getItems()) {
            out << QString("  - %1 x%2 @ $%3 (discount %4%) = $%5\n")
                .arg(item.productName)
                .arg(item.quantity)
                .arg(item.unitPrice, 0, 'f', 2)
                .arg(item.discountPercent, 0, 'f', 1)
                .arg(item.totalPrice, 0, 'f', 2);
        }
        
        out << QString("Total: $%1\n").arg(order.getTotalAmount(), 0, 'f', 2);
        out << "---\n\n";
    }
    
    file.close();
    QMessageBox::information(this, "Success", "Report exported successfully!");
}

