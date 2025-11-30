#pragma once

#include "entities/ProductRepository.h"
#include "entities/Product.h"
#include <vector>
#include <memory>

class InventoryService {
private:
    ProductRepository<Product> repository;
    std::vector<std::shared_ptr<Product>> writeOffHistory;

public:
    InventoryService();

    void addProduct(std::shared_ptr<Product> product);
    void updateProduct(int id, std::shared_ptr<Product> product);
    void deleteProduct(int id);
    std::shared_ptr<Product> getProduct(int id) const;
    std::vector<std::shared_ptr<Product>> getAllProducts() const;

    void addStock(int id, int quantity);
    void removeStock(int id, int quantity);
    
    std::vector<std::shared_ptr<Product>> searchProducts(const std::string& name) const;
    std::vector<std::shared_ptr<Product>> filterByCategory(const std::string& category) const;

    double calculateTotalInventoryValue() const;
    double calculateTotalInventoryCost() const;
    int getTotalProductCount() const;
    int getTotalQuantity() const;

    void writeOffProduct(int id, int quantity, const std::string& reason);
    std::vector<std::shared_ptr<Product>> getWriteOffHistory() const { return writeOffHistory; }

    void sortProductsByName();
    void sortProductsByPrice();
    void sortProductsByQuantity();
    void sortProductsByCategory();

    ProductRepository<Product>& getRepository() { return repository; }
    const ProductRepository<Product>& getRepository() const { return repository; }
};

