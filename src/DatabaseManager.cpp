#include "DatabaseManager.h"
#include "Product.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <algorithm>
#include <vector>

DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager::DatabaseManager() {
    // Set data file paths
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath)) {
        dir.mkpath(dataPath);
    }
    dataFilePath = dataPath + "/products.dat";
    writeOffFilePath = dataPath + "/writeoff.dat";
}

DatabaseManager::~DatabaseManager() {
    // Nothing to do - files are closed automatically
}

DatabaseManager* DatabaseManager::getInstance() {
    if (!instance) {
        instance = new DatabaseManager();
    }
    return instance;
}

void DatabaseManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

bool DatabaseManager::initializeDatabase() {
    return connect();
}

bool DatabaseManager::connect() {
    // For binary file, "connect" just means files are accessible
    // Create empty files if they don't exist
    QFile file(dataFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qDebug() << "Cannot create/open products file:" << dataFilePath;
        return false;
    }
    file.close();
    
    QFile writeOffFile(writeOffFilePath);
    if (!writeOffFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qDebug() << "Cannot create/open write-off file:" << writeOffFilePath;
        return false;
    }
    writeOffFile.close();
    
    return true;
}

void DatabaseManager::disconnect() {
    // Nothing to do for binary files
}

bool DatabaseManager::isConnected() const {
    // Check if files exist and are accessible
    QFileInfo info(dataFilePath);
    return info.exists() || info.dir().exists();
}

bool DatabaseManager::loadProducts(std::vector<Product>& products) {
    products.clear();
    QFile file(dataFilePath);
    if (!file.exists()) {
        // File doesn't exist yet, return empty vector
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    // Check if file is empty
    if (file.size() == 0) {
        file.close();
        return true;
    }
    
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    
    Product product;
    int maxId = 0;
    while (!in.atEnd()) {
        if (readProductFromFile(in, product)) {
            products.push_back(product);
            if (product.getId() > maxId) {
                maxId = product.getId();
            }
        } else {
            break;
        }
    }
    
    file.close();
    Product::setNextId(maxId);
    return true;
}

bool DatabaseManager::saveProducts(const std::vector<Product>& products) {
    QFile file(dataFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Error opening products file for writing:" << dataFilePath;
        return false;
    }
    
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);
    
    for (const auto& product : products) {
        writeProductToFile(out, product);
    }
    
    file.close();
    return true;
}

bool DatabaseManager::loadWriteOffRecords(std::vector<WriteOffRecord>& records) {
    records.clear();
    QFile file(writeOffFilePath);
    if (!file.exists()) {
        // File doesn't exist yet, return empty vector
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    // Check if file is empty
    if (file.size() == 0) {
        file.close();
        return true;
    }
    
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    
    WriteOffRecord record;
    while (!in.atEnd()) {
        if (readWriteOffRecordFromFile(in, record)) {
            records.push_back(record);
        } else {
            break;
        }
    }
    
    file.close();
    return true;
}

bool DatabaseManager::saveWriteOffRecords(const std::vector<WriteOffRecord>& records) {
    QFile file(writeOffFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Error opening write-off file for writing:" << writeOffFilePath;
        return false;
    }
    
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);
    
    for (const auto& record : records) {
        writeWriteOffRecordToFile(out, record);
    }
    
    file.close();
    return true;
}

void DatabaseManager::writeProductToFile(QDataStream& stream, const Product& product) {
    // Write ID
    stream << static_cast<qint32>(product.getId());
    
    // Write name
    QString name = QString::fromStdString(product.getName());
    stream << name;
    
    // Write category
    QString category = QString::fromStdString(product.getCategory());
    stream << category;
    
    // Write quantity
    stream << static_cast<qint32>(product.getQuantity());
    
    // Write unit price
    stream << product.getUnitPrice();
    
    // Write expiration date
    stream << product.getExpirationDate();
    
    // Write product type
    QString productType = QString::fromStdString(product.getProductType());
    stream << productType;
}

bool DatabaseManager::readProductFromFile(QDataStream& stream, Product& product) {
    if (stream.atEnd()) {
        return false;
    }
    
    // Read ID
    qint32 id;
    stream >> id;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // Read name
    QString name;
    stream >> name;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // Read category
    QString category;
    stream >> category;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // Read quantity
    qint32 quantity;
    stream >> quantity;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // Read unit price
    double unitPrice;
    stream >> unitPrice;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // Read expiration date
    QDate expDate;
    stream >> expDate;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // Read product type
    QString productType;
    stream >> productType;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // Create product and set ID
    product = Product(name.toStdString(), category.toStdString(), quantity, unitPrice, expDate);
    product.setId(id);
    
    return true;
}

void DatabaseManager::writeWriteOffRecordToFile(QDataStream& stream, const WriteOffRecord& record) {
    // Write ID
    stream << static_cast<qint32>(record.id);
    
    // Write product ID
    stream << static_cast<qint32>(record.productId);
    
    // Write product name
    QString name = QString::fromStdString(record.productName);
    stream << name;
    
    // Write quantity
    stream << static_cast<qint32>(record.quantity);
    
    // Write value
    stream << record.value;
    
    // Write reason
    stream << record.reason;
    
    // Write write-off date
    stream << record.writeOffDate;
}

