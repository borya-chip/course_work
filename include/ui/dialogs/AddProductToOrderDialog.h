#pragma once

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <vector>
#include "entities/Product.h"

class AddProductToOrderDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddProductToOrderDialog(const std::vector<Product>& availableProducts, 
                                     bool isWholesale, QWidget* parent = nullptr);
    
    Product getSelectedProduct() const;
    int getQuantity() const { return quantitySpinBox->value(); }

private slots:
    void onProductChanged(int index);
    void updatePricePreview();

private:
    void setupUI();
    
    QComboBox* productComboBox;
    QSpinBox* quantitySpinBox;
    QLabel* pricePreviewLabel;
    QLabel* discountLabel;
    QLabel* totalPriceLabel;
    QPushButton* addButton;
    QPushButton* cancelButton;
    
    std::vector<Product> availableProducts;
    bool isWholesale;
    Product selectedProduct;
};



