#include "services/WriteOffService.h"
#include "services/WriteOffCalculator.h"

WriteOffService::Result WriteOffService::writeOffProduct(InventoryService& inventory,
                                                         DatabaseManager* dbManager,
                                                         int productId,
                                                         int quantity,
                                                         const QString& reason,
                                                         const QString& productName) {
    // Выполняем само списание (валидирует количество и обновляет склад + in-memory history)
    inventory.writeOffProduct(productId, quantity, reason.toStdString());

    // Находим продукт после списания, чтобы иметь актуальные данные
    auto productPtr = inventory.getProduct(productId);
    WriteOffService::Result result{0.0, false};

    if (productPtr) {
        // Сумма списания считается отдельно: по исходному количеству списания
        result.writeOffValue = WriteOffCalculator::calculateWriteOffValue(*productPtr, quantity);
    }

    // Пишем запись в историю списаний через DatabaseManager (если он есть)
    if (dbManager && result.writeOffValue >= 0.0) {
        try {
            result.dbRecordSaved = dbManager->addWriteOffRecord(
                productId,
                quantity,
                result.writeOffValue,
                reason,
                productName.isEmpty() ? QString("Unknown Product") : productName
            );
        } catch (...) {
            result.dbRecordSaved = false;
        }
    }

    return result;
}
