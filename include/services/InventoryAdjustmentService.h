#pragma once

#include "services/InventoryService.h"
#include "managers/DatabaseManager.h"
#include <QString>

// Сервис, инкапсулирующий бизнес-логику инвентаризации (Inventory Check)
class InventoryAdjustmentService {
public:
    struct Result {
        int itemsUpdated = 0;          // количество позиций, по которым были изменения
        int quantityAdded = 0;         // сколько единиц товара добавлено всего
        int quantityWrittenOff = 0;    // сколько единиц товара списано всего
    };

    // Применяет корректировку по одной строке инвентаризации.
    // id          - ID продукта
    // currentQty  - текущее количество
    // actualQty   - фактическое количество по результатам проверки
    // result      - накопительный результат по всем строкам
    // Бросает ProductException/std::exception при ошибках InventoryService/WriteOffService.
    static void applyAdjustment(InventoryService& inventory,
                                DatabaseManager* dbManager,
                                int id,
                                int currentQty,
                                int actualQty,
                                Result& result);
};
