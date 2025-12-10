#include "services/OrderService.h"

OrderService::Result OrderService::createOrder(InventoryService &inventory,
                                               DatabaseManager &db,
                                               const Order &order) {
  Result result;

  if (!db.addOrder(order)) {
    result.saved = false;
    return result;
  }

  for (const auto &item : order.getItems()) {
    auto productPtr = inventory.getProduct(item.productId);
    if (productPtr) {
      int newQuantity = productPtr->getQuantity() - item.quantity;
      if (newQuantity < 0)
        newQuantity = 0;
      productPtr->setQuantity(newQuantity);
    }
  }

  result.saved = true;
  result.totalAmount = order.getTotalAmount();
  return result;
}
