#pragma once

#include "entities/Product.h"
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <string>

template<typename T>
class ProductRepository {
private:
    std::vector<std::shared_ptr<T>> products;
    std::map<int, std::shared_ptr<T>> productMap;

public:
    void add(std::shared_ptr<T> product);
    void remove(int id);
    std::shared_ptr<T> findById(int id) const;
    std::vector<std::shared_ptr<T>> findAll() const { return products; }
    
    template<typename Predicate>
    std::vector<std::shared_ptr<T>> filter(Predicate pred) const {
        std::vector<std::shared_ptr<T>> result;
        std::copy_if(products.begin(), products.end(), 
                    std::back_inserter(result), pred);
        return result;
    }

    void sortByName();
    void sortByPrice();
    void sortByQuantity();
    void sortByCategory();

    std::vector<std::shared_ptr<T>> searchByName(const std::string& name) const;
    std::vector<std::shared_ptr<T>> searchByCategory(const std::string& category) const;

    using iterator = typename std::vector<std::shared_ptr<T>>::iterator;
    using const_iterator = typename std::vector<std::shared_ptr<T>>::const_iterator;
    
    iterator begin() { return products.begin(); }
    iterator end() { return products.end(); }
    const_iterator begin() const { return products.begin(); }
    const_iterator end() const { return products.end(); }
    const_iterator cbegin() const { return products.cbegin(); }
    const_iterator cend() const { return products.cend(); }

    size_t size() const { return products.size(); }
    bool empty() const { return products.empty(); }
    void clear();

    double calculateTotalInventoryValue() const;
};

#include "ProductRepository.tpp"

