#ifndef QNODES_SLOT_HPP_INCLUDED
#define QNODES_SLOT_HPP_INCLUDED

#include <QGraphicsObject>

namespace qnodes {

class Node;
class Connection;

class Slot : public QGraphicsObject {
    Q_OBJECT

public:
    static const double slotRadius;

    enum Type { Input, Output };

    Slot(Type type, const QString &label);
    Slot(const Slot &) = delete;
    Slot(Slot &&) = delete;
    ~Slot();

    Node *node() const;

    Type slotType() const;

    void setLabel(const QString &label);
    QString label() const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

signals:
    void labelChanged(const QString &label);
    void scenePosChanged();
    void connectionAdded(Connection *newConnection);

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    virtual bool acceptConnectionFrom(const Slot *src) const;

private:
    struct Impl;
    Impl *m_impl;
};

} // namespace qnodes

#endif // QNODES_SLOT_HPP_INCLUDED
