# C++ Topics Demonstration Guide

This document maps each required C++ topic to specific code locations in the project.

## 1. Classes & Objects with Encapsulation

**Location:**
- `include/Product.h` and `src/Product.cpp`
- `include/InventoryManager.h` and `src/InventoryManager.cpp`
- `include/WriteOffCalculator.h` and `src/WriteOffCalculator.cpp`

**Demonstration:**
- All classes have private/protected data members
- Public getters and setters for controlled access
- Encapsulation principles followed throughout

**Example:**
```cpp
class Product : public AbstractProduct {
private:
    int id;  // Encapsulated data
public:
    int getId() const { return id; }  // Public getter
};
```

## 2. Friend Functions & Operator Overloading

**Location:**
- `include/AbstractProduct.h` - Friend function declaration
- `include/Product.h` - Operator overloads and friend functions
- `src/AbstractProduct.cpp` - Friend function implementation
- `src/Product.cpp` - Operator implementations

**Demonstrations:**

### Friend Functions:
- `calculateDiscountedPrice()` in `AbstractProduct.h` (line 44)
- `operator<<()` and `operator>>()` for Product I/O

### Operator Overloading:
- `Product operator+(int)` - Addition operator (line 33)
- `Product& operator+=(int)` - Compound assignment (line 34)
- `Product operator-(int)` - Subtraction operator (line 35)
- `Product& operator-=(int)` - Compound assignment (line 36)
- `bool operator==()` - Equality comparison (line 37)
- `bool operator!=()` - Inequality comparison (line 38)
- `bool operator<()` - Less-than for sorting (line 39)
- `std::ostream& operator<<()` - Stream output (line 42)
- `std::istream& operator>>()` - Stream input (line 43)
- `QTextStream& operator<<()` - Qt stream output (line 44)

**Example:**
```cpp
// Friend function
friend double calculateDiscountedPrice(const AbstractProduct& product, double discount);

// Operator overloading
Product operator+(int quantity) const;
friend std::ostream& operator<<(std::ostream& os, const Product& product);
```

## 3. Inheritance

**Location:**
- `include/AbstractProduct.h` - Base class
- `include/Product.h` - Derived class
- `include/FoodProduct.h` and `src/FoodProduct.cpp` - Derived class
- `include/NonFoodProduct.h` and `src/NonFoodProduct.cpp` - Derived class

**Demonstration:**
- `AbstractProduct` is the base class
- `Product`, `FoodProduct`, and `NonFoodProduct` inherit from `AbstractProduct`
- Protected members accessed by derived classes
- Constructor initialization lists

**Example:**
```cpp
class AbstractProduct {
protected:
    std::string name;
    // ... other protected members
};

class Product : public AbstractProduct {
    // Inherits protected members
};

class FoodProduct : public AbstractProduct {
    // Inherits from AbstractProduct
};
```

## 4. Virtual Functions & Abstract Classes

**Location:**
- `include/AbstractProduct.h` - Abstract base class definition
- `src/AbstractProduct.cpp` - Virtual function implementations
- `src/Product.cpp` - Pure virtual override
- `src/FoodProduct.cpp` - Pure virtual override
- `src/NonFoodProduct.cpp` - Pure virtual override

**Demonstration:**
- `AbstractProduct` is an abstract class (cannot be instantiated)
- Pure virtual function: `calculateTotalValue() = 0`
- Virtual functions: `getProductType()`, `isExpired()`, `isExpiringSoon()`
- Each derived class implements `calculateTotalValue()` differently
- Polymorphism demonstrated through virtual function calls

**Example:**
```cpp
class AbstractProduct {
public:
    virtual double calculateTotalValue() const = 0;  // Pure virtual
    virtual std::string getProductType() const = 0;   // Pure virtual
    virtual bool isExpired() const;                  // Virtual
};

class Product : public AbstractProduct {
    double calculateTotalValue() const override;  // Implements pure virtual
    std::string getProductType() const override;
};
```

## 5. Templates

**Location:**
- `include/ProductRepository.h` - Template class definition
- `include/ProductRepository.tpp` - Template implementation
- Used in `InventoryManager` class

**Demonstration:**
- Template class: `ProductRepository<T>`
- Template function: `filter<Predicate>()` with predicate support
- Generic programming demonstrated
- STL algorithms used with templates

**Example:**
```cpp
template<typename T>
class ProductRepository {
    template<typename Predicate>
    std::vector<std::shared_ptr<T>> filter(Predicate pred) const {
        std::vector<std::shared_ptr<T>> result;
        std::copy_if(products.begin(), products.end(), 
                    std::back_inserter(result), pred);
        return result;
    }
};
```

## 6. Exception Handling

**Location:**
- `include/Exceptions.h` - Custom exception class hierarchy
- `src/AbstractProduct.cpp` - Exception throwing in setters
- `src/Product.cpp` - Exception throwing in operators
- `src/InventoryManager.cpp` - Exception handling
- `src/MainWindow.cpp` - Try-catch blocks in UI

**Demonstration:**
- Custom exception hierarchy:
  - `ProductException` (base)
  - `NegativeQuantityException`
  - `InvalidPriceException`
  - `InvalidDateException`
  - `ProductNotFoundException`
