#pragma once

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include "entities/Product.h"

class ProductDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProductDialog(QWidget* parent = nullptr);
    ProductDialog(const Product& product, QWidget* parent = nullptr);
    
    Product getProduct() const;
    bool isEditMode() const { return editMode; }

private slots:
    void validateInput();
    void accept() override;

private:
    void setupUI();
    void populateFields(const Product& product);
    
    QSpinBox* idSpinBox;
    QLineEdit* nameEdit;
    QComboBox* categoryComboBox;
    QSpinBox* quantitySpinBox;
    QDoubleSpinBox* unitPriceSpinBox;
    
    QPushButton* okButton;
    QPushButton* cancelButton;
    
    bool editMode;
    int productId;
};

