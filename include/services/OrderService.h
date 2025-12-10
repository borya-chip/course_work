#pragma once

#include "entities/Order.h"
#include "services/InventoryService.h"
#include "managers/DatabaseManager.h"

// Сервис, инкапсулирующий бизнес-логику создания заказа
class OrderService {
public:
    struct Result {
        bool saved = false;        // заказ сохранён в БД
        double totalAmount = 0.0;  // итоговая сумма заказа
    };

    // Создаёт заказ:
    //  - сохраняет его через DatabaseManager
    //  - при успехе обновляет остатки товаров в InventoryService
    // Файловая запись (inventory.dat, history txt) и UI остаются на стороне MainWindow.
    static Result createOrder(InventoryService& inventory,
                              DatabaseManager& db,
                              const Order& order);
};
