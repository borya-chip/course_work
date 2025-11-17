#pragma once

#include <string>
#include <QDate>
#include <QString>

class AbstractProduct {
protected:
    std::string name;
    std::string category;
    int quantity;
    double unitPrice;
    QDate expirationDate;

public:
    AbstractProduct(const std::string& name, const std::string& category, 
                   int quantity, double unitPrice, const QDate& expirationDate);
    virtual ~AbstractProduct() = default;

    virtual double calculateTotalValue() const = 0;

    virtual std::string getProductType() const = 0;
    virtual bool isExpired() const;
    virtual bool isExpiringSoon(int days = 30) const;

    std::string getName() const { return name; }
    std::string getCategory() const { return category; }
    int getQuantity() const { return quantity; }
    double getUnitPrice() const { return unitPrice; }
    QDate getExpirationDate() const { return expirationDate; }

    void setName(const std::string& name) { this->name = name; }
    void setCategory(const std::string& category) { this->category = category; }
    void setQuantity(int quantity);
    void setUnitPrice(double unitPrice);
    void setExpirationDate(const QDate& date) { this->expirationDate = date; }

    friend double calculateDiscountedPrice(const AbstractProduct& product, double discount);
};

double calculateDiscountedPrice(const AbstractProduct& product, double discount);

