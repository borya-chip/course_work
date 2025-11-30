#include "ui/models/ProductModel.h"
#include <QColor>
#include <QDate>

ProductModel::ProductModel(InventoryService* inventoryManager, QObject* parent)
    : QAbstractTableModel(parent), inventoryManager(inventoryManager) {
    refresh();
}

ProductModel::~ProductModel() {
}

int ProductModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(products.size());
}

int ProductModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant ProductModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(products.size())) {
        return QVariant();
    }
    
    const Product& product = products[index.row()];
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Id:
                return product.getId();
            case Name:
                return QString::fromStdString(product.getName());
            case Category:
                return QString::fromStdString(product.getCategory());
            case Quantity:
                return product.getQuantity();
            case UnitPrice:
                return QString::number(product.getUnitPrice(), 'f', 2);
            case TotalValue:
                return QString::number(product.calculateTotalValue(), 'f', 2);
            case Actions:
                return "Actions";
            default:
                return QVariant();
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case Quantity:
            case UnitPrice:
            case TotalValue:
                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
            default:
                return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    
    return QVariant();
}

QVariant ProductModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    
    switch (section) {
        case Id:
            return "ID";
        case Name:
            return "Name";
        case Category:
            return "Category";
        case Quantity:
            return "Quantity";
        case UnitPrice:
            return "Unit Price";
        case TotalValue:
            return "Total Value";
        case Actions:
            return "Actions";
        default:
            return QVariant();
    }
}

bool ProductModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }
    
    if (index.row() >= static_cast<int>(products.size())) {
        return false;
    }
    
    Product& product = products[index.row()];
    
    switch (index.column()) {
        case Name:
            product.setName(value.toString().toStdString());
            break;
        case Category:
            product.setCategory(value.toString().toStdString());
            break;
        case Quantity:
            product.setQuantity(value.toInt());
            break;
        case UnitPrice:
            product.setUnitPrice(value.toDouble());
            break;
        default:
            return false;
    }
    
    Q_EMIT dataChanged(index, index, QList<int>() << role);
    return true;
}

Qt::ItemFlags ProductModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    
    return flags;
}

void ProductModel::setProducts(const std::vector<Product>& newProducts) {
    beginResetModel();
    products = newProducts;
    endResetModel();
}

Product ProductModel::getProduct(int row) const {
    if (row >= 0 && row < static_cast<int>(products.size())) {
        return products[row];
    }
    return Product();
}

void ProductModel::refresh() {
    if (inventoryManager) {
        beginResetModel();
        auto productPtrs = inventoryManager->getAllProducts();
        products.clear();
        products.reserve(productPtrs.size());
        
        int maxId = 0;
        for (const auto& productPtr : productPtrs) {
            if (productPtr) {
                products.push_back(*productPtr);
                if (productPtr->getId() > maxId) {
                    maxId = productPtr->getId();
                }
            }
        }
        
        if (maxId > 0) {
            Product::setNextId(maxId);
        }
        
        endResetModel();
    }
}

