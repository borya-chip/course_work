#include "managers/FileManager.h"
#include "entities/Product.h"
#include "services/InventoryService.h"
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>
#include <fstream>
#include <iomanip>

static const quint32 FILE_MAGIC = 0x50524F44;

bool FileManager::saveToBinary(const InventoryService& inventory, const std::string& filename) {
    try {
        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_6_0);
        
        out << FILE_MAGIC;
        out << static_cast<quint32>(1);
        
        auto products = inventory.getAllProducts();
        
        out << static_cast<quint32>(products.size());
        
        for (const auto& product : products) {
            if (product) {
                out << static_cast<qint32>(product->getId());
                
                QString name = QString::fromStdString(product->getName());
                out << name;
                
                QString category = QString::fromStdString(product->getCategory());
                out << category;
                
                out << static_cast<qint32>(product->getQuantity());
                
                out << product->getUnitPrice();
                
                QString productType = QString::fromStdString(product->getProductType());
                out << productType;
            }
        }
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

bool FileManager::loadFromBinary(InventoryService& inventory, const std::string& filename) {
    try {
        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }
        
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_6_0);
        
        quint32 magic;
        in >> magic;
        if (magic != FILE_MAGIC) {
            file.close();
            return false;
        }
        
        quint32 version;
        in >> version;
        
        quint32 productCount;
        in >> productCount;
        
        int maxId = 0;
        for (quint32 i = 0; i < productCount; ++i) {
            qint32 id;
            in >> id;
            if (id > maxId) {
                maxId = id;
            }
            
            QString name;
            in >> name;
            
            QString category;
            in >> category;
            
            qint32 quantity;
            in >> quantity;
            
            double unitPrice;
            in >> unitPrice;
            
            QString productType;
            in >> productType;
            
            auto product = std::make_shared<Product>(
                name.toStdString(),
                category.toStdString(),
                quantity,
                unitPrice
            );
            
            product->setId(id);
            
            inventory.addProduct(product);
        }
        
        if (maxId > 0) {
            Product::setNextId(maxId);
        }
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

bool FileManager::exportReportToText(const InventoryService& inventory, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "=== INVENTORY REPORT ===\n\n";
        file << "Total Products: " << inventory.getAllProducts().size() << "\n";
        file << "Total Inventory Value: $" << std::fixed << std::setprecision(2) 
             << inventory.calculateTotalInventoryValue() << "\n";
        file << "Total Quantity: " << inventory.getTotalQuantity() << "\n\n";
        
        file << "=== PRODUCT LIST ===\n\n";
        auto products = inventory.getAllProducts();
        for (const auto& product : products) {
            if (product) {
                file << "ID: " << product->getId() << "\n";
                file << "Name: " << product->getName() << "\n";
                file << "Category: " << product->getCategory() << "\n";
                file << "Quantity: " << product->getQuantity() << "\n";
                file << "Unit Price: $" << std::fixed << std::setprecision(2) 
                     << product->getUnitPrice() << "\n";
                file << "Total Value: $" << std::fixed << std::setprecision(2) 
                     << product->calculateTotalValue() << "\n";
                file << "---\n\n";
            }
        }
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

bool FileManager::exportWriteOffHistoryToText(const InventoryService& inventory, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "=== WRITE-OFF HISTORY REPORT ===\n\n";
        auto writeOffHistory = inventory.getWriteOffHistory();
        file << "Total Write-offs: " << writeOffHistory.size() << "\n\n";
        
        double totalValue = 0.0;
        for (const auto& product : writeOffHistory) {
            if (product) {
                file << "ID: " << product->getId() << "\n";
                file << "Name: " << product->getName() << "\n";
                file << "Quantity: " << product->getQuantity() << "\n";
                file << "Value: $" << std::fixed << std::setprecision(2) 
                     << product->calculateTotalValue() << "\n";
                file << "---\n\n";
                totalValue += product->calculateTotalValue();
            }
        }
        
        file << "Total Write-off Value: $" << std::fixed << std::setprecision(2) << totalValue << "\n";
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}
