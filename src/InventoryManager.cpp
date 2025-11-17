#include "InventoryManager.h"
#include "Exceptions.h"
#include <algorithm>
#include <QDate>

InventoryManager::InventoryManager() {
}

void InventoryManager::addProduct(std::shared_ptr<Product> product) {
    try {
        repository.add(product);
    } catch (const ProductException& e) {
        throw;
    }
}

void InventoryManager::updateProduct(int id, std::shared_ptr<Product> product) {
    try {
        auto existingProduct = repository.findById(id);
        if (!existingProduct) {
            throw ProductNotFoundException("Product with ID " + std::to_string(id) + " not found");
        }
        repository.remove(id);
        repository.add(product);
    } catch (const ProductException& e) {
        throw;
    }
}

void InventoryManager::deleteProduct(int id) {
    try {
        auto product = repository.findById(id);
        if (!product) {
            throw ProductNotFoundException("Product with ID " + std::to_string(id) + " not found");
        }
        repository.remove(id);
    } catch (const ProductException& e) {
        throw;
    }
}

std::shared_ptr<Product> InventoryManager::getProduct(int id) const {
    return repository.findById(id);
}

std::vector<std::shared_ptr<Product>> InventoryManager::getAllProducts() const {
    return repository.findAll();
}

void InventoryManager::addStock(int id, int quantity) {
    try {
        auto product = repository.findById(id);
        if (!product) {
            throw ProductNotFoundException("Product with ID " + std::to_string(id) + " not found");
        }
        if (quantity < 0) {
            throw NegativeQuantityException("Stock quantity cannot be negative");
        }
        *product += quantity;
    } catch (const ProductException& e) {
        throw;
    }
}

void InventoryManager::removeStock(int id, int quantity) {
    try {
        auto product = repository.findById(id);
        if (!product) {
            throw ProductNotFoundException("Product with ID " + std::to_string(id) + " not found");
        }
        if (quantity < 0) {
            throw NegativeQuantityException("Stock quantity cannot be negative");
        }
        *product -= quantity;
    } catch (const ProductException& e) {
        throw;
    }
}

std::vector<std::shared_ptr<Product>> InventoryManager::searchProducts(const std::string& name) const {
    return repository.searchByName(name);
}

std::vector<std::shared_ptr<Product>> InventoryManager::filterByCategory(const std::string& category) const {
    return repository.searchByCategory(category);
}

std::vector<std::shared_ptr<Product>> InventoryManager::getExpiredProducts() const {
    return repository.filter([](const std::shared_ptr<Product>& p) {
        return p && p->isExpired();
    });
}

std::vector<std::shared_ptr<Product>> InventoryManager::getExpiringSoon(int days) const {
    return repository.filter([days](const std::shared_ptr<Product>& p) {
        return p && p->isExpiringSoon(days);
    });
}

double InventoryManager::calculateTotalInventoryValue() const {
    return repository.calculateTotalInventoryValue();
}

double InventoryManager::calculateTotalInventoryCost() const {
    return calculateTotalInventoryValue();
}

int InventoryManager::getTotalProductCount() const {
    return static_cast<int>(repository.size());
}

int InventoryManager::getTotalQuantity() const {
    int total = 0;
    for (const auto& product : repository) {
        if (product) {
            total += product->getQuantity();
        }
    }
    return total;
}

void InventoryManager::writeOffProduct(int id, int quantity, const std::string& reason) {
    try {
        auto product = repository.findById(id);
        if (!product) {
            throw ProductNotFoundException("Product with ID " + std::to_string(id) + " not found");
        }
        
        if (quantity < 0 || quantity > product->getQuantity()) {
            throw NegativeQuantityException("Invalid write-off quantity");
        }
        
        auto writeOffProduct = std::make_shared<Product>(*product);
        writeOffProduct->setQuantity(quantity);
        writeOffHistory.push_back(writeOffProduct);
        
        *product -= quantity;
    } catch (const ProductException& e) {
        throw;
    }
}

void InventoryManager::sortProductsByName() {
    repository.sortByName();
}

void InventoryManager::sortProductsByPrice() {
    repository.sortByPrice();
}

void InventoryManager::sortProductsByQuantity() {
    repository.sortByQuantity();
}

void InventoryManager::sortProductsByExpirationDate() {
    repository.sortByExpirationDate();
}

void InventoryManager::sortProductsByCategory() {
    repository.sortByCategory();
}

