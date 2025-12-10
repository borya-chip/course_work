#include "services/ProductFilterService.h"
#include <algorithm>

std::vector<std::shared_ptr<Product>>
ProductFilterService::filterProducts(const InventoryService &inventory,
                                     const QString &category,
                                     const QString &searchText) {
  std::vector<std::shared_ptr<Product>> products;

  if (category == "All Categories" || category.isEmpty()) {
    products = inventory.getAllProducts();
  } else {
    products = inventory.filterByCategory(category.toStdString());
  }

  if (!searchText.trimmed().isEmpty()) {
    QString lowered = searchText.trimmed().toLower();
    std::vector<std::shared_ptr<Product>> filtered;
    filtered.reserve(products.size());

    for (const auto &p : products) {
      if (!p)
        continue;
      QString productName = QString::fromStdString(p->getName()).toLower();
      if (productName.contains(lowered)) {
        filtered.push_back(p);
      }
    }
    products.swap(filtered);
  }

  return products;
}
