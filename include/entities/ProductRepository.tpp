#pragma once

#include "entities/ProductRepository.h"
#include <algorithm>

template<typename T>
void ProductRepository<T>::add(std::shared_ptr<T> product) {
    if (auto* p = dynamic_cast<Product*>(product.get())) {
        products.push_back(product);
        productMap[p->getId()] = product;
    }
}

template<typename T>
void ProductRepository<T>::remove(int id) {
    auto it = std::remove_if(products.begin(), products.end(),
        [id](const std::shared_ptr<T>& p) {
            if (auto* product = dynamic_cast<Product*>(p.get())) {
                return product->getId() == id;
            }
            return false;
        });
    products.erase(it, products.end());
    productMap.erase(id);
}

template<typename T>
std::shared_ptr<T> ProductRepository<T>::findById(int id) const {
    auto it = productMap.find(id);
    if (it != productMap.end()) {
        return it->second;
    }
    return nullptr;
}

template<typename T>
void ProductRepository<T>::sortByName() {
    std::sort(products.begin(), products.end(),
        [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
            if (auto* pa = dynamic_cast<Product*>(a.get())) {
                if (auto* pb = dynamic_cast<Product*>(b.get())) {
                    return pa->getName() < pb->getName();
                }
            }
            return false;
        });
}

template<typename T>
void ProductRepository<T>::sortByPrice() {
    std::sort(products.begin(), products.end(),
        [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
            if (auto* pa = dynamic_cast<Product*>(a.get())) {
                if (auto* pb = dynamic_cast<Product*>(b.get())) {
                    return pa->getUnitPrice() < pb->getUnitPrice();
                }
            }
            return false;
        });
}

template<typename T>
void ProductRepository<T>::sortByQuantity() {
    std::sort(products.begin(), products.end(),
        [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
            if (auto* pa = dynamic_cast<Product*>(a.get())) {
                if (auto* pb = dynamic_cast<Product*>(b.get())) {
                    return pa->getQuantity() < pb->getQuantity();
                }
            }
            return false;
        });
}

template<typename T>
void ProductRepository<T>::sortByCategory() {
    std::sort(products.begin(), products.end(),
        [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
            if (auto* pa = dynamic_cast<Product*>(a.get())) {
                if (auto* pb = dynamic_cast<Product*>(b.get())) {
                    return pa->getCategory() < pb->getCategory();
                }
            }
            return false;
        });
}

template<typename T>
std::vector<std::shared_ptr<T>> ProductRepository<T>::searchByName(const std::string& name) const {
    return filter([&name](const std::shared_ptr<T>& p) {
        if (auto* product = dynamic_cast<Product*>(p.get())) {
            return product->getName().find(name) != std::string::npos;
        }
        return false;
    });
}

template<typename T>
std::vector<std::shared_ptr<T>> ProductRepository<T>::searchByCategory(const std::string& category) const {
    return filter([&category](const std::shared_ptr<T>& p) {
        if (auto* product = dynamic_cast<Product*>(p.get())) {
            return product->getCategory() == category;
        }
        return false;
    });
}

template<typename T>
void ProductRepository<T>::clear() {
    products.clear();
    productMap.clear();
}

template<typename T>
double ProductRepository<T>::calculateTotalInventoryValue() const {
    double total = 0.0;
    for (const auto& product : products) {
        if (auto* p = dynamic_cast<Product*>(product.get())) {
            total += p->calculateTotalValue();
        }
    }
    return total;
}

