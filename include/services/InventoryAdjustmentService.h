#pragma once

#include "managers/DatabaseManager.h"
#include "services/InventoryService.h"
#include <QString>

class InventoryAdjustmentService {
public:
  struct Result {
    int itemsUpdated = 0;
    int quantityAdded = 0;
    int quantityWrittenOff = 0;
  };

  static void applyAdjustment(InventoryService &inventory,
                              DatabaseManager *dbManager, int id,
                              int currentQty, int actualQty, Result &result);
};
