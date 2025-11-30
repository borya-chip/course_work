#include "ui/dialogs/AddProductToOrderDialog.h"
#include "services/DiscountCalculator.h"
#include <QMessageBox>
#include <QFormLayout>
#include <QGroupBox>

AddProductToOrderDialog::AddProductToOrderDialog(const std::vector<Product>& availableProducts, 
                                                 bool isWholesale, QWidget* parent)
    : QDialog(parent), availableProducts(availableProducts), isWholesale(isWholesale) {
    setupUI();
}

void AddProductToOrderDialog::setupUI() {
    setWindowTitle("Add Product to Order");
    setMinimumWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();
    
    productComboBox = new QComboBox(this);
    productComboBox->setMinimumWidth(300);
    for (const auto& product : availableProducts) {
        QString displayText = QString("%1 (ID: %2, Price: $%3, Stock: %4)")
            .arg(QString::fromStdString(product.getName()))
            .arg(product.getId())
            .arg(product.getUnitPrice(), 0, 'f', 2)
            .arg(product.getQuantity());
        productComboBox->addItem(displayText, product.getId());
    }
    formLayout->addRow("Product:", productComboBox);
    
    quantitySpinBox = new QSpinBox(this);
    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setMaximum(9999);
    quantitySpinBox->setValue(1);
    formLayout->addRow("Quantity:", quantitySpinBox);
    
    mainLayout->addLayout(formLayout);
    
    QGroupBox* previewGroupBox = new QGroupBox("Price Preview", this);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroupBox);
    
    pricePreviewLabel = new QLabel("Unit Price: $0.00", this);
    discountLabel = new QLabel("Discount: 0%", this);
    totalPriceLabel = new QLabel("Total: $0.00", this);
    totalPriceLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    
    previewLayout->addWidget(pricePreviewLabel);
    previewLayout->addWidget(discountLabel);
    previewLayout->addWidget(totalPriceLabel);
    
    mainLayout->addWidget(previewGroupBox);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Add", this);
    addButton->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px;");
    cancelButton = new QPushButton("Cancel", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    connect(productComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddProductToOrderDialog::onProductChanged);
    connect(quantitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &AddProductToOrderDialog::updatePricePreview);
    connect(addButton, &QPushButton::clicked, [this]() {
        int quantity = quantitySpinBox->value();
        if (quantity > selectedProduct.getQuantity()) {
            QMessageBox::warning(this, "Error", 
                QString("Insufficient stock! Available: %1").arg(selectedProduct.getQuantity()));
            return;
        }
        if (selectedProduct.getId() == 0) {
            QMessageBox::warning(this, "Error", "Please select a product!");
            return;
        }
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &AddProductToOrderDialog::reject);
    
    if (!availableProducts.empty()) {
        onProductChanged(0);
    }
}

void AddProductToOrderDialog::onProductChanged(int index) {
    if (index >= 0 && index < static_cast<int>(availableProducts.size())) {
        selectedProduct = availableProducts[index];
        quantitySpinBox->setMaximum(selectedProduct.getQuantity());
        updatePricePreview();
    }
}

void AddProductToOrderDialog::updatePricePreview() {
    if (selectedProduct.getId() == 0) {
        return;
    }
    
    int quantity = quantitySpinBox->value();
    double unitPrice = selectedProduct.getUnitPrice();
    double discount = DiscountCalculator::calculateDiscount(isWholesale, quantity);
    double priceAfterDiscount = unitPrice * (1.0 - discount / 100.0);
    double total = priceAfterDiscount * quantity;
    
    pricePreviewLabel->setText(QString("Unit Price: $%1").arg(unitPrice, 0, 'f', 2));
    
    if (discount > 0) {
        discountLabel->setText(QString("Discount: %1%").arg(discount, 0, 'f', 1));
        discountLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        discountLabel->setText("Discount: 0%");
        discountLabel->setStyleSheet("");
    }
    
    totalPriceLabel->setText(QString("Total: $%1").arg(total, 0, 'f', 2));
    
    if (quantity > selectedProduct.getQuantity()) {
        addButton->setEnabled(false);
        totalPriceLabel->setText(QString("Total: $%1 (Insufficient stock! Available: %2)")
            .arg(total, 0, 'f', 2)
            .arg(selectedProduct.getQuantity()));
        totalPriceLabel->setStyleSheet("font-weight: bold; color: red;");
    } else {
        addButton->setEnabled(true);
        totalPriceLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    }
}

Product AddProductToOrderDialog::getSelectedProduct() const {
    return selectedProduct;
}

