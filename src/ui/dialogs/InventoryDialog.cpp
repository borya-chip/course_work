#include "ui/dialogs/InventoryDialog.h"
#include "entities/Product.h"
#include "exceptions/Exceptions.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QTimer>

class QuantityDelegate : public QStyledItemDelegate {
public:
    explicit QuantityDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override {
        if (index.column() == 4) {
            QSpinBox* editor = new QSpinBox(parent);
            editor->setMinimum(0);
            editor->setMaximum(999999);
            return editor;
        }
        return nullptr;
    }
    
    void setEditorData(QWidget* editor, const QModelIndex& index) const override {
        if (index.column() == 4) {
            int value = index.model()->data(index, Qt::EditRole).toInt();
            QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
            spinBox->setValue(value);
        }
    }
    
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const override {
        if (index.column() == 4) {
            QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
            model->setData(index, spinBox->value(), Qt::EditRole);
        }
    }
};

InventoryDialog::InventoryDialog(InventoryService* inventoryManager, QWidget* parent)
    : QDialog(parent), inventoryManager(inventoryManager) {
    setupUI();
    updateDifferences();
}

InventoryDialog::~InventoryDialog() {
}

void InventoryDialog::setupUI() {
    setWindowTitle("Inventory");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    QLabel* titleLabel = new QLabel("Inventory Check", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);
    
    setupTable();
    mainLayout->addWidget(tableView, 1);
    
    QTimer::singleShot(0, this, [this]() {
        tableView->horizontalHeader()->setStretchLastSection(true);
        resize(tableView->horizontalHeader()->length() + 50, height());
    });
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    saveButton = new QPushButton("Save Inventory", this);
    cancelButton = new QPushButton("Cancel", this);
    
    saveButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
    );
    
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #f44336;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #da190b;"
        "}"
    );
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    connect(saveButton, &QPushButton::clicked, this, &InventoryDialog::saveInventory);
    connect(cancelButton, &QPushButton::clicked, this, &InventoryDialog::reject);
}

void InventoryDialog::setupTable() {
    tableView = new QTableView(this);
    
    auto products = inventoryManager->getAllProducts();
    inventoryItems.clear();
    inventoryItems.reserve(products.size());
    
    for (const auto& product : products) {
        if (product) {
            InventoryItem item;
            item.id = product->getId();
            item.name = QString::fromStdString(product->getName());
            item.category = QString::fromStdString(product->getCategory());
            item.currentQuantity = product->getQuantity();
            item.actualQuantity = product->getQuantity();
            item.unitPrice = product->getUnitPrice();
            inventoryItems.push_back(item);
        }
    }
    
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(6);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Name");
    model->setHeaderData(2, Qt::Horizontal, "Category");
    model->setHeaderData(3, Qt::Horizontal, "Current Quantity");
    model->setHeaderData(4, Qt::Horizontal, "Actual Quantity");
    model->setHeaderData(5, Qt::Horizontal, "Difference");
    
    for (size_t i = 0; i < inventoryItems.size(); ++i) {
        const auto& item = inventoryItems[i];
        
        QStandardItem* idItem = new QStandardItem(QString::number(item.id));
        idItem->setEditable(false);
        model->setItem(static_cast<int>(i), 0, idItem);
        
        QStandardItem* nameItem = new QStandardItem(item.name);
        nameItem->setEditable(false);
        model->setItem(static_cast<int>(i), 1, nameItem);
        
        QStandardItem* categoryItem = new QStandardItem(item.category);
        categoryItem->setEditable(false);
        model->setItem(static_cast<int>(i), 2, categoryItem);
        
        QStandardItem* currentQtyItem = new QStandardItem(QString::number(item.currentQuantity));
        currentQtyItem->setEditable(false);
        model->setItem(static_cast<int>(i), 3, currentQtyItem);
        
        QStandardItem* actualItem = new QStandardItem(QString::number(item.actualQuantity));
        actualItem->setEditable(true);
        model->setItem(static_cast<int>(i), 4, actualItem);
        
        int difference = item.actualQuantity - item.currentQuantity;
        QStandardItem* diffItem = new QStandardItem(QString::number(difference));
        diffItem->setEditable(false);
        if (difference > 0) {
            diffItem->setForeground(QBrush(QColor(Qt::green)));
        } else if (difference < 0) {
            diffItem->setForeground(QBrush(QColor(Qt::red)));
        }
        model->setItem(static_cast<int>(i), 5, diffItem);
    }
    
    tableView->setModel(model);
    tableView->setItemDelegateForColumn(4, new QuantityDelegate(this));
    
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::NoSelection);
    tableView->setAlternatingRowColors(true);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    tableView->verticalHeader()->setVisible(false);
    tableView->verticalHeader()->setMaximumWidth(0);
    tableView->setFocusPolicy(Qt::NoFocus);
    
    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    
    tableView->setStyleSheet(
        "QTableView {"
        "    border: 1px solid #ddd;"
        "    border-radius: 5px;"
        "    background-color: white;"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: transparent;"
        "    selection-color: #333;"
        "    outline: none;"
        "}"
        "QTableView::item {"
        "    padding: 8px;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableView::item:selected {"
        "    background-color: transparent;"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableView::item:hover {"
        "    background-color: transparent;"
        "}"
        "QTableView::item:focus {"
        "    border: none;"
        "    outline: none;"
        "}"
        "QTableView:focus {"
        "    outline: none;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f5f5f5;"
        "    color: #333;"
        "    padding: 10px;"
        "    border: none;"
        "    border-bottom: 2px solid #ddd;"
        "    font-weight: bold;"
        "}"
    );
    
    connect(model, &QStandardItemModel::itemChanged, this, [this](QStandardItem* item) {
        if (item && item->column() == 4) {
            int row = item->row();
            if (row >= 0 && row < static_cast<int>(inventoryItems.size())) {
                inventoryItems[row].actualQuantity = item->text().toInt();
                updateDifferences();
            }
        }
    });
    
    inventoryModel = model;
}

