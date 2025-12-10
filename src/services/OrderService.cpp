#include "services/OrderService.h"

OrderService::Result OrderService::createOrder(InventoryService& inventory,
                                               DatabaseManager& db,
                                               const Order& order) {
    Result result;

    // Сохраняем заказ в БД
    if (!db.addOrder(order)) {
        result.saved = false;
        return result;
    }

    // При успехе обновляем остатки товаров
    for (const auto& item : order.getItems()) {
        auto productPtr = inventory.getProduct(item.productId);
        if (productPtr) {
            int newQuantity = productPtr->getQuantity() - item.quantity;
            if (newQuantity < 0) newQuantity = 0;
            productPtr->setQuantity(newQuantity);
        }
    }

    result.saved = true;
    result.totalAmount = order.getTotalAmount();
    return result;
}
