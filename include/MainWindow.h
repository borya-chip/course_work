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
#include "ProductModel.h"
#include "InventoryManager.h"
#include "FileManager.h"
#include "ActionsDelegate.h"
#include "InventoryDialog.h"

QT_BEGIN_NAMESPACE
class QTableView;
class QLabel;
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
    
    void exportReport();
    
    void searchProducts();
    void filterByCategory();
    
    void refreshTable();
    
    void onSelectionChanged();
    
    void editProductByRow(int row);
    void deleteProductByRow(int row);
    void writeOffProductByRow(int row);
    void reportProductByRow(int row);


private:
    void setupUI();
    void setupTable();
    void setupSearchBar();
    void setupButtons();
    void connectSignals();
    
    QWidget* createTopBar();

    QTableView* tableView;
    ProductModel* productModel;
    ActionsDelegate* actionsDelegate;
    QLineEdit* searchLineEdit;
    QComboBox* categoryComboBox;
    QPushButton* addButton;
    QPushButton* searchButton;
    
    InventoryManager* inventoryManager;
    QString dataFilePath;
};

