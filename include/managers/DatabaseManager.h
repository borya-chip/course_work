#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "entities/Order.h"
#include "entities/Product.h"
#include <QDataStream>
#include <QDate>
#include <QFile>
#include <QString>
#include <QStringList>
#include <string>
#include <vector>


struct WriteOffRecord {
  int id;
  int productId;
  std::string productName;
  int quantity;
  double value;
  QString reason;

  WriteOffRecord() : id(0), productId(0), quantity(0), value(0.0) {}
};

class DatabaseManager {
private:
  static DatabaseManager *instance;
  QString dataFilePath;
  QString writeOffFilePath;
  QString ordersFilePath;

  DatabaseManager();
  ~DatabaseManager();

public:
  static DatabaseManager *getInstance();
  static void destroyInstance();

  bool initializeDatabase();
  bool connect();
  void disconnect();
  bool isConnected() const;

  bool addProduct(const Product &product);
  bool updateProduct(const Product &product);
  bool deleteProduct(int id);
  Product getProduct(int id);
  std::vector<Product> getAllProducts();

  std::vector<Product> searchProductsByName(const QString &name);
  std::vector<Product> searchProductsByCategory(const QString &category);

  bool addWriteOffRecord(int productId, int quantity, double value,
                         const QString &reason);
  bool addWriteOffRecord(int productId, int quantity, double value,
                         const QString &reason, const QString &productName);
  std::vector<QStringList> getWriteOffHistory();

  bool addOrder(const Order &order);
  bool updateOrder(const Order &order);
  bool deleteOrder(int id);
  Order getOrder(int id);
  std::vector<Order> getAllOrders();
  std::vector<Order> getOrdersByCompany(const QString &companyName);
  std::vector<Order> getOrdersByType(OrderType type);
  std::vector<Order> getOrdersByDateRange(const QDate &startDate,
                                          const QDate &endDate);

private:
  bool loadProducts(std::vector<Product> &products);
  bool saveProducts(const std::vector<Product> &products);
  bool loadWriteOffRecords(std::vector<WriteOffRecord> &records);
  bool saveWriteOffRecords(const std::vector<WriteOffRecord> &records);
  void writeProductToFile(QDataStream &stream, const Product &product);
  bool readProductFromFile(QDataStream &stream, Product &product);
  void writeWriteOffRecordToFile(QDataStream &stream,
                                 const WriteOffRecord &record);
  bool readWriteOffRecordFromFile(QDataStream &stream, WriteOffRecord &record);
  bool loadOrders(std::vector<Order> &orders);
  bool saveOrders(const std::vector<Order> &orders);
  void writeOrderToFile(QDataStream &stream, const Order &order);
  bool readOrderFromFile(QDataStream &stream, Order &order);
  QString dateToString(const QDate &date);
  QDate stringToDate(const QString &dateString);
};

#endif
