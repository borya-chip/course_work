#pragma once

#include <vector>
#include <memory>
#include <QString>
#include "entities/Product.h"
#include "services/InventoryService.h"

// Сервис для фильтрации и поиска продуктов
class ProductFilterService {
public:
    // Возвращает список продуктов (shared_ptr), отфильтрованных по категории и строке поиска.
    // category      - текст из comboBox (например, "All Categories", "Food" и т.п.)
    // searchText    - строка поиска по имени (регистр игнорируется)
    static std::vector<std::shared_ptr<Product>> filterProducts(
        const InventoryService& inventory,
        const QString& category,
        const QString& searchText);
};
