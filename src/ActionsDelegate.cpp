#include "ActionsDelegate.h"
#include <QStyleOptionButton>
#include <QStyle>
#include <QFont>
#include <QColor>

ActionsDelegate::ActionsDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {
}

void ActionsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const {
    Q_UNUSED(index);
    
    painter->fillRect(option.rect, option.palette.base());
    
    QRect rect = getButtonRect(option);
    QColor bgColor = QColor("#2196F3");
    QColor hoverColor = QColor("#0b7dda");
    
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush((option.state & QStyle::State_MouseOver) ? hoverColor : bgColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect, 5, 5);
    
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(rect, Qt::AlignCenter, "Actions");
}

bool ActionsDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) {
    Q_UNUSED(model);
    
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        
        QRect buttonRect = getButtonRect(option);
        if (buttonRect.contains(pos)) {
            QMenu menu;
            
            QAction* editAction = menu.addAction("Edit");
            QAction* deleteAction = menu.addAction("Delete");
            QAction* writeOffAction = menu.addAction("Write-off");
            QAction* reportAction = menu.addAction("Generate Report");
            
            QPoint globalPos = option.widget->mapToGlobal(option.rect.bottomLeft());
            QAction* selectedAction = menu.exec(globalPos);
            
            if (selectedAction == editAction) {
                emit editRequested(index.row());
            } else if (selectedAction == deleteAction) {
                emit deleteRequested(index.row());
            } else if (selectedAction == writeOffAction) {
                emit writeOffRequested(index.row());
            } else if (selectedAction == reportAction) {
                emit reportRequested(index.row());
            }
            
            return true;
        }
    }
    
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize ActionsDelegate::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
    Q_UNUSED(index);
    Q_UNUSED(option);
    return QSize(80, 30);
}

QRect ActionsDelegate::getButtonRect(const QStyleOptionViewItem& option) const {
    QRect buttonRect = option.rect;
    buttonRect.setLeft(option.rect.left() + 5);
    buttonRect.setRight(option.rect.right() - 5);
    buttonRect.setTop(option.rect.top() + 2);
    buttonRect.setBottom(option.rect.bottom() - 2);
    return buttonRect;
}


