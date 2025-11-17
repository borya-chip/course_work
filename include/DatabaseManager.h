#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QDate>
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <vector>
#include "Product.h"

struct WriteOffRecord {
    int id;
    int productId;
    std::string productName;
    int quantity;
    double value;
    QString reason;
    QDate writeOffDate;
    
    WriteOffRecord() : id(0), productId(0), quantity(0), value(0.0) {}
};

class DatabaseManager {
private:
    static DatabaseManager* instance;
    QString dataFilePath;
    QString writeOffFilePath;
    
    DatabaseManager();
    ~DatabaseManager();

public:
    // Singleton pattern
    static DatabaseManager* getInstance();
    static void destroyInstance();

    // Database operations
    bool initializeDatabase();
    bool connect();
    void disconnect();
    bool isConnected() const;

    // Product CRUD operations
    bool addProduct(const Product& product);
    bool updateProduct(const Product& product);
    bool deleteProduct(int id);
    Product getProduct(int id);
    std::vector<Product> getAllProducts();
    
    // Search operations
    std::vector<Product> searchProductsByName(const QString& name);
    std::vector<Product> searchProductsByCategory(const QString& category);
    std::vector<Product> getExpiredProducts();
    std::vector<Product> getExpiringSoon(int days = 30);
    
    // Write-off operations
    bool addWriteOffRecord(int productId, int quantity, double value, 
                          const QString& reason);
    std::vector<QStringList> getWriteOffHistory();

private:
    bool loadProducts(std::vector<Product>& products);
    bool saveProducts(const std::vector<Product>& products);
    bool loadWriteOffRecords(std::vector<WriteOffRecord>& records);
    bool saveWriteOffRecords(const std::vector<WriteOffRecord>& records);
    void writeProductToFile(QDataStream& stream, const Product& product);
    bool readProductFromFile(QDataStream& stream, Product& product);
    void writeWriteOffRecordToFile(QDataStream& stream, const WriteOffRecord& record);
    bool readWriteOffRecordFromFile(QDataStream& stream, WriteOffRecord& record);
    QString dateToString(const QDate& date);
    QDate stringToDate(const QString& dateString);
};

#endif // DATABASEMANAGER_H

