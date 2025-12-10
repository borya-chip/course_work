#pragma once

#include "managers/DatabaseManager.h"
#include "services/InventoryService.h"
#include <QString>

class WriteOffService {
public:
  struct Result {
    double writeOffValue;
    bool dbRecordSaved;
  };

  static Result writeOffProduct(InventoryService &inventory,
                                DatabaseManager *dbManager, int productId,
                                int quantity, const QString &reason,
                                const QString &productName);
};
