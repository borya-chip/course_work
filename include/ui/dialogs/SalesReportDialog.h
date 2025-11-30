#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "entities/Order.h"
#include "managers/DatabaseManager.h"

class SalesReportDialog : public QDialog {
    Q_OBJECT

public:
    explicit SalesReportDialog(QWidget* parent = nullptr);

private slots:
    void generateReport();
    void onReportTypeChanged(int index);
    void exportReport();

private:
    void setupUI();
    void setupFilters();
    void setupReportTable();
    void setupStatistics();
    void refreshReport();
    void calculateStatistics();
    
    QGroupBox* createFiltersGroupBox();
    QGroupBox* createReportTableGroupBox();
    QGroupBox* createStatisticsGroupBox();
    
    // UI Components
    QComboBox* reportTypeComboBox;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;
    QPushButton* exportButton;
    QTableWidget* reportTable;
    QLabel* totalRetailLabel;
    QLabel* totalWholesaleLabel;
    QLabel* totalAmountLabel;
    QLabel* totalDiscountLabel;
    QLabel* ordersCountLabel;
    
    // Data
    std::vector<Order> currentOrders;
    DatabaseManager* dbManager;
};

