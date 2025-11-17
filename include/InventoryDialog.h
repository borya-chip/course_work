#pragma once

#include <QDialog>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStandardItemModel>
#include <vector>
#include "InventoryManager.h"

class InventoryDialog : public QDialog {
    Q_OBJECT

public:
    explicit InventoryDialog(InventoryManager* inventoryManager, QWidget* parent = nullptr);
    ~InventoryDialog();

private slots:
    void saveInventory();
    void onCellChanged(int row, int column);

private:
    void setupUI();
    void setupTable();
    void updateDifferences();
    
    InventoryManager* inventoryManager;
    QTableView* tableView;
    QStandardItemModel* inventoryModel;
    QPushButton* saveButton;
    QPushButton* cancelButton;
    
    struct InventoryItem {
        int id;
        QString name;
        QString category;
        int currentQuantity;
        int actualQuantity;
        double unitPrice;
    };
    
    std::vector<InventoryItem> inventoryItems;
};

