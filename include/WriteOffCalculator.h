#pragma once

#include "Product.h"
#include <vector>
#include <memory>
#include <QDate>

struct WriteOffRecord {
    int productId;
    std::string productName;
    int quantity;
    double totalValue;
    QDate writeOffDate;
    std::string reason;
};

class WriteOffCalculator {
public:
    static double calculateWriteOffValue(const Product& product, int quantity);
    
    static double calculateTotalWriteOffValue(const std::vector<std::shared_ptr<Product>>& products);
    
    static bool shouldWriteOffExpired(const Product& product);
    
    static bool shouldWriteOffDamaged(const Product& product, double damagePercentage);
    
    static std::vector<WriteOffRecord> generateWriteOffReport(
        const std::vector<std::shared_ptr<Product>>& writeOffHistory);
    
    static double calculateMonthlyWriteOffValue(const std::vector<WriteOffRecord>& records, 
                                               int month, int year);
    
    static int calculateMonthlyWriteOffCount(const std::vector<WriteOffRecord>& records,
                                            int month, int year);
};

