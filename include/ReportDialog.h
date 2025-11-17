#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include "InventoryManager.h"

class ReportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReportDialog(const InventoryManager& inventory, QWidget* parent = nullptr);

private slots:
    void generateReport();
    void exportReport();

private:
    void setupUI();
    void generateInventoryReport();
    void generateWriteOffReport();
    
    const InventoryManager& inventoryManager;
    QTextEdit* reportTextEdit;
    QComboBox* reportTypeComboBox;
    QPushButton* generateButton;
    QPushButton* exportButton;
    QPushButton* closeButton;
};

