#pragma once

#include <QString>
#include "services/InventoryService.h"
#include "managers/DatabaseManager.h"

// Сервис, инкапсулирующий бизнес-логику списания товара
class WriteOffService {
public:
    struct Result {
        double writeOffValue;   // сумма списания
        bool dbRecordSaved;     // удалось ли сохранить запись в базе (writeoff.dat)
    };

    // Выполняет списание товара:
    //  - вызывает InventoryService::writeOffProduct
    //  - считает сумму списания через WriteOffCalculator
    //  - при наличии dbManager добавляет запись в историю списаний
    // Бросает те же исключения, что и writeOffProduct (для валидации количества и т.п.).
    static Result writeOffProduct(InventoryService& inventory,
                                  DatabaseManager* dbManager,
                                  int productId,
                                  int quantity,
                                  const QString& reason,
                                  const QString& productName);
};
