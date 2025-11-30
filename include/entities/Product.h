#pragma once

#include "entities/AbstractProduct.h"
#include <iostream>
#include <fstream>
#include <QTextStream>

class ProductDialog;
class FileManager;

class Product : public AbstractProduct {
    friend class ProductDialog;
    friend class FileManager;
    friend class DatabaseManager;
    
private:
    int id;
    static int nextId;
    
    void setId(int newId) { id = newId; }

public:
    Product(const std::string& name = "", const std::string& category = "", 
           int quantity = 0, double unitPrice = 0.0);
    
    Product(const Product& other);
    Product& operator=(const Product& other);

    double calculateTotalValue() const override;
    std::string getProductType() const override;

    int getId() const { return id; }
    
    static void setNextId(int maxId);

    Product operator+(int quantity) const;
    Product& operator+=(int quantity);
    Product operator-(int quantity) const;
    Product& operator-=(int quantity);
    bool operator==(const Product& other) const;
    bool operator!=(const Product& other) const;
    bool operator<(const Product& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Product& product);
    friend std::istream& operator>>(std::istream& is, Product& product);
    friend QTextStream& operator<<(QTextStream& stream, const Product& product);
};

std::ostream& operator<<(std::ostream& os, const Product& product);
std::istream& operator>>(std::istream& is, Product& product);
QTextStream& operator<<(QTextStream& stream, const Product& product);

