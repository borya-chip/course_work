#include "services/WriteOffCalculator.h"
#include "managers/DatabaseManager.h"
#include <algorithm>

double WriteOffCalculator::calculateWriteOffValue(const Product& product, int quantity) {
    if (quantity < 0 || quantity > product.getQuantity()) {
        return 0.0;
    }
    return quantity * product.getUnitPrice();
}

double WriteOffCalculator::calculateTotalWriteOffValue(const std::vector<std::shared_ptr<Product>>& products) {
    double total = 0.0;
    for (const auto& product : products) {
        if (product) {
            total += product->calculateTotalValue();
        }
    }
    return total;
}

bool WriteOffCalculator::shouldWriteOffDamaged(const Product& product, double damagePercentage) {
    if (damagePercentage < 0 || damagePercentage > 100) {
        return false;
    }
    return damagePercentage > 50.0;
}

std::vector<WriteOffRecord> WriteOffCalculator::generateWriteOffReport(
    const std::vector<std::shared_ptr<Product>>& writeOffHistory) {
    
    std::vector<WriteOffRecord> records;
    int recordId = 1;
    for (const auto& product : writeOffHistory) {
        if (product) {
            WriteOffRecord record;
            record.id = recordId++;
            record.productId = product->getId();
            record.productName = product->getName();
            record.quantity = product->getQuantity();
            record.value = product->calculateTotalValue();
            record.reason = "Damaged or written off";
            records.push_back(record);
        }
    }
    return records;
}

