#pragma once

#include "entities/Product.h"
#include "managers/DatabaseManager.h"
#include <vector>
#include <memory>

class WriteOffCalculator {
public:
    static double calculateWriteOffValue(const Product& product, int quantity);
    
    static double calculateTotalWriteOffValue(const std::vector<std::shared_ptr<Product>>& products);
    
    static bool shouldWriteOffDamaged(const Product& product, double damagePercentage);
    
    static std::vector<WriteOffRecord> generateWriteOffReport(
        const std::vector<std::shared_ptr<Product>>& writeOffHistory);
};

