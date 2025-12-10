#pragma once

class DiscountCalculator {
public:
  static double calculateWholesaleDiscount(int quantity) {
    if (quantity >= 100) {
      return 15.0;
    } else if (quantity >= 50) {
      return 10.0;
    } else if (quantity >= 10) {
      return 5.0;
    }
    return 0.0;
  }

  static double calculateRetailDiscount(int quantity) {
    Q_UNUSED(quantity);
    return 0.0;
  }

  static double calculateDiscount(bool isWholesale, int quantity) {
    if (isWholesale) {
      return calculateWholesaleDiscount(quantity);
    } else {
      return calculateRetailDiscount(quantity);
    }
  }
};
