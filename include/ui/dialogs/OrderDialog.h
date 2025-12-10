#pragma once

#include "entities/Order.h"
#include "entities/Product.h"
#include <QButtonGroup>
#include <QDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>
#include <QVBoxLayout>


class AddProductToOrderDialog;

class OrderDialog : public QDialog {
  Q_OBJECT

public:
  explicit OrderDialog(const std::vector<Product> &availableProducts,
                       QWidget *parent = nullptr);

  Order getOrder() const { return currentOrder; }
  void setOrder(const Order &order);

private slots:
  void onOrderTypeChanged();
  void onAddProductClicked();
  void onRemoveProductClicked();
  void onQuantityChanged(int row, int quantity);
  void updateTotals();
  void validateAndAccept();

private:
  void setupUI();
  void addItemToCart(const Product &product, int quantity);
  void refreshCartTable();

  QGroupBox *createCompanyGroupBox();
  QGroupBox *createOrderTypeGroupBox();
  QGroupBox *createCartGroupBox();
  QGroupBox *createTotalsGroupBox();

  QLineEdit *companyNameEdit;
  QLineEdit *contactPersonEdit;
  QLineEdit *phoneEdit;
  QRadioButton *retailRadioButton;
  QRadioButton *wholesaleRadioButton;
  QButtonGroup *orderTypeGroup;
  QPushButton *addProductButton;
  QTableWidget *cartTable;
  QLabel *subtotalLabel;
  QLabel *discountAmountLabel;
  QLabel *finalTotalLabel;
  QPushButton *createOrderButton;
  QPushButton *cancelButton;

  Order currentOrder;
  std::vector<Product> availableProducts;
};
