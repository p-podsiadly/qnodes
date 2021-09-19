#ifndef QNODES_NODE_HPP_INCLUDED
#define QNODES_NODE_HPP_INCLUDED

#include "slot.hpp"

namespace qnodes {

class Node : public QGraphicsObject {
    Q_OBJECT

public:
    static const double borderWidth;
    static const double cornerRadius;

    explicit Node(QGraphicsItem *parent = nullptr);
    Node(const Node &) = delete;
    Node(Node &&) = delete;
    ~Node();

    void setLabel(const QString &label);
    QString label() const;

    void resize(const QSizeF &size);
    QSizeF size() const;

    void setBackgroundBrush(const QBrush &brush);
    QBrush backgroundBrush() const;

    Slot *addSlot(std::unique_ptr<Slot> slot);
    Slot *addSlot(Slot::Type type, const QString &label);

    Slot *slot(int index);
    Slot *slot(Slot::Type type, int index);
    Slot *slotAt(const QPointF &pos);

    QPointF slotPos(Slot::Type type, int idx) const;

    void setContent(QGraphicsWidget *content);
    QGraphicsWidget *content();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

signals:
    void sizeChanged(const QSizeF &new_size);
    void contextMenuRequested(const QPoint &screenPos);

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    struct Impl;
    Impl *m_impl;
};

} // namespace qnodes

#endif // QNODES_NODE_HPP_INCLUDED
