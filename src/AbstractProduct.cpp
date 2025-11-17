#include "AbstractProduct.h"
#include "Exceptions.h"
#include <QDate>

AbstractProduct::AbstractProduct(const std::string& name, const std::string& category,
                               int quantity, double unitPrice, const QDate& expirationDate)
    : name(name), category(category), quantity(quantity), 
      unitPrice(unitPrice), expirationDate(expirationDate) {
    if (quantity < 0) {
        throw NegativeQuantityException("Quantity cannot be negative");
    }
    if (unitPrice < 0) {
        throw InvalidPriceException("Unit price cannot be negative");
    }
}

void AbstractProduct::setQuantity(int quantity) {
    if (quantity < 0) {
        throw NegativeQuantityException("Quantity cannot be negative");
    }
    this->quantity = quantity;
}

void AbstractProduct::setUnitPrice(double unitPrice) {
    if (unitPrice < 0) {
        throw InvalidPriceException("Unit price cannot be negative");
    }
    this->unitPrice = unitPrice;
}

bool AbstractProduct::isExpired() const {
    return expirationDate < QDate::currentDate();
}

bool AbstractProduct::isExpiringSoon(int days) const {
    QDate futureDate = QDate::currentDate().addDays(days);
    return expirationDate >= QDate::currentDate() && expirationDate <= futureDate;
}

double calculateDiscountedPrice(const AbstractProduct& product, double discount) {
    if (discount < 0 || discount > 1.0) {
        throw InvalidPriceException("Discount must be between 0 and 1");
    }
    return product.unitPrice * (1.0 - discount);
}

