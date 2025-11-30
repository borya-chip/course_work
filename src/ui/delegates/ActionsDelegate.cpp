#include "ui/delegates/ActionsDelegate.h"
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
    
    // Calculate button widths (each button takes 1/3 of available space)
    int buttonWidth = (option.rect.width() - 20) / 3; // 20px for spacing
    int buttonHeight = option.rect.height() - 4;
    int buttonSpacing = 5;
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Edit button
    QRect editRect(option.rect.left() + 5, option.rect.top() + 2, buttonWidth, buttonHeight);
    QColor editBgColor = QColor("#2196F3");
    painter->setBrush(editBgColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(editRect, 3, 3);
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 9, QFont::Bold));
    painter->drawText(editRect, Qt::AlignCenter, "Edit");
    
    // Delete button
    QRect deleteRect(editRect.right() + buttonSpacing, option.rect.top() + 2, buttonWidth, buttonHeight);
    QColor deleteBgColor = QColor("#f44336");
    painter->setBrush(deleteBgColor);
    painter->drawRoundedRect(deleteRect, 3, 3);
    painter->drawText(deleteRect, Qt::AlignCenter, "Delete");
    
    // Write-off button
    QRect writeOffRect(deleteRect.right() + buttonSpacing, option.rect.top() + 2, buttonWidth, buttonHeight);
    QColor writeOffBgColor = QColor("#FF9800");
    painter->setBrush(writeOffBgColor);
    painter->drawRoundedRect(writeOffRect, 3, 3);
    painter->drawText(writeOffRect, Qt::AlignCenter, "Write-off");
}

bool ActionsDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) {
    Q_UNUSED(model);
    
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        
        // Calculate button rectangles
        int buttonWidth = (option.rect.width() - 20) / 3;
        int buttonHeight = option.rect.height() - 4;
        int buttonSpacing = 5;
        
        QRect editRect(option.rect.left() + 5, option.rect.top() + 2, buttonWidth, buttonHeight);
        QRect deleteRect(editRect.right() + buttonSpacing, option.rect.top() + 2, buttonWidth, buttonHeight);
        QRect writeOffRect(deleteRect.right() + buttonSpacing, option.rect.top() + 2, buttonWidth, buttonHeight);
        
        if (editRect.contains(pos)) {
            emit editRequested(index.row());
            return true;
        } else if (deleteRect.contains(pos)) {
            emit deleteRequested(index.row());
            return true;
        } else if (writeOffRect.contains(pos)) {
            emit writeOffRequested(index.row());
            return true;
        }
    }
    
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize ActionsDelegate::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex& index) const {
    Q_UNUSED(index);
    Q_UNUSED(option);
    return QSize(240, 30); // Wider to accommodate 3 buttons
}


