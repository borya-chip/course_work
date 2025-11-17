#pragma once

#include <QAbstractTableModel>
#include <vector>
#include <memory>
#include "Product.h"
#include "InventoryManager.h"

class ProductModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Id = 0,
        Name,
        Category,
        Quantity,
        UnitPrice,
        TotalValue,
        Actions,
        ColumnCount
    };

    explicit ProductModel(InventoryManager* inventoryManager, QObject* parent = nullptr);
    ~ProductModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, 
                       int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, 
                int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void setProducts(const std::vector<Product>& products);
    Product getProduct(int row) const;
    void refresh();

private:
    std::vector<Product> products;
    InventoryManager* inventoryManager;
};

