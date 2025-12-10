#include "entities/Product.h"
#include "exceptions/Exceptions.h"
#include <QString>
#include <QTextStream>
#include <iomanip>
#include <sstream>


int Product::nextId = 1;

void Product::setNextId(int id) { nextId = id + 1; }

Product::Product(const std::string &name, const std::string &category,
                 int quantity, double unitPrice)
    : AbstractProduct(name, category, quantity, unitPrice), id(0) {}

Product::Product(const Product &other)
    : AbstractProduct(other.name, other.category, other.quantity,
                      other.unitPrice),
      id(other.id) {}

Product &Product::operator=(const Product &other) {
  if (this != &other) {
    name = other.name;
    category = other.category;
    quantity = other.quantity;
    unitPrice = other.unitPrice;
    id = other.id;
  }
  return *this;
}

double Product::calculateTotalValue() const { return quantity * unitPrice; }

std::string Product::getProductType() const { return "Standard Product"; }

Product Product::operator+(int qty) const {
  Product result(*this);
  result.quantity += qty;
  if (result.quantity < 0) {
    throw NegativeQuantityException("Resulting quantity cannot be negative");
  }
  return result;
}

Product &Product::operator+=(int qty) {
  quantity += qty;
  if (quantity < 0) {
    throw NegativeQuantityException("Resulting quantity cannot be negative");
  }
  return *this;
}

Product Product::operator-(int qty) const {
  Product result(*this);
  result.quantity -= qty;
  if (result.quantity < 0) {
    throw NegativeQuantityException("Resulting quantity cannot be negative");
  }
  return result;
}

Product &Product::operator-=(int qty) {
  if (qty < 0) {
    throw NegativeQuantityException("Cannot subtract negative quantity");
  }
  quantity -= qty;
  if (quantity < 0) {
    quantity += qty;
    throw NegativeQuantityException("Resulting quantity cannot be negative");
  }
  return *this;
}

bool Product::operator==(const Product &other) const { return id == other.id; }

bool Product::operator!=(const Product &other) const {
  return !(*this == other);
}

bool Product::operator<(const Product &other) const {
  return name < other.name;
}

std::ostream &operator<<(std::ostream &os, const Product &product) {
  os << "Product{"
     << "id=" << product.id << ", "
     << "name=" << product.name << ", "
     << "category=" << product.category << ", "
     << "quantity=" << product.quantity << ", "
     << "unitPrice=" << std::fixed << std::setprecision(2) << product.unitPrice
     << ", "
     << "totalValue=" << product.calculateTotalValue() << "}";
  return os;
}

std::istream &operator>>(std::istream &is, Product &product) {
  std::string line;
  std::getline(is, line);

  std::istringstream iss(line);
  std::string token;

  if (std::getline(iss, token, '|'))
    product.name = token;
  if (std::getline(iss, token, '|'))
    product.category = token;
  if (std::getline(iss, token, '|'))
    product.quantity = std::stoi(token);
  if (std::getline(iss, token, '|'))
    product.unitPrice = std::stod(token);

  return is;
}

QTextStream &operator<<(QTextStream &stream, const Product &product) {
  stream << "ID: " << product.id << "\n"
         << "Name: " << QString::fromStdString(product.name) << "\n"
         << "Category: " << QString::fromStdString(product.category) << "\n"
         << "Quantity: " << product.quantity << "\n"
         << "Unit Price: $" << QString::number(product.unitPrice, 'f', 2)
         << "\n"
         << "Total Value: $"
         << QString::number(product.calculateTotalValue(), 'f', 2) << "\n";
  return stream;
}
