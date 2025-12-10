#pragma once

#include "entities/Product.h"
#include "services/InventoryService.h"
#include <QString>
#include <memory>
#include <vector>

class ProductFilterService {
public:
  static std::vector<std::shared_ptr<Product>>
  filterProducts(const InventoryService &inventory, const QString &category,
                 const QString &searchText);
};