void InventoryDialog::updateDifferences() {
    if (!inventoryModel) return;
    
    for (int row = 0; row < inventoryModel->rowCount(); ++row) {
        int currentQty = inventoryModel->item(row, 3)->text().toInt();
        int actualQty = inventoryModel->item(row, 4)->text().toInt();
        int difference = actualQty - currentQty;
        
        QStandardItem* diffItem = inventoryModel->item(row, 5);
        if (diffItem) {
            diffItem->setText(QString::number(difference));
            if (difference > 0) {
                diffItem->setForeground(QBrush(QColor(Qt::green)));
            } else if (difference < 0) {
                diffItem->setForeground(QBrush(QColor(Qt::red)));
            } else {
                diffItem->setForeground(QBrush(QColor(Qt::black)));
            }
        }
    }
}

void InventoryDialog::saveInventory() {
    if (!inventoryModel) return;
    
    int itemsUpdated = 0;
    int itemsAdded = 0;
    int itemsWrittenOff = 0;
    
    for (int row = 0; row < inventoryModel->rowCount(); ++row) {
        int id = inventoryModel->item(row, 0)->text().toInt();
        int currentQty = inventoryModel->item(row, 3)->text().toInt();
        int actualQty = inventoryModel->item(row, 4)->text().toInt();
        int difference = actualQty - currentQty;
        
        if (difference == 0) continue;
        
        try {
            auto product = inventoryManager->getProduct(id);
            if (product) {
                if (difference > 0) {
                    inventoryManager->addStock(id, difference);
                    itemsAdded++;
                } else {
                    int writeOffQty = -difference;
                    inventoryManager->writeOffProduct(id, writeOffQty, "Inventory adjustment");
                    itemsWrittenOff++;
                }
                itemsUpdated++;
            }
        } catch (const ProductException& e) {
            QMessageBox::warning(this, "Error", 
                QString("Failed to update product ID %1: %2")
                .arg(id).arg(QString::fromStdString(e.what())));
        }
    }
    
    if (itemsUpdated > 0) {
        QMessageBox::information(this, "Success", 
            QString("Inventory saved successfully!\n"
                   "Items updated: %1\n"
                   "Items added: %2\n"
                   "Items written off: %3")
            .arg(itemsUpdated)
            .arg(itemsAdded)
            .arg(itemsWrittenOff));
        accept();
    } else {
        QMessageBox::information(this, "Info", "No changes to save.");
    }
}

void InventoryDialog::onCellChanged(int row, int column) {
    Q_UNUSED(row);
    Q_UNUSED(column);
    updateDifferences();
}

