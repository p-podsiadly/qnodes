#include <QGraphicsScene>
#include <QKeyEvent>
#include <QPainter>
#include <QPalette>
#include <qnodes/bezier.hpp>
#include <qnodes/connection.hpp>
#include <qnodes/slot.hpp>

namespace qnodes {

static QRectF rectFromPoints(const QPointF &a, const QPointF &b) {
    return {QPointF(std::min(a.x(), b.x()), std::min(a.y(), b.y())),
            QPointF(std::max(a.x(), b.x()), std::max(a.y(), b.y()))};
}

const double Connection::width = 2.0;

struct Connection::Impl {
    Connection &self;

    Slot *sourceSlot = nullptr;
    Slot *targetSlot = nullptr;
    QPointF targetPos;
    QuadBezier curve[2];

    Impl(Connection &self, Slot *source) : self(self), sourceSlot(source) {}

    void sourcePosChanged() {
        self.setPos(sourceSlot->scenePos());
        updateCurve();
    }

    void sourceDestroyed() {
        sourceSlot = nullptr;
        self.deleteLater();
    }

    void targetPosChanged() {
        self.prepareGeometryChange();
        targetPos = self.targetPos();
        updateCurve();
    }

    void targetDestroyed() {
        targetSlot = nullptr;
        self.deleteLater();
    }

    void updateCurve() {
        QPointF tPos = self.mapFromScene(self.targetPos());
        QPointF midPos = tPos / 2.0;

        curve[0].set({}, {tPos.x() * 0.25, 0.0}, midPos);
        curve[1].set(midPos, {tPos.x() * 0.75, tPos.y()}, tPos);
    }
};

Connection::Connection(Slot *source) : m_impl(new Impl(*this, source)) {
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);

    connect(source, &Slot::scenePosChanged, this,
            [this]() { m_impl->sourcePosChanged(); });
    connect(source, &Slot::destroyed, this,
            [this]() { m_impl->sourceDestroyed(); });

    m_impl->sourcePosChanged();
}

Connection::~Connection() { delete m_impl; }

Slot *Connection::sourceSlot() const { return m_impl->sourceSlot; }

void Connection::setTargetSlot(Slot *target) {
    prepareGeometryChange();

    if (m_impl->targetSlot) {
        m_impl->targetSlot->disconnect(this);
    }

    m_impl->targetSlot = target;

    if (m_impl->targetSlot) {
        m_impl->targetPos = targetPos();

        connect(m_impl->targetSlot, &Slot::scenePosChanged, this,
                [this]() { m_impl->targetPosChanged(); });
        connect(m_impl->targetSlot, &Slot::destroyed, this,
                [this]() { m_impl->targetDestroyed(); });
    }

    m_impl->updateCurve();
}

Slot *Connection::targetSlot() const { return m_impl->targetSlot; }

void Connection::setTargetPos(const QPointF &pos) {
    if (!m_impl->targetSlot) {
        prepareGeometryChange();
        m_impl->targetPos = pos;
        m_impl->updateCurve();
    }
}

QPointF Connection::targetPos() const {
    if (m_impl->targetSlot) {
        return m_impl->targetSlot->scenePos();
    } else {
        return m_impl->targetPos;
    }
}

bool Connection::contains(const QPointF &pos) const {
    for (const auto &c : m_impl->curve) {
        QPointF cp = c.closestPointTo(pos);
        if (squaredDistance(pos, cp) <= (width * width)) {
            return true;
        }
    }

    return false;
}

QRectF Connection::boundingRect() const {
    double m = width;

    QPointF ep = m_impl->curve[1].endPoint();
    return rectFromPoints({}, ep).marginsAdded({m, m, m, m});
}

QPainterPath Connection::shape() const {
    QPainterPath path;

    for (const auto &c : m_impl->curve) {
        path.quadTo(c.point(1), c.point(2));
    }

    return path;
}

void Connection::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
    ((void)option);
    ((void)widget);

    QPalette plt = scene()->palette();
    double handleR = Slot::slotRadius * 0.5;

    QPainterPath path = shape();

    if (isSelected()) {
        painter->setPen(QPen(plt.color(QPalette::Highlight), width * 2.0));
        painter->drawPath(path);
        painter->drawEllipse(m_impl->curve[1].endPoint(), handleR, handleR);
    }

    QColor color = plt.color(isSelected() ? QPalette::HighlightedText
                                          : QPalette::WindowText);

    painter->setPen(QPen(color, width));
    painter->drawPath(path);

    painter->setBrush(color);
    painter->drawEllipse(m_impl->curve[1].endPoint(), handleR, handleR);
}

QVariant Connection::itemChange(GraphicsItemChange change,
                                const QVariant &value) {
    m_impl->updateCurve();
    return QGraphicsObject::itemChange(change, value);
}

void Connection::keyReleaseEvent(QKeyEvent *event) {
    if ((event->key() == Qt::Key_Delete) && isSelected()) {
        deleteLater();
    } else {
        QGraphicsObject::keyReleaseEvent(event);
    }
}

} // namespace qnodes
