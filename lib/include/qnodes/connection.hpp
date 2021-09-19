#ifndef QNODES_CONNECTION_HPP_INCLUDED
#define QNODES_CONNECTION_HPP_INCLUDED

#include <QGraphicsObject>

namespace qnodes {

class Slot;

class Connection : public QGraphicsObject {
    Q_OBJECT

public:
    static const double width;

    explicit Connection(Slot *source);
    Connection(const Connection &) = delete;
    Connection(Connection &&) = delete;
    ~Connection();

    Slot *sourceSlot() const;

    void setTargetSlot(Slot *target);
    Slot *targetSlot() const;

    void setTargetPos(const QPointF &pos);
    QPointF targetPos() const;

    bool contains(const QPointF &pos) const override;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:
    struct Impl;
    Impl *m_impl;
};

} // namespace qnodes

#endif // QNODES_CONNECTION_HPP_INCLUDED
