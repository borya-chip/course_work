#pragma once

#include "entities/Order.h"
#include "managers/DatabaseManager.h"
#include "services/InventoryService.h"

class OrderService {
public:
  struct Result {
    bool saved = false;
    double totalAmount = 0.0;
  };

  static Result createOrder(InventoryService &inventory, DatabaseManager &db,
                            const Order &order);
};
