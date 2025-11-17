#pragma once

#include "AbstractProduct.h"

class FoodProduct : public AbstractProduct {
private:
    std::string storageTemperature;
    bool requiresRefrigeration;

public:
    FoodProduct(const std::string& name, const std::string& category,
               int quantity, double unitPrice, const QDate& expirationDate,
               const std::string& storageTemperature = "Room Temperature",
                  bool requiresRefrigeration = false);

    double calculateTotalValue() const override;
    std::string getProductType() const override;

    std::string getStorageTemperature() const { return storageTemperature; }
    bool getRequiresRefrigeration() const { return requiresRefrigeration; }
    void setStorageTemperature(const std::string& temp) { storageTemperature = temp; }
    void setRequiresRefrigeration(bool required) { requiresRefrigeration = required; }
};