- Try-catch blocks in critical operations
- Exception propagation through layers
- Exception handling in UI layer

**Example:**
```cpp
// Custom exception
class NegativeQuantityException : public ProductException {
public:
    explicit NegativeQuantityException(const std::string& message = "Quantity cannot be negative")
        : ProductException(message) {}
};

// Exception throwing
void setQuantity(int quantity) {
    if (quantity < 0) {
        throw NegativeQuantityException("Quantity cannot be negative");
    }
    this->quantity = quantity;
}

// Exception handling
try {
    inventoryManager->addProduct(productPtr);
} catch (const ProductException& e) {
    QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
}
```

## 7. I/O Streams & Files

**Location:**
- `include/FileManager.h` and `src/FileManager.cpp` - File operations
- `src/Product.cpp` - Stream operators
- JSON/XML save/load functions
- Text file export functions

**Demonstration:**
- Standard I/O streams: `std::ostream`, `std::istream`
- Qt streams: `QTextStream`
- File I/O: `std::ofstream`, `QFile`
- JSON parsing: `QJsonDocument`, `QJsonObject`, `QJsonArray`
- XML parsing: `QXmlStreamWriter`, `QXmlStreamReader`
- Text file export

**Example:**
```cpp
// Stream operators
std::ostream& operator<<(std::ostream& os, const Product& product);
std::istream& operator>>(std::istream& is, Product& product);

// File I/O
bool saveToJson(const InventoryManager& inventory, const std::string& filename);
bool loadFromJson(InventoryManager& inventory, const std::string& filename);
bool saveToXml(const InventoryManager& inventory, const std::string& filename);
bool exportReportToText(const InventoryManager& inventory, const std::string& filename);
```

## 8. STL Containers & Iterators

**Location:**
- `include/ProductRepository.h` - Container usage
- `include/ProductRepository.tpp` - Iterator and algorithm usage
- `src/InventoryManager.cpp` - Container operations
- Throughout the codebase

**Demonstration:**
- `std::vector` - Sequential storage
- `std::map` - Fast lookup by ID
- Iterators: `begin()`, `end()`, `cbegin()`, `cend()`
- STL Algorithms:
  - `std::sort()` - Sorting products
  - `std::copy_if()` - Filtering products
  - `std::remove_if()` - Removing products
  - `std::find_if()` - Finding products

**Example:**
```cpp
// Containers
std::vector<std::shared_ptr<T>> products;
std::map<int, std::shared_ptr<T>> productMap;

// Iterators
using iterator = typename std::vector<std::shared_ptr<T>>::iterator;
iterator begin() { return products.begin(); }
iterator end() { return products.end(); }

// STL Algorithms
std::sort(products.begin(), products.end(), comparator);
std::copy_if(products.begin(), products.end(), 
             std::back_inserter(result), predicate);
```

## Qt Specific Requirements

### Model-View Architecture
**Location:** `include/ProductModel.h` and `src/ProductModel.cpp`
- Custom `QAbstractTableModel` subclass
- Model-View separation

### Signals and Slots
**Location:** `include/MainWindow.h` and `src/MainWindow.cpp`
- Signal-slot connections throughout
- Custom signals for product events

### Database Integration
**Location:** `include/DatabaseManager.h` and `src/DatabaseManager.cpp`
- `QSqlDatabase` for SQLite connection
- `QSqlQuery` for database operations
- Database abstraction layer

### UI Components
**Location:** `include/MainWindow.h`, `include/ProductDialog.h`, etc.
- `QMainWindow` - Main application window
- `QTableView` - Product display
- `QFormLayout` - Form layouts
- `QMenu`, `QToolBar`, `QStatusBar` - Navigation
- Dialog windows for data entry

## Architecture Patterns

### MVC Pattern
- **Model:** `ProductModel`, `InventoryManager`, `DatabaseManager`
- **View:** `MainWindow`, `ProductDialog`, `WriteOffDialog`, `ReportDialog`
- **Controller:** Signal-slot connections in `MainWindow`

### Singleton Pattern
- `DatabaseManager` uses singleton pattern for database access

### Repository Pattern
- `ProductRepository<T>` provides data access abstraction

## Summary

All required C++ topics are comprehensively demonstrated throughout the codebase:

1. ✅ Classes & Objects - Throughout (Product, InventoryManager, etc.)
2. ✅ Friend Functions - AbstractProduct.h, Product.h
3. ✅ Operator Overloading - Product.h, Product.cpp (10+ operators)
4. ✅ Inheritance - AbstractProduct → Product, FoodProduct, NonFoodProduct
5. ✅ Virtual Functions - AbstractProduct with pure virtual functions
6. ✅ Templates - ProductRepository<T> template class
7. ✅ Exception Handling - Custom exception hierarchy with try-catch
8. ✅ I/O Streams - JSON, XML, text file operations
9. ✅ STL Containers - vector, map with iterators and algorithms

All Qt requirements are also met:
- ✅ Modern UI components
- ✅ Database integration
- ✅ Signals and slots
- ✅ Model-View architecture
- ✅ Complete dialog system

