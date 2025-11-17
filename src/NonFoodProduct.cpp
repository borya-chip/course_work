#include "NonFoodProduct.h"
#include "Exceptions.h"

NonFoodProduct::NonFoodProduct(const std::string& name, const std::string& category,
                              int quantity, double unitPrice, const QDate& expirationDate,
                              const std::string& warrantyPeriod, bool isFragile)
    : AbstractProduct(name, category, quantity, unitPrice, expirationDate),
      warrantyPeriod(warrantyPeriod),
      isFragile(isFragile) {
}

double NonFoodProduct::calculateTotalValue() const {
    return quantity * unitPrice;
}

std::string NonFoodProduct::getProductType() const {
    return "Non-Food Product";
}

