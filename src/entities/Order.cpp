#include "entities/Order.h"
#include "services/DiscountCalculator.h"
#include <algorithm>

int Order::nextId = 1;

void Order::setNextId(int id) {
    nextId = id + 1;
}

Order::Order() 
    : id(nextId++), orderType(OrderType::RETAIL), 
      totalAmount(0.0), totalDiscount(0.0), orderDate(QDate::currentDate()) {
}

Order::Order(const QString& company, const QString& contact, const QString& phoneNum, OrderType type)
    : id(nextId++), companyName(company), contactPerson(contact), phone(phoneNum),
      orderType(type), totalAmount(0.0), totalDiscount(0.0), orderDate(QDate::currentDate()) {
}

void Order::addItem(const OrderItem& item) {
    // Проверяем, есть ли уже такой товар в заказе
    auto it = std::find_if(items.begin(), items.end(),
        [&item](const OrderItem& i) { return i.productId == item.productId; });
    
    if (it != items.end()) {
        // Увеличиваем количество существующего товара
        it->quantity += item.quantity;
    } else {
        // Добавляем новый товар
        items.push_back(item);
    }
    
    calculateTotal();
}

void Order::removeItem(int productId) {
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [productId](const OrderItem& item) { return item.productId == productId; }),
        items.end()
    );
    calculateTotal();
}

void Order::updateItemQuantity(int productId, int quantity) {
    auto it = std::find_if(items.begin(), items.end(),
        [productId](const OrderItem& item) { return item.productId == productId; });
    
    if (it != items.end()) {
        if (quantity <= 0) {
            removeItem(productId);
        } else {
            it->quantity = quantity;
            calculateTotal();
        }
    }
}

void Order::clearItems() {
    items.clear();
    totalAmount = 0.0;
    totalDiscount = 0.0;
}

void Order::calculateTotal() {
    totalAmount = 0.0;
    totalDiscount = 0.0;
    
    bool isWholesale = (orderType == OrderType::WHOLESALE);
    
    for (auto& item : items) {
        double discount = DiscountCalculator::calculateDiscount(isWholesale, item.quantity);
        item.calculateTotal(discount);
        
        double originalPrice = item.unitPrice * item.quantity;
        totalAmount += item.totalPrice;
        totalDiscount += (originalPrice - item.totalPrice);
    }
}

