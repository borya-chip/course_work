#include "managers/DatabaseManager.h"
#include "entities/Product.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <algorithm>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <exception>

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
    ordersFilePath = dataPath + "/orders.dat";
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
    QIODevice::OpenMode appendMode = QIODevice::WriteOnly | QIODevice::Append;
    
    {
        QFile file(dataFilePath);
        if (!file.open(appendMode)) {
            qDebug() << "Cannot create/open products file:" << dataFilePath;
            return false;
    }
    }
    
    {
        QFile writeOffFile(writeOffFilePath);
        if (!writeOffFile.open(appendMode)) {
            qDebug() << "Cannot create/open write-off file:" << writeOffFilePath;
            return false;
        }
    }
    
    {
        QFile ordersFile(ordersFilePath);
        if (!ordersFile.open(appendMode)) {
            qDebug() << "Cannot create/open orders file:" << ordersFilePath;
        return false;
    }
    }
    
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
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if (!file.open(mode)) {
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
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if (!file.open(mode)) {
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
    
    // Read product type
    QString productType;
    stream >> productType;
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    
    // For backward compatibility, read expiration date if present but ignore it
    if (!stream.atEnd()) {
        QDate dummyDate;
        stream >> dummyDate;
    }
    
    // Create product and set ID
    product = Product(name.toStdString(), category.toStdString(), quantity, unitPrice);
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
    
    // Try to read write-off date for backward compatibility (if exists in old files)
    // But don't store it - we don't use dates anymore
    if (!stream.atEnd()) {
        QDate dummyDate;
        stream >> dummyDate;
        // Ignore the date - we don't use it anymore
    }
    
    return true;
}

bool DatabaseManager::addProduct(const Product& product) {
    std::vector<Product> products;
    if (!loadProducts(products)) {
        // If file doesn't exist or is empty, start with empty vector
        products.clear();
    }
    
    // Check if product already exists
    auto it = std::find_if(products.begin(), products.end(),
        [&product](const Product& p) { return p.getId() == product.getId(); });
    
    if (it != products.end()) {
        // Product already exists, update it instead
        *it = product;
    } else {
        // Add new product
        products.push_back(product);
    }
    
    return saveProducts(products);
}

bool DatabaseManager::updateProduct(const Product& product) {
    std::vector<Product> products;
    if (!loadProducts(products)) {
        qDebug() << "Failed to load products for update";
        return false;
    }
    
    auto it = std::find_if(products.begin(), products.end(),
        [&product](const Product& p) { return p.getId() == product.getId(); });
    
    if (it != products.end()) {
        *it = product;
        if (!saveProducts(products)) {
            qDebug() << "Failed to save products after update";
            return false;
        }
        return true;
    }
    
    // Product not found - this is not necessarily an error
    // It might not exist in database yet
    qDebug() << "Product with ID" << product.getId() << "not found in database for update";
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

bool DatabaseManager::addWriteOffRecord(int productId, int quantity, double value, 
                                       const QString& reason) {
    // Get product name
    Product product = getProduct(productId);
    if (product.getId() == 0) {
        qDebug() << "Product not found for write-off record";
        return false;
    }
    
    return addWriteOffRecord(productId, quantity, value, reason, QString::fromStdString(product.getName()));
}

bool DatabaseManager::addWriteOffRecord(int productId, int quantity, double value, 
                                       const QString& reason, const QString& productName) {
    try {
        std::vector<WriteOffRecord> records;
        if (!loadWriteOffRecords(records)) {
            // If file doesn't exist or can't be loaded, start with empty vector
            records.clear();
        }
        
        WriteOffRecord record;
        // Safely calculate next ID
        if (records.empty()) {
            record.id = 1;
        } else {
            int maxId = 0;
            for (const auto& r : records) {
                if (r.id > maxId) {
                    maxId = r.id;
                }
            }
            record.id = maxId + 1;
        }
        
        record.productId = productId;
        record.productName = productName.toStdString();
        record.quantity = quantity;
        record.value = value;
        record.reason = reason.isEmpty() ? QString("No reason provided") : reason;
        
        records.push_back(record);
        return saveWriteOffRecords(records);
    } catch (const std::exception& e) {
        qDebug() << "Exception in addWriteOffRecord:" << e.what();
        return false;
    } catch (...) {
        qDebug() << "Unknown exception in addWriteOffRecord";
        return false;
    }
}

std::vector<QStringList> DatabaseManager::getWriteOffHistory() {
    std::vector<WriteOffRecord> records;
    if (!loadWriteOffRecords(records)) {
        return std::vector<QStringList>();
    }
    
    // Sort by ID descending (most recent first)
    std::sort(records.begin(), records.end(),
        [](const WriteOffRecord& a, const WriteOffRecord& b) {
            return a.id > b.id;
        });
    
    std::vector<QStringList> history;
    for (const auto& record : records) {
        QStringList list;
        list << QString::number(record.id);
        list << QString::fromStdString(record.productName);
        list << QString::number(record.quantity);
        list << QString::number(record.value, 'f', 2);
        list << record.reason;
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

// Order operations
bool DatabaseManager::addOrder(const Order& order) {
    std::vector<Order> orders;
    if (!loadOrders(orders)) {
        orders.clear();
    }
    
    orders.push_back(order);
    return saveOrders(orders);
}

std::vector<Order> DatabaseManager::getAllOrders() {
    std::vector<Order> orders;
    loadOrders(orders);
    return orders;
}

std::vector<Order> DatabaseManager::getOrdersByCompany(const QString& companyName) {
    std::vector<Order> allOrders;
    if (!loadOrders(allOrders)) {
        return std::vector<Order>();
    }
    
    std::vector<Order> results;
    QString searchName = companyName.toLower();
    
    for (const auto& order : allOrders) {
        if (order.getCompanyName().toLower().contains(searchName)) {
            results.push_back(order);
        }
    }
    
    return results;
}

std::vector<Order> DatabaseManager::getOrdersByType(OrderType type) {
    std::vector<Order> allOrders;
    if (!loadOrders(allOrders)) {
        return std::vector<Order>();
    }
    
    std::vector<Order> results;
    for (const auto& order : allOrders) {
        if (order.getOrderType() == type) {
            results.push_back(order);
        }
    }
    
    return results;
}

std::vector<Order> DatabaseManager::getOrdersByDateRange(const QDate& startDate, const QDate& endDate) {
    std::vector<Order> allOrders;
    if (!loadOrders(allOrders)) {
        return std::vector<Order>();
    }
    
    std::vector<Order> results;
    for (const auto& order : allOrders) {
        QDate orderDate = order.getOrderDate();
        if (orderDate >= startDate && orderDate <= endDate) {
            results.push_back(order);
        }
    }
    
    return results;
}

bool DatabaseManager::updateOrder(const Order& order) {
    std::vector<Order> orders;
    if (!loadOrders(orders)) {
        return false;
    }
    
    auto it = std::find_if(orders.begin(), orders.end(),
        [&order](const Order& o) { return o.getId() == order.getId(); });
    
    if (it != orders.end()) {
        *it = order;
        return saveOrders(orders);
    }
    
    return false;
}

bool DatabaseManager::deleteOrder(int id) {
    std::vector<Order> orders;
    if (!loadOrders(orders)) {
        return false;
    }
    
    auto it = std::remove_if(orders.begin(), orders.end(),
        [id](const Order& o) { return o.getId() == id; });
    
    if (it != orders.end()) {
        orders.erase(it, orders.end());
        return saveOrders(orders);
    }
    
    return false;
}

Order DatabaseManager::getOrder(int id) {
    std::vector<Order> orders;
    if (!loadOrders(orders)) {
        return Order();
    }
    
    auto it = std::find_if(orders.begin(), orders.end(),
        [id](const Order& o) { return o.getId() == id; });
    
    if (it != orders.end()) {
        return *it;
    }
    
    return Order();
}

bool DatabaseManager::loadOrders(std::vector<Order>& orders) {
    orders.clear();
    QFile file(ordersFilePath);
    if (!file.exists()) {
        return true;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    if (file.size() == 0) {
        file.close();
        return true;
    }
    
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    
    Order order;
    int maxId = 0;
    while (!in.atEnd()) {
        if (readOrderFromFile(in, order)) {
            orders.push_back(order);
            if (order.getId() > maxId) {
                maxId = order.getId();
            }
        } else {
            break;
        }
    }
    
    file.close();
    Order::setNextId(maxId);
    return true;
}

bool DatabaseManager::saveOrders(const std::vector<Order>& orders) {
    QFile file(ordersFilePath);
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if (!file.open(mode)) {
        qDebug() << "Error opening orders file for writing:" << ordersFilePath;
        return false;
    }
    
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);
    
    for (const auto& order : orders) {
        writeOrderToFile(out, order);
    }
    
    file.close();
    return true;
}

void DatabaseManager::writeOrderToFile(QDataStream& stream, const Order& order) {
    // Write ID
    stream << static_cast<qint32>(order.getId());
    
    // Write company info
    stream << order.getCompanyName();
    stream << order.getContactPerson();
    stream << order.getPhone();
    
    // Write order type
    stream << static_cast<qint32>(static_cast<int>(order.getOrderType()));
    
    // Write order date
    stream << order.getOrderDate();
    
    // Write items count
    const auto& items = order.getItems();
    stream << static_cast<qint32>(items.size());
    
    // Write items
    for (const auto& item : items) {
        stream << static_cast<qint32>(item.productId);
        stream << item.productName;
        stream << item.category;
        stream << static_cast<qint32>(item.quantity);
        stream << item.unitPrice;
        stream << item.discountPercent;
        stream << item.totalPrice;
    }
    
    // Write totals
    stream << order.getTotalAmount();
    stream << order.getTotalDiscount();
}

bool DatabaseManager::readOrderFromFile(QDataStream& stream, Order& order) {
    if (stream.atEnd()) {
        return false;
    }
    
    // Read ID
    qint32 id;
    stream >> id;
    if (stream.status() != QDataStream::Ok) return false;
    
    // Read company info
    QString companyName, contactPerson, phone;
    stream >> companyName;
    if (stream.status() != QDataStream::Ok) return false;
    stream >> contactPerson;
    if (stream.status() != QDataStream::Ok) return false;
    stream >> phone;
    if (stream.status() != QDataStream::Ok) return false;
    
    // Read order type
    qint32 orderTypeInt;
    stream >> orderTypeInt;
    if (stream.status() != QDataStream::Ok) return false;
    OrderType orderType = static_cast<OrderType>(orderTypeInt);
    
    // Read order date
    QDate orderDate;
    stream >> orderDate;
    if (stream.status() != QDataStream::Ok) return false;
    
    // Create order
    order = Order(companyName, contactPerson, phone, orderType);
    order.setId(id);
    order.setOrderType(orderType);
    order.setOrderDate(orderDate);
    
    // Read items count
    qint32 itemsCount;
    stream >> itemsCount;
    if (stream.status() != QDataStream::Ok) return false;
    
    // Read items
    for (int i = 0; i < itemsCount; ++i) {
        qint32 productId;
        QString productName, category;
        qint32 quantity;
        double unitPrice, discountPercent, totalPrice;
        
        stream >> productId;
        if (stream.status() != QDataStream::Ok) return false;
        stream >> productName;
        if (stream.status() != QDataStream::Ok) return false;
        stream >> category;
        if (stream.status() != QDataStream::Ok) return false;
        stream >> quantity;
        if (stream.status() != QDataStream::Ok) return false;
        stream >> unitPrice;
        if (stream.status() != QDataStream::Ok) return false;
        stream >> discountPercent;
        if (stream.status() != QDataStream::Ok) return false;
        stream >> totalPrice;
        if (stream.status() != QDataStream::Ok) return false;
        
        OrderItem item(productId, productName, category, quantity, unitPrice);
        item.discountPercent = discountPercent;
        item.totalPrice = totalPrice;
        order.addItem(item);
    }
    
    // Read totals (for verification, but order already calculated)
    double totalAmount, totalDiscount;
    stream >> totalAmount;
    if (stream.status() != QDataStream::Ok) return false;
    stream >> totalDiscount;
    if (stream.status() != QDataStream::Ok) return false;
    
    return true;
}
