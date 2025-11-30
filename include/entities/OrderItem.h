#pragma once

#include "entities/Product.h"
#include <QString>

struct OrderItem {
    int productId;
    QString productName;
    QString category;
    int quantity;
    double unitPrice;
    double discountPercent;
    double totalPrice;
    
    OrderItem() : productId(0), quantity(0), unitPrice(0.0), discountPercent(0.0), totalPrice(0.0) {}
    
    OrderItem(int id, const QString& name, const QString& cat, int qty, double price)
        : productId(id), productName(name), category(cat), quantity(qty), 
          unitPrice(price), discountPercent(0.0), totalPrice(0.0) {}
    
    void calculateTotal(double discount) {
        discountPercent = discount;
        double priceAfterDiscount = unitPrice * (1.0 - discount / 100.0);
        totalPrice = priceAfterDiscount * quantity;
    }
};








