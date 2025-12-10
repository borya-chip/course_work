#pragma once

#include <QString>
#include "services/InventoryService.h"

// Утилитный класс для проверки корректности ID продукта
class ProductValidator {
public:
    // Проверка, что ID в допустимом диапазоне ( > 0 )
    static bool isIdInRange(int id) {
        return id > 0;
    }

    // Проверка, что продукт с таким ID отсутствует в инвентаре
    static bool isIdUnique(const InventoryService& inventory, int id) {
        auto product = inventory.getProduct(id);
        return !product; // уникален, если продукта нет
    }

    // Возвращает пустую строку, если ID валиден и уникален,
    // либо текст ошибки, который можно показать пользователю.
    static QString validateId(const InventoryService& inventory, int id) {
        if (!isIdInRange(id)) {
            return "ID must be greater than 0.";
        }
        if (!isIdUnique(inventory, id)) {
            return QString("Product with ID %1 already exists.").arg(id);
        }
        return QString();
    }
};
