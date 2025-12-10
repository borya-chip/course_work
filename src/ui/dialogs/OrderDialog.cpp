#include "ui/dialogs/OrderDialog.h"
#include "services/DiscountCalculator.h"
#include "ui/dialogs/AddProductToOrderDialog.h"
#include <QBrush>
#include <QButtonGroup>
#include <QColor>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSpinBox>
#include <QTableWidgetItem>
#include <algorithm>


OrderDialog::OrderDialog(const std::vector<Product> &availableProducts,
                         QWidget *parent)
    : QDialog(parent), availableProducts(availableProducts) {
  currentOrder = Order("", "", "", OrderType::RETAIL);
  setupUI();
}

void OrderDialog::setOrder(const Order &order) {
  currentOrder = order;

  companyNameEdit->setText(order.getCompanyName());
  contactPersonEdit->setText(order.getContactPerson());
  phoneEdit->setText(order.getPhone());

  if (order.getOrderType() == OrderType::RETAIL) {
    retailRadioButton->setChecked(true);
  } else {
    wholesaleRadioButton->setChecked(true);
  }

  currentOrder.clearItems();
  for (const auto &item : order.getItems()) {
    currentOrder.addItem(item);
  }
  currentOrder.calculateTotal();

  refreshCartTable();
  updateTotals();

  createOrderButton->setText("Update Order");
  setWindowTitle("Edit Order");
}

void OrderDialog::setupUI() {
  setWindowTitle("Create Order");
  setMinimumSize(1000, 700);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  mainLayout->addWidget(createCompanyGroupBox());

  mainLayout->addWidget(createOrderTypeGroupBox());

  QHBoxLayout *addProductLayout = new QHBoxLayout();
  addProductButton = new QPushButton("Add Product", this);
  addProductButton->setStyleSheet(
      "background-color: #4CAF50; color: white; padding: 10px 20px; "
      "font-weight: bold; border-radius: 5px;");
  addProductLayout->addWidget(addProductButton);
  addProductLayout->addStretch();
  mainLayout->addLayout(addProductLayout);

  mainLayout->addWidget(createCartGroupBox());

  mainLayout->addWidget(createTotalsGroupBox());

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  createOrderButton = new QPushButton("Create Order", this);
  createOrderButton->setStyleSheet(
      "background-color: #4CAF50; color: white; padding: 10px 20px; "
      "font-weight: bold; border-radius: 5px;");
  cancelButton = new QPushButton("Cancel", this);
  cancelButton->setStyleSheet(
      "background-color: #757575; color: white; padding: 10px 20px; "
      "font-weight: bold; border-radius: 5px;");

  buttonLayout->addStretch();
  buttonLayout->addWidget(createOrderButton);
  buttonLayout->addWidget(cancelButton);
  mainLayout->addLayout(buttonLayout);

  connect(retailRadioButton, &QRadioButton::toggled, this,
          &OrderDialog::onOrderTypeChanged);
  connect(wholesaleRadioButton, &QRadioButton::toggled, this,
          &OrderDialog::onOrderTypeChanged);
  connect(addProductButton, &QPushButton::clicked, this,
          &OrderDialog::onAddProductClicked);
  connect(createOrderButton, &QPushButton::clicked, this,
          &OrderDialog::validateAndAccept);
  connect(cancelButton, &QPushButton::clicked, this, &OrderDialog::reject);

  updateTotals();
}

QGroupBox *OrderDialog::createCompanyGroupBox() {
  QGroupBox *groupBox = new QGroupBox("Company Information", this);
  QFormLayout *layout = new QFormLayout(groupBox);

  companyNameEdit = new QLineEdit(this);
  companyNameEdit->setPlaceholderText("Company Name");
  layout->addRow("Company Name:", companyNameEdit);

  contactPersonEdit = new QLineEdit(this);
  contactPersonEdit->setPlaceholderText("Contact Person");
  layout->addRow("Contact Person:", contactPersonEdit);

  phoneEdit = new QLineEdit(this);
  phoneEdit->setPlaceholderText("+1 (XXX) XXX-XXXX");
  layout->addRow("Phone:", phoneEdit);

  return groupBox;
}

