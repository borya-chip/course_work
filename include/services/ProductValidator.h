#pragma once

#include "services/InventoryService.h"
#include <QString>

class ProductValidator {
public:
  static bool isIdInRange(int id) { return id > 0; }

  static bool isIdUnique(const InventoryService &inventory, int id) {
    auto product = inventory.getProduct(id);
    return !product;
  }

  static QString validateId(const InventoryService &inventory, int id) {
    if (!isIdInRange(id)) {
      return "ID must be greater than 0.";
    }
    if (!isIdUnique(inventory, id)) {
      return QString("Product with ID %1 already exists.").arg(id);
    }
    return QString();
  }
};
