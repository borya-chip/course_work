#include "services/InventoryService.h"
#include "exceptions/Exceptions.h"
#include <algorithm>
#include <string>

InventoryService::InventoryService() {}

void InventoryService::addProduct(std::shared_ptr<Product> product) {
  try {
    repository.add(product);
  } catch (const ProductException &e) {
    throw;
  }
}

void InventoryService::updateProduct(int id, std::shared_ptr<Product> product) {
  try {
    auto existingProduct = repository.findById(id);
    if (!existingProduct) {
      throw ProductNotFoundException("Product with ID " + std::to_string(id) +
                                     " not found");
    }
    repository.remove(id);
    repository.add(product);
  } catch (const ProductException &e) {
    throw;
  }
}

void InventoryService::deleteProduct(int id) {
  try {
    auto product = repository.findById(id);
    if (!product) {
      throw ProductNotFoundException("Product with ID " + std::to_string(id) +
                                     " not found");
    }
    repository.remove(id);
  } catch (const ProductException &e) {
    throw;
  }
}

std::shared_ptr<Product> InventoryService::getProduct(int id) const {
  return repository.findById(id);
}

std::vector<std::shared_ptr<Product>> InventoryService::getAllProducts() const {
  return repository.findAll();
}

void InventoryService::addStock(int id, int quantity) {
  try {
    auto product = repository.findById(id);
    if (!product) {
      throw ProductNotFoundException("Product with ID " + std::to_string(id) +
                                     " not found");
    }
    if (quantity < 0) {
      throw NegativeQuantityException("Stock quantity cannot be negative");
    }
    *product += quantity;
  } catch (const ProductException &e) {
    throw;
  }
}

void InventoryService::removeStock(int id, int quantity) {
  try {
    auto product = repository.findById(id);
    if (!product) {
      throw ProductNotFoundException("Product with ID " + std::to_string(id) +
                                     " not found");
    }
    if (quantity < 0) {
      throw NegativeQuantityException("Stock quantity cannot be negative");
    }
    *product -= quantity;
  } catch (const ProductException &e) {
    throw;
  }
}

std::vector<std::shared_ptr<Product>>
InventoryService::searchProducts(const std::string &name) const {
  return repository.searchByName(name);
}

std::vector<std::shared_ptr<Product>>
InventoryService::filterByCategory(const std::string &category) const {
  return repository.searchByCategory(category);
}

double InventoryService::calculateTotalInventoryValue() const {
  return repository.calculateTotalInventoryValue();
}

double InventoryService::calculateTotalInventoryCost() const {
  return calculateTotalInventoryValue();
}

int InventoryService::getTotalProductCount() const {
  return static_cast<int>(repository.size());
}

int InventoryService::getTotalQuantity() const {
  int total = 0;
  for (const auto &product : repository) {
    if (product) {
      total += product->getQuantity();
    }
  }
  return total;
}

void InventoryService::writeOffProduct(int id, int quantity,
                                       const std::string &reason) {

  auto product = repository.findById(id);
  if (!product) {
    throw ProductNotFoundException("Product with ID " + std::to_string(id) +
                                   " not found");
  }

  if (quantity < 0) {
    throw NegativeQuantityException("Write-off quantity cannot be negative");
  }

  int currentQuantity = 0;
  try {
    currentQuantity = product->getQuantity();
  } catch (const std::exception &e) {
    throw ProductException(std::string("Failed to get product quantity: ") +
                           e.what());
  }

  if (quantity > currentQuantity) {
    throw NegativeQuantityException(
        "Invalid write-off quantity: cannot write off more than available");
  }

  int newQuantity = currentQuantity - quantity;
  if (newQuantity < 0) {
    throw NegativeQuantityException("Resulting quantity cannot be negative");
  }

  try {
    if (product) {
      auto writeOffProduct = std::make_shared<Product>(*product);
      writeOffProduct->setQuantity(quantity);
      writeOffHistory.push_back(writeOffProduct);
    }
  } catch (const std::exception &e) {

  } catch (...) {
  }

  try {

    if (!product) {
      throw ProductException(
          "Product became invalid during write-off operation");
    }

    product->setQuantity(newQuantity);
  } catch (const NegativeQuantityException &e) {

    throw;
  } catch (const ProductException &e) {

    throw;
  } catch (const std::exception &e) {
    throw ProductException(std::string("Failed to write off product: ") +
                           e.what());
  } catch (...) {
    throw ProductException("Failed to write off product: unknown error");
  }
}

void InventoryService::sortProductsByName() { repository.sortByName(); }

void InventoryService::sortProductsByPrice() { repository.sortByPrice(); }

void InventoryService::sortProductsByQuantity() { repository.sortByQuantity(); }

void InventoryService::sortProductsByCategory() { repository.sortByCategory(); }
