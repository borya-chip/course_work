#pragma once

#include "entities/OrderItem.h"
#include <QString>
#include <QDate>
#include <vector>

class DatabaseManager;

enum class OrderType {
    RETAIL,  // Розница
    WHOLESALE  // Опт
};

class Order {
    friend class DatabaseManager;
    
private:
    int id;
    QString companyName;
    QString contactPerson;
    QString phone;
    OrderType orderType;
    QDate orderDate;
    std::vector<OrderItem> items;
    double totalAmount;
    double totalDiscount;
    
    static int nextId;
    
public:
    Order();
    Order(const QString& company, const QString& contact, const QString& phoneNum, OrderType type);
    
    int getId() const { return id; }
    QString getCompanyName() const { return companyName; }
    QString getContactPerson() const { return contactPerson; }
    QString getPhone() const { return phone; }
    OrderType getOrderType() const { return orderType; }
    QDate getOrderDate() const { return orderDate; }
    const std::vector<OrderItem>& getItems() const { return items; }
    double getTotalAmount() const { return totalAmount; }
    double getTotalDiscount() const { return totalDiscount; }
    
    void setCompanyName(const QString& name) { companyName = name; }
    void setContactPerson(const QString& contact) { contactPerson = contact; }
    void setPhone(const QString& phoneNum) { phone = phoneNum; }
    void setOrderType(OrderType type) { orderType = type; }
    void setOrderDate(const QDate& date) { orderDate = date; }
    void setId(int newId) { id = newId; }
    
    void addItem(const OrderItem& item);
    void removeItem(int productId);
    void updateItemQuantity(int productId, int quantity);
    void clearItems();
    
    void calculateTotal();
    
    QString getOrderTypeString() const {
        return orderType == OrderType::RETAIL ? "Retail" : "Wholesale";
    }
    
    static void setNextId(int maxId);
};