QGroupBox *OrderDialog::createOrderTypeGroupBox() {
  QGroupBox *groupBox = new QGroupBox("Order Mode", this);
  QHBoxLayout *layout = new QHBoxLayout(groupBox);

  orderTypeGroup = new QButtonGroup(this);

  retailRadioButton = new QRadioButton("Retail", this);
  wholesaleRadioButton = new QRadioButton("Wholesale", this);
  retailRadioButton->setChecked(true);

  orderTypeGroup->addButton(retailRadioButton,
                            static_cast<int>(OrderType::RETAIL));
  orderTypeGroup->addButton(wholesaleRadioButton,
                            static_cast<int>(OrderType::WHOLESALE));

  layout->addWidget(retailRadioButton);
  layout->addWidget(wholesaleRadioButton);
  layout->addStretch();

  return groupBox;
}

QGroupBox *OrderDialog::createCartGroupBox() {
  QGroupBox *groupBox = new QGroupBox("Products in Order", this);
  QVBoxLayout *layout = new QVBoxLayout(groupBox);

  cartTable = new QTableWidget(0, 7, this);
  cartTable->setHorizontalHeaderLabels({"Product", "Quantity", "Price",
                                        "Amount", "Discount", "Total",
                                        "Actions"});
  cartTable->verticalHeader()->setVisible(false);
  cartTable->verticalHeader()->setMaximumWidth(0);
  cartTable->horizontalHeader()->setStretchLastSection(false);
  cartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  cartTable->horizontalHeader()->setSectionResizeMode(
      6, QHeaderView::ResizeToContents);
  cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  cartTable->setAlternatingRowColors(true);

  layout->addWidget(cartTable);

  return groupBox;
}

QGroupBox *OrderDialog::createTotalsGroupBox() {
  QGroupBox *groupBox = new QGroupBox("Order Summary", this);
  QVBoxLayout *layout = new QVBoxLayout(groupBox);

  QHBoxLayout *subtotalLayout = new QHBoxLayout();
  subtotalLayout->addWidget(new QLabel("Subtotal:", this));
  subtotalLabel = new QLabel("$0.00", this);
  subtotalLabel->setAlignment(Qt::AlignRight);
  subtotalLayout->addWidget(subtotalLabel);
  layout->addLayout(subtotalLayout);

  QHBoxLayout *discountLayout = new QHBoxLayout();
  discountLayout->addWidget(new QLabel("Discount:", this));
  discountAmountLabel = new QLabel("$0.00", this);
  discountAmountLabel->setAlignment(Qt::AlignRight);
  discountAmountLabel->setStyleSheet("color: green; font-weight: bold;");
  discountLayout->addWidget(discountAmountLabel);
  layout->addLayout(discountLayout);

  QHBoxLayout *totalLayout = new QHBoxLayout();
  QLabel *totalTitleLabel = new QLabel("TOTAL:", this);
  totalTitleLabel->setStyleSheet("font-weight: bold; font-size: 14pt;");
  totalLayout->addWidget(totalTitleLabel);
  finalTotalLabel = new QLabel("$0.00", this);
  finalTotalLabel->setAlignment(Qt::AlignRight);
  finalTotalLabel->setStyleSheet(
      "font-weight: bold; font-size: 16pt; color: #2196F3;");
  totalLayout->addWidget(finalTotalLabel);
  layout->addLayout(totalLayout);

  return groupBox;
}

void OrderDialog::onOrderTypeChanged() {
  OrderType type =
      retailRadioButton->isChecked() ? OrderType::RETAIL : OrderType::WHOLESALE;
  currentOrder.setOrderType(type);
  refreshCartTable();
  updateTotals();
}

