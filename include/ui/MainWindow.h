#pragma once

#include "managers/DatabaseManager.h"
#include "managers/FileManager.h"
#include "services/InventoryService.h"
#include "ui/delegates/ActionsDelegate.h"
#include "ui/dialogs/InventoryDialog.h"
#include "ui/dialogs/OrderDialog.h"
#include "ui/dialogs/SalesReportDialog.h"
#include "ui/models/ProductModel.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QPair>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QTableView>
#include <QTreeWidget>
#include <QVBoxLayout>


QT_BEGIN_NAMESPACE
class QTableView;
class QLabel;
class QTreeWidgetItem;
QT_END_NAMESPACE

class QTextEdit;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void addProduct();
  void generateReport();
  void openInventory();
  void createOrder();
  void editOrder(int orderId);
  void showSalesReport();
  void saveOrderHistoryToTxt(const Order &order);

  void exportReport();

  void searchProducts();
  void filterByCategory();
  void applyFilters();

  void refreshTable();

  void onSelectionChanged();

  void editProductByRow(int row);
  void deleteProductByRow(int row);
  void writeOffProductByRow(int row);

private:
  void setupUI();
  void setupTable();
  void setupSearchBar();
  void setupButtons();
  void connectSignals();
  void setupSidebar();
  void setupContentArea();

  QWidget *createWarehouseSection();
  QWidget *createOrdersSection();
  QWidget *createInventorySection();
  QWidget *createReportsSection();
  QString updateWriteOffsReport();
  double calculateTotalSales();
  QMap<QString, double> getCategorySalesData();
  QList<QPair<QString, double>> getTopCompaniesData(int topCount);

  QString getPrimaryButtonStyle() const;
  QString getSecondaryButtonStyle() const;
  QString getDangerButtonStyle() const;
  QString getSuccessButtonStyle() const;

  QTreeWidget *sidebarMenu;

  QWidget *contentWidget;
  QTableView *tableView;
  ProductModel *productModel;
  ActionsDelegate *actionsDelegate;
  QLineEdit *searchLineEdit;
  QComboBox *categoryComboBox;
  QPushButton *addButton;

  InventoryService *inventoryManager;
  DatabaseManager *dbManager;
  QString dataFilePath;
  QTextEdit *writeOffsReportTextEdit;

  void onSidebarItemClicked(QTreeWidgetItem *item, int column);
};
