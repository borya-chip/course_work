#pragma once

#include "AbstractProduct.h"

class NonFoodProduct : public AbstractProduct {
private:
    std::string warrantyPeriod;
    bool isFragile;

public:
    NonFoodProduct(const std::string& name, const std::string& category,
                  int quantity, double unitPrice, const QDate& expirationDate,
                  const std::string& warrantyPeriod = "1 year",
                  bool isFragile = false);

    double calculateTotalValue() const override;
    std::string getProductType() const override;

    std::string getWarrantyPeriod() const { return warrantyPeriod; }
    bool getIsFragile() const { return isFragile; }
    void setWarrantyPeriod(const std::string& period) { warrantyPeriod = period; }
    void setIsFragile(bool fragile) { isFragile = fragile; }
};

