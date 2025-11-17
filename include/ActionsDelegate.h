#pragma once

#include <QStyledItemDelegate>
#include <QPushButton>
#include <QMenu>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QModelIndex>

class ActionsDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit ActionsDelegate(QObject* parent = nullptr);
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    
    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;
    
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

signals:
    void editRequested(int row);
    void deleteRequested(int row);
    void writeOffRequested(int row);
    void reportRequested(int row);

private:
    QRect getButtonRect(const QStyleOptionViewItem& option) const;
};

