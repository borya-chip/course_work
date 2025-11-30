#pragma once

#include "entities/Product.h"
#include "services/InventoryService.h"
#include <vector>
#include <string>
#include <memory>

class FileManager {
public:
    static bool saveToBinary(const InventoryService& inventory, const std::string& filename);
    
    static bool loadFromBinary(InventoryService& inventory, const std::string& filename);
    
    static bool exportReportToText(const InventoryService& inventory, const std::string& filename);
    
    static bool exportWriteOffHistoryToText(const InventoryService& inventory, 
                                           const std::string& filename);
};