void OrderDialog::onAddProductClicked() {
  bool isWholesale = wholesaleRadioButton->isChecked();

  std::vector<Product> filteredProducts = availableProducts;
  for (auto &prod : filteredProducts) {

    for (const auto &item : currentOrder.getItems()) {
      if (item.productId == prod.getId()) {

        int newQty = prod.getQuantity() - item.quantity;
        if (newQty < 0)
          newQty = 0;
        prod.setQuantity(newQty);
        break;
      }
    }
  }

  AddProductToOrderDialog dialog(filteredProducts, isWholesale, this);

  if (dialog.exec() == QDialog::Accepted) {
    Product product = dialog.getSelectedProduct();
    int quantity = dialog.getQuantity();

    bool productExists = false;
    for (const auto &item : currentOrder.getItems()) {
      if (item.productId == product.getId()) {
        productExists = true;

        int totalQty = item.quantity + quantity;
        auto it =
            std::find_if(availableProducts.begin(), availableProducts.end(),
                         [&product](const Product &p) {
                           return p.getId() == product.getId();
                         });
        if (it != availableProducts.end() && totalQty > it->getQuantity()) {
          QMessageBox::warning(
              this, "Error",
              QString("Total quantity exceeds available stock! Available: %1, "
                      "Already in cart: %2")
                  .arg(it->getQuantity())
                  .arg(item.quantity));
          return;
        }

        currentOrder.updateItemQuantity(product.getId(), totalQty);
        break;
      }
    }

    if (!productExists) {
      addItemToCart(product, quantity);
    }

    refreshCartTable();
    updateTotals();
  }
}

void OrderDialog::onRemoveProductClicked() {
  int currentRow = cartTable->currentRow();
  if (currentRow < 0) {
    QMessageBox::warning(this, "Error", "Please select a product to remove!");
    return;
  }

  QTableWidgetItem *idItem = cartTable->item(currentRow, 0);
  if (idItem) {
    int productId = idItem->data(Qt::UserRole).toInt();
    currentOrder.removeItem(productId);
    refreshCartTable();
    updateTotals();
  }
}

void OrderDialog::onQuantityChanged(int row, int quantity) {
  QTableWidgetItem *idItem = cartTable->item(row, 0);
  if (idItem) {
    int productId = idItem->data(Qt::UserRole).toInt();

    auto it = std::find_if(
        availableProducts.begin(), availableProducts.end(),
        [productId](const Product &p) { return p.getId() == productId; });

    if (it != availableProducts.end()) {

      if (quantity > it->getQuantity()) {
        QMessageBox::warning(this, "Error",
                             QString("Insufficient stock! Available: %1")
                                 .arg(it->getQuantity()));

        refreshCartTable();
        updateTotals();
        return;
      }
    }

    currentOrder.updateItemQuantity(productId, quantity);
    refreshCartTable();
    updateTotals();
  }
}

void OrderDialog::addItemToCart(const Product &product, int quantity) {
  OrderItem item(product.getId(), QString::fromStdString(product.getName()),
                 QString::fromStdString(product.getCategory()), quantity,
                 product.getUnitPrice());

  currentOrder.addItem(item);
}

