#pragma once

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include "entities/Product.h"

class WriteOffDialog : public QDialog {
    Q_OBJECT

public:
    explicit WriteOffDialog(const Product& product, QWidget* parent = nullptr);
    
    int getWriteOffQuantity() const;
    QString getReason() const;

private slots:
    void validateInput();
    void accept() override;

private:
    void setupUI();
    
    Product product;
    QLabel* productNameLabel;
    QLabel* currentQuantityLabel;
    QLabel* totalValueLabel;
    QSpinBox* quantitySpinBox;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