bool DatabaseManager::readWriteOffRecordFromFile(QDataStream& stream, WriteOffRecord& record) {
    if (stream.atEnd()) {
        return false;
    }
    
    // Read ID
    qint32 id;
    stream >> id;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    record.id = id;
    
    // Read product ID
    qint32 productId;
    stream >> productId;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    record.productId = productId;
    
    // Read product name
    QString name;
    stream >> name;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    record.productName = name.toStdString();
    
    // Read quantity
    qint32 quantity;
    stream >> quantity;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    record.quantity = quantity;
    
    // Read value
    double value;
    stream >> value;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    record.value = value;
    
    // Read reason
    QString reason;
    stream >> reason;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    record.reason = reason;
    
    // Read write-off date
    QDate writeOffDate;
    stream >> writeOffDate;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    record.writeOffDate = writeOffDate;
    
    return true;
}

bool DatabaseManager::addProduct(const Product& product) {
    std::vector<Product> products;
    if (!loadProducts(products)) {
        // If file doesn't exist or is empty, start with empty vector
        products.clear();
    }
    
    products.push_back(product);
    return saveProducts(products);
}

bool DatabaseManager::updateProduct(const Product& product) {
    std::vector<Product> products;
    if (!loadProducts(products)) {
        return false;
    }
    
    auto it = std::find_if(products.begin(), products.end(),
        [&product](const Product& p) { return p.getId() == product.getId(); });
    
    if (it != products.end()) {
        *it = product;
        return saveProducts(products);
    }
    
    return false;
}

bool DatabaseManager::deleteProduct(int id) {
    std::vector<Product> products;
    if (!loadProducts(products)) {
        return false;
    }
    
    auto it = std::remove_if(products.begin(), products.end(),
        [id](const Product& p) { return p.getId() == id; });
    
    if (it != products.end()) {
        products.erase(it, products.end());
        return saveProducts(products);
    }
    
    return false;
}

Product DatabaseManager::getProduct(int id) {
    std::vector<Product> products;
    if (!loadProducts(products)) {
        return Product();
    }
    
    auto it = std::find_if(products.begin(), products.end(),
        [id](const Product& p) { return p.getId() == id; });
    
    if (it != products.end()) {
        return *it;
    }
    
    return Product();
}

std::vector<Product> DatabaseManager::getAllProducts() {
    std::vector<Product> products;
    loadProducts(products);
    return products;
}

std::vector<Product> DatabaseManager::searchProductsByName(const QString& name) {
    std::vector<Product> allProducts;
    if (!loadProducts(allProducts)) {
        return std::vector<Product>();
    }
    
    std::vector<Product> results;
    QString searchName = name.toLower();
    
    for (const auto& product : allProducts) {
        QString productName = QString::fromStdString(product.getName()).toLower();
        if (productName.contains(searchName)) {
            results.push_back(product);
        }
    }
    
    return results;
}

std::vector<Product> DatabaseManager::searchProductsByCategory(const QString& category) {
    std::vector<Product> allProducts;
    if (!loadProducts(allProducts)) {
        return std::vector<Product>();
    }
    
    std::vector<Product> results;
    QString searchCategory = category.toLower();
    
    for (const auto& product : allProducts) {
        QString productCategory = QString::fromStdString(product.getCategory()).toLower();
        if (productCategory == searchCategory) {
            results.push_back(product);
        }
    }
    
    return results;
}

std::vector<Product> DatabaseManager::getExpiredProducts() {
    std::vector<Product> allProducts;
    if (!loadProducts(allProducts)) {
        return std::vector<Product>();
    }
    
    std::vector<Product> results;
    QDate currentDate = QDate::currentDate();
    
    for (const auto& product : allProducts) {
        if (product.getExpirationDate() < currentDate) {
            results.push_back(product);
        }
    }
    
    return results;
}

std::vector<Product> DatabaseManager::getExpiringSoon(int days) {
    std::vector<Product> allProducts;
    if (!loadProducts(allProducts)) {
        return std::vector<Product>();
    }
    
    std::vector<Product> results;
    QDate currentDate = QDate::currentDate();
    QDate futureDate = currentDate.addDays(days);
    
    for (const auto& product : allProducts) {
        QDate expDate = product.getExpirationDate();
        if (expDate >= currentDate && expDate <= futureDate) {
            results.push_back(product);
        }
    }
    
    return results;
}

bool DatabaseManager::addWriteOffRecord(int productId, int quantity, double value, 
                                       const QString& reason) {
    // Get product name
    Product product = getProduct(productId);
    if (product.getId() == 0) {
        qDebug() << "Product not found for write-off record";
        return false;
    }
    
    std::vector<WriteOffRecord> records;
    loadWriteOffRecords(records);
    
    WriteOffRecord record;
    record.id = records.empty() ? 1 : (records.back().id + 1);
    record.productId = productId;
    record.productName = product.getName();
    record.quantity = quantity;
    record.value = value;
    record.reason = reason;
    record.writeOffDate = QDate::currentDate();
    
    records.push_back(record);
    return saveWriteOffRecords(records);
}

std::vector<QStringList> DatabaseManager::getWriteOffHistory() {
    std::vector<WriteOffRecord> records;
    if (!loadWriteOffRecords(records)) {
        return std::vector<QStringList>();
    }
    
    // Sort by date descending
    std::sort(records.begin(), records.end(),
        [](const WriteOffRecord& a, const WriteOffRecord& b) {
            return a.writeOffDate > b.writeOffDate;
        });
    
    std::vector<QStringList> history;
    for (const auto& record : records) {
        QStringList list;
        list << QString::number(record.id);
        list << QString::fromStdString(record.productName);
        list << QString::number(record.quantity);
        list << QString::number(record.value, 'f', 2);
        list << record.reason;
        list << dateToString(record.writeOffDate);
        history.push_back(list);
    }
    
    return history;
}

QDate DatabaseManager::stringToDate(const QString& dateString) {
    return QDate::fromString(dateString, Qt::ISODate);
}

QString DatabaseManager::dateToString(const QDate& date) {
    return date.toString(Qt::ISODate);
}