void OrderDialog::refreshCartTable() {
  cartTable->setRowCount(0);

  const auto &items = currentOrder.getItems();
  bool isWholesale = (currentOrder.getOrderType() == OrderType::WHOLESALE);

  for (const auto &item : items) {
    int row = cartTable->rowCount();
    cartTable->insertRow(row);

    QTableWidgetItem *nameItem = new QTableWidgetItem(item.productName);
    nameItem->setData(Qt::UserRole, item.productId);
    cartTable->setItem(row, 0, nameItem);

    QSpinBox *qtySpinBox = new QSpinBox();
    qtySpinBox->setMinimum(1);

    int maxStock = 9999;
    auto it = std::find_if(
        availableProducts.begin(), availableProducts.end(),
        [&item](const Product &p) { return p.getId() == item.productId; });
    if (it != availableProducts.end()) {
      maxStock = it->getQuantity();
    }
    qtySpinBox->setMaximum(maxStock);
    qtySpinBox->setValue(item.quantity);
    connect(qtySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            [this, row](int value) { onQuantityChanged(row, value); });
    cartTable->setCellWidget(row, 1, qtySpinBox);

    cartTable->setItem(
        row, 2,
        new QTableWidgetItem(QString("$%1").arg(item.unitPrice, 0, 'f', 2)));

    double amount = item.unitPrice * item.quantity;
    cartTable->setItem(
        row, 3, new QTableWidgetItem(QString("$%1").arg(amount, 0, 'f', 2)));

    double discount =
        DiscountCalculator::calculateDiscount(isWholesale, item.quantity);
    QTableWidgetItem *discountItem =
        new QTableWidgetItem(QString("%1%").arg(discount, 0, 'f', 1));
    if (discount > 0) {
      discountItem->setForeground(QBrush(QColor("green")));
    }
    cartTable->setItem(row, 4, discountItem);

    cartTable->setItem(
        row, 5,
        new QTableWidgetItem(QString("$%1").arg(item.totalPrice, 0, 'f', 2)));

    QWidget *actionsWidget = new QWidget();
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(5, 2, 5, 2);
    actionsLayout->setSpacing(5);

    QPushButton *deleteBtn = new QPushButton("Delete", actionsWidget);
    deleteBtn->setStyleSheet("QPushButton {"
                             "    background-color: #f44336;"
                             "    color: white;"
                             "    border: none;"
                             "    border-radius: 3px;"
                             "    padding: 5px 10px;"
                             "    font-size: 12px;"
                             "    font-weight: bold;"
                             "}"
                             "QPushButton:hover {"
                             "    background-color: #da190b;"
                             "}");

    connect(deleteBtn, &QPushButton::clicked, this, [this, item]() {
      currentOrder.removeItem(item.productId);
      refreshCartTable();
      updateTotals();
    });

    actionsLayout->addWidget(deleteBtn);
    actionsLayout->addStretch();
    cartTable->setCellWidget(row, 6, actionsWidget);
  }

  cartTable->horizontalHeader()->setStretchLastSection(false);
  cartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  cartTable->horizontalHeader()->setSectionResizeMode(
      6, QHeaderView::ResizeToContents);
}

void OrderDialog::updateTotals() {
  currentOrder.calculateTotal();

  double subtotal = 0.0;
  for (const auto &item : currentOrder.getItems()) {
    subtotal += item.unitPrice * item.quantity;
  }

  double discount = currentOrder.getTotalDiscount();
  double finalTotal = currentOrder.getTotalAmount();

  subtotalLabel->setText(QString("$%1").arg(subtotal, 0, 'f', 2));
  discountAmountLabel->setText(QString("-$%1").arg(discount, 0, 'f', 2));
  finalTotalLabel->setText(QString("$%1").arg(finalTotal, 0, 'f', 2));
}

void OrderDialog::validateAndAccept() {
  if (companyNameEdit->text().trimmed().isEmpty()) {
    QMessageBox::warning(this, "Error", "Please enter company name!");
    return;
  }

  if (contactPersonEdit->text().trimmed().isEmpty()) {
    QMessageBox::warning(this, "Error", "Please enter contact person!");
    return;
  }

  if (phoneEdit->text().trimmed().isEmpty()) {
    QMessageBox::warning(this, "Error", "Please enter phone number!");
    return;
  }

  if (currentOrder.getItems().empty()) {
    QMessageBox::warning(this, "Error", "Please add products to cart!");
    return;
  }

  currentOrder.setCompanyName(companyNameEdit->text().trimmed());
  currentOrder.setContactPerson(contactPersonEdit->text().trimmed());
  currentOrder.setPhone(phoneEdit->text().trimmed());
  OrderType type =
      retailRadioButton->isChecked() ? OrderType::RETAIL : OrderType::WHOLESALE;
  currentOrder.setOrderType(type);
  currentOrder.calculateTotal();

  accept();
}
