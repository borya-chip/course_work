#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QListWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QGroupBox>
#include <QMap>
#include <QList>
#include <QPair>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "ui/models/ProductModel.h"
#include "services/InventoryService.h"
#include "managers/FileManager.h"
#include "ui/delegates/ActionsDelegate.h"
#include "ui/dialogs/InventoryDialog.h"
#include "ui/dialogs/OrderDialog.h"
#include "ui/dialogs/SalesReportDialog.h"
#include "managers/DatabaseManager.h"

QT_BEGIN_NAMESPACE
class QTableView;
class QLabel;
class QTreeWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void addProduct();
    void generateReport();
    void openInventory();
    void createOrder();
    void editOrder(int orderId);
    void showSalesReport();
    void saveOrderHistoryToTxt(const Order& order);
    
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
    
    QWidget* createWarehouseSection();
    QWidget* createOrdersSection();
    QWidget* createInventorySection();
    QWidget* createReportsSection();
    double calculateTotalSales();
    QMap<QString, double> getCategorySalesData();
    QList<QPair<QString, double>> getTopCompaniesData(int topCount);
    
    // Button styles
    QString getPrimaryButtonStyle() const;
    QString getSecondaryButtonStyle() const;
    QString getDangerButtonStyle() const;
    QString getSuccessButtonStyle() const;

    // Sidebar
    QTreeWidget* sidebarMenu;
    
    // Content area
    QWidget* contentWidget;
    QTableView* tableView;
    ProductModel* productModel;
    ActionsDelegate* actionsDelegate;
    QLineEdit* searchLineEdit;
    QComboBox* categoryComboBox;
    QPushButton* addButton;
    
    InventoryService* inventoryManager;
    DatabaseManager* dbManager;
    QString dataFilePath;
    
    // Navigation slots
    void onSidebarItemClicked(QTreeWidgetItem* item, int column);
};

