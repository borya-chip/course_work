#pragma once

#include "Product.h"
#include "InventoryManager.h"
#include <vector>
#include <string>
#include <memory>

class FileManager {
public:
    static bool saveToBinary(const InventoryManager& inventory, const std::string& filename);
    
    static bool loadFromBinary(InventoryManager& inventory, const std::string& filename);
    
    static bool exportReportToText(const InventoryManager& inventory, const std::string& filename);
    
    static bool exportWriteOffHistoryToText(const InventoryManager& inventory, 
                                           const std::string& filename);
};

