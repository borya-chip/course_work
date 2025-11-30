#pragma once

#include <string>

class AbstractProduct {
protected:
    std::string name;
    std::string category;
    int quantity;
    double unitPrice;

public:
    AbstractProduct(const std::string& name, const std::string& category, 
                   int quantity, double unitPrice);
    virtual ~AbstractProduct() = default;

    virtual double calculateTotalValue() const = 0;

    virtual std::string getProductType() const = 0;

    std::string getName() const { return name; }
    std::string getCategory() const { return category; }
    int getQuantity() const { return quantity; }
    double getUnitPrice() const { return unitPrice; }

    void setName(const std::string& name) { this->name = name; }
    void setCategory(const std::string& category) { this->category = category; }
    void setQuantity(int quantity);
    void setUnitPrice(double unitPrice);

    friend double calculateDiscountedPrice(const AbstractProduct& product, double discount);
};

double calculateDiscountedPrice(const AbstractProduct& product, double discount);

