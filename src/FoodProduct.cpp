#include "FoodProduct.h"
#include "Exceptions.h"

FoodProduct::FoodProduct(const std::string& name, const std::string& category,
                        int quantity, double unitPrice, const QDate& expirationDate,
                        const std::string& storageTemperature, bool requiresRefrigeration)
    : AbstractProduct(name, category, quantity, unitPrice, expirationDate),
      storageTemperature(storageTemperature),
      requiresRefrigeration(requiresRefrigeration) {
}

double FoodProduct::calculateTotalValue() const {
    double baseValue = quantity * unitPrice;
    if (isExpiringSoon(7)) {
        return baseValue * 0.9;
    }
    return baseValue;
}

std::string FoodProduct::getProductType() const {
    return "Food Product";
}

