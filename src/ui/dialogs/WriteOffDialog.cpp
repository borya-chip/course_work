#include "ui/dialogs/WriteOffDialog.h"
#include "exceptions/Exceptions.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDialogButtonBox>

WriteOffDialog::WriteOffDialog(const Product& product, QWidget* parent)
    : QDialog(parent), product(product) {
    setupUI();
}

void WriteOffDialog::setupUI() {
    setWindowTitle("Write-off Product");
    setMinimumWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();
    
    productNameLabel = new QLabel(QString::fromStdString(product.getName()), this);
    productNameLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    formLayout->addRow("Product:", productNameLabel);
    
    currentQuantityLabel = new QLabel(QString::number(product.getQuantity()), this);
    formLayout->addRow("Current Quantity:", currentQuantityLabel);
    
    totalValueLabel = new QLabel(QString("$%1").arg(
        QString::number(product.calculateTotalValue(), 'f', 2)), this);
    formLayout->addRow("Total Value:", totalValueLabel);
    
    quantitySpinBox = new QSpinBox(this);
    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setMaximum(product.getQuantity());
    quantitySpinBox->setValue(1);
    formLayout->addRow("Write-off Quantity:", quantitySpinBox);
    
    mainLayout->addLayout(formLayout);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    
    mainLayout->addWidget(buttonBox);
    
    connect(okButton, &QPushButton::clicked, this, &WriteOffDialog::validateInput);
    connect(cancelButton, &QPushButton::clicked, this, &WriteOffDialog::reject);
}

int WriteOffDialog::getWriteOffQuantity() const {
    return quantitySpinBox->value();
}

QString WriteOffDialog::getReason() const {
    return QString();
}

void WriteOffDialog::validateInput() {
    int quantity = quantitySpinBox->value();
    
    if (quantity <= 0) {
        QMessageBox::warning(this, "Validation Error", "Write-off quantity must be greater than 0.");
        return;
    }
    
    if (quantity > product.getQuantity()) {
        QMessageBox::warning(this, "Validation Error", 
                           QString("Write-off quantity cannot exceed current quantity (%1).")
                           .arg(product.getQuantity()));
        return;
    }
    
    accept();
}

void WriteOffDialog::accept() {
    QDialog::accept();
}

