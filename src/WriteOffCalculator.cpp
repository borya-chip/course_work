#include "WriteOffCalculator.h"
#include <QDate>
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

bool WriteOffCalculator::shouldWriteOffExpired(const Product& product) {
    return product.isExpired();
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
    for (const auto& product : writeOffHistory) {
        if (product) {
            WriteOffRecord record;
            record.productId = product->getId();
            record.productName = product->getName();
            record.quantity = product->getQuantity();
            record.totalValue = product->calculateTotalValue();
            record.writeOffDate = QDate::currentDate();
            record.reason = "Expired or damaged";
            records.push_back(record);
        }
    }
    return records;
}

double WriteOffCalculator::calculateMonthlyWriteOffValue(
    const std::vector<WriteOffRecord>& records, int month, int year) {
    
    double total = 0.0;
    for (const auto& record : records) {
        if (record.writeOffDate.month() == month && record.writeOffDate.year() == year) {
            total += record.totalValue;
        }
    }
    return total;
}

int WriteOffCalculator::calculateMonthlyWriteOffCount(
    const std::vector<WriteOffRecord>& records, int month, int year) {
    
    int count = 0;
    for (const auto& record : records) {
        if (record.writeOffDate.month() == month && record.writeOffDate.year() == year) {
            count++;
        }
    }
    return count;
}

