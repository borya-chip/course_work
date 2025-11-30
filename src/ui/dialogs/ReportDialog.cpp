#include "ui/dialogs/ReportDialog.h"
#include "managers/FileManager.h"
#include "services/WriteOffCalculator.h"
#include "managers/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

ReportDialog::ReportDialog(const InventoryService& inventory, QWidget* parent)
    : QDialog(parent), inventoryManager(inventory) {
    setupUI();
    generateReport();
}

void ReportDialog::setupUI() {
    setWindowTitle("Generate Report");
    setMinimumSize(700, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Report Type:", this));
    reportTypeComboBox = new QComboBox(this);
    reportTypeComboBox->addItems({"Inventory Report", "Write-off History"});
    typeLayout->addWidget(reportTypeComboBox);
    typeLayout->addStretch();
    
    mainLayout->addLayout(typeLayout);
    
    reportTextEdit = new QTextEdit(this);
    reportTextEdit->setReadOnly(true);
    reportTextEdit->setFont(QFont("Courier", 10));
    mainLayout->addWidget(reportTextEdit);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    generateButton = new QPushButton("Generate Report", this);
    exportButton = new QPushButton("Export to File", this);
    closeButton = new QPushButton("Close", this);
    
    buttonLayout->addWidget(generateButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
    
    connect(generateButton, &QPushButton::clicked, this, &ReportDialog::generateReport);
    connect(exportButton, &QPushButton::clicked, this, &ReportDialog::exportReport);
    connect(closeButton, &QPushButton::clicked, this, &ReportDialog::accept);
    connect(reportTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportDialog::generateReport);
}

void ReportDialog::generateReport() {
    int reportType = reportTypeComboBox->currentIndex();
    
    if (reportType == 0) {
        generateInventoryReport();
    } else {
        generateWriteOffReport();
    }
}

void ReportDialog::generateInventoryReport() {
    QString report;
    report += "=== INVENTORY REPORT ===\n\n";
    
    report += QString("Total Products: %1\n").arg(inventoryManager.getTotalProductCount());
    report += QString("Total Inventory Value: $%1\n")
              .arg(QString::number(inventoryManager.calculateTotalInventoryValue(), 'f', 2));
    report += QString("Total Quantity: %1\n\n").arg(inventoryManager.getTotalQuantity());
    
    report += "=== PRODUCT LIST ===\n\n";
    auto products = inventoryManager.getAllProducts();
    for (const auto& product : products) {
        if (product) {
            report += QString("ID: %1\n").arg(product->getId());
            report += QString("Name: %1\n").arg(QString::fromStdString(product->getName()));
            report += QString("Category: %1\n").arg(QString::fromStdString(product->getCategory()));
            report += QString("Quantity: %1\n").arg(product->getQuantity());
            report += QString("Unit Price: $%1\n")
                      .arg(QString::number(product->getUnitPrice(), 'f', 2));
            report += QString("Total Value: $%1\n")
                      .arg(QString::number(product->calculateTotalValue(), 'f', 2));
            report += "---\n\n";
        }
    }
    
    reportTextEdit->setPlainText(report);
}

void ReportDialog::generateWriteOffReport() {
    QString report;
    report += "=== WRITE-OFF HISTORY REPORT ===\n\n";
    
    // Use DatabaseManager to get write-off history from database
    auto dbManager = DatabaseManager::getInstance();
    auto writeOffHistory = dbManager->getWriteOffHistory();
    report += QString("Total Write-offs: %1\n\n").arg(writeOffHistory.size());
    
    double totalValue = 0.0;
    for (int i = 0; i < writeOffHistory.size(); ++i) {
        const auto& record = writeOffHistory[i];
        if (record.size() >= 5) {
            report += QString("Record #%1\n").arg(record[0]); // ID
            report += QString("Product: %1\n").arg(record[1]); // Product Name
            report += QString("Quantity: %1\n").arg(record[2]); // Quantity
            report += QString("Value: $%1\n").arg(record[3]); // Value
            report += "---\n\n";
            totalValue += record[3].toDouble();
        }
    }
    
    report += QString("Total Write-off Value: $%1\n")
              .arg(QString::number(totalValue, 'f', 2));
    
    reportTextEdit->setPlainText(report);
}

void ReportDialog::exportReport() {
    QString filename = QFileDialog::getSaveFileName(this, "Export Report",
                                                    "", "Text Files (*.txt)");
    if (!filename.isEmpty()) {
        int reportType = reportTypeComboBox->currentIndex();
        bool success = false;
        
        if (reportType == 0) {
            success = FileManager::exportReportToText(inventoryManager, filename.toStdString());
        } else {
            success = FileManager::exportWriteOffHistoryToText(inventoryManager, filename.toStdString());
        }
        
        if (success) {
            QMessageBox::information(this, "Success", "Report exported successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to export report.");
        }
    }
}

