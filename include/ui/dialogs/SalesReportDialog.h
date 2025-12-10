#pragma once

#include "entities/Order.h"
#include "managers/DatabaseManager.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>


class SalesReportDialog : public QDialog {
  Q_OBJECT

public:
  explicit SalesReportDialog(QWidget *parent = nullptr);

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

  QGroupBox *createFiltersGroupBox();
  QGroupBox *createReportTableGroupBox();
  QGroupBox *createStatisticsGroupBox();

  QComboBox *reportTypeComboBox;
  QDateEdit *startDateEdit;
  QDateEdit *endDateEdit;
  QPushButton *exportButton;
  QTableWidget *reportTable;
  QLabel *totalRetailLabel;
  QLabel *totalWholesaleLabel;
  QLabel *totalAmountLabel;
  QLabel *totalDiscountLabel;
  QLabel *ordersCountLabel;

  std::vector<Order> currentOrders;
  DatabaseManager *dbManager;
};
