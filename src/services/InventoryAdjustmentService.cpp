#include "services/InventoryAdjustmentService.h"
#include "exceptions/Exceptions.h"
#include "services/WriteOffService.h"


void InventoryAdjustmentService::applyAdjustment(InventoryService &inventory,
                                                 DatabaseManager *dbManager,
                                                 int id, int currentQty,
                                                 int actualQty,
                                                 Result &result) {
  int difference = actualQty - currentQty;
  if (difference == 0) {
    return;
  }

  auto product = inventory.getProduct(id);
  if (!product) {
    throw ProductException("Product with ID " + std::to_string(id) +
                           " not found during inventory adjustment");
  }

  if (difference > 0) {

    inventory.addStock(id, difference);
    result.quantityAdded += difference;
  } else {

    int writeOffQty = -difference;
    QString reason = "Inventory adjustment";
    QString productName = QString::fromStdString(product->getName());
    auto writeOffResult = WriteOffService::writeOffProduct(
        inventory, dbManager, id, writeOffQty, reason, productName);
    Q_UNUSED(writeOffResult);
    result.quantityWrittenOff += writeOffQty;
  }

  result.itemsUpdated++;
}
