#include "ProductDialog.h"
#include "Exceptions.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDate>

ProductDialog::ProductDialog(QWidget* parent)
    : QDialog(parent), editMode(false), productId(-1) {
    setupUI();
}

ProductDialog::ProductDialog(const Product& product, QWidget* parent)
    : QDialog(parent), editMode(true), productId(product.getId()) {
    setupUI();
    populateFields(product);
}

void ProductDialog::setupUI() {
    setWindowTitle(editMode ? "Edit Product" : "Add Product");
    setMinimumWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();
    
    idSpinBox = new QSpinBox(this);
    idSpinBox->setMinimum(1);
    idSpinBox->setMaximum(999999);
    if (editMode) {
        idSpinBox->setValue(productId);
        idSpinBox->setReadOnly(true);
    }
    formLayout->addRow("ID:", idSpinBox);
    
    nameEdit = new QLineEdit(this);
    formLayout->addRow("Name:", nameEdit);
    
    categoryComboBox = new QComboBox(this);
    categoryComboBox->setEditable(true);
    categoryComboBox->addItems({"Food", "Electronics", "Clothing", "Books", "Other"});
    formLayout->addRow("Category:", categoryComboBox);
    
    quantitySpinBox = new QSpinBox(this);
    quantitySpinBox->setMinimum(0);
    quantitySpinBox->setMaximum(999999);
    formLayout->addRow("Quantity:", quantitySpinBox);
    
    unitPriceSpinBox = new QDoubleSpinBox(this);
    unitPriceSpinBox->setMinimum(0.0);
    unitPriceSpinBox->setMaximum(999999.99);
    unitPriceSpinBox->setDecimals(2);
    unitPriceSpinBox->setPrefix("$ ");
    formLayout->addRow("Unit Price:", unitPriceSpinBox);
    
    mainLayout->addLayout(formLayout);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    
    mainLayout->addWidget(buttonBox);
    
    connect(okButton, &QPushButton::clicked, this, &ProductDialog::validateInput);
    connect(cancelButton, &QPushButton::clicked, this, &ProductDialog::reject);
}

void ProductDialog::populateFields(const Product& product) {
    idSpinBox->setValue(product.getId());
    nameEdit->setText(QString::fromStdString(product.getName()));
    categoryComboBox->setCurrentText(QString::fromStdString(product.getCategory()));
    quantitySpinBox->setValue(product.getQuantity());
    unitPriceSpinBox->setValue(product.getUnitPrice());
}

Product ProductDialog::getProduct() const {
    try {
        int id = idSpinBox->value();
        std::string name = nameEdit->text().toStdString();
        std::string category = categoryComboBox->currentText().toStdString();
        int quantity = quantitySpinBox->value();
        double unitPrice = unitPriceSpinBox->value();
        QDate expirationDate = QDate::currentDate().addDays(365);
        
        Product product(name, category, quantity, unitPrice, expirationDate);
        
        product.setId(id);
        
        return product;
    } catch (const ProductException& e) {
        throw;
    }
}

void ProductDialog::validateInput() {
    if (idSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Validation Error", "ID must be greater than 0.");
        return;
    }
    
    if (nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Product name cannot be empty.");
        return;
    }
    
    if (quantitySpinBox->value() < 0) {
        QMessageBox::warning(this, "Validation Error", "Quantity cannot be negative.");
        return;
    }
    
    if (unitPriceSpinBox->value() < 0) {
        QMessageBox::warning(this, "Validation Error", "Unit price cannot be negative.");
        return;
    }
    
    accept();
}

void ProductDialog::accept() {
    try {
        getProduct();
        QDialog::accept();
    } catch (const ProductException& e) {
        QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
    }
}

