#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QStaticText>
#include <qnodes/connection.hpp>
#include <qnodes/node.hpp>
#include <qnodes/slot.hpp>

namespace qnodes {

const double Slot::slotRadius = 6.0;

struct Slot::Impl {
    Slot &self;
    Type type;
    QString label;
    QStaticText labelText;
    std::unique_ptr<Connection> newConnection;

    explicit Impl(Slot &self, Type type, const QString &label)
        : self(self), type(type) {}

    QPointF labelPos() const {
        if (type == Input) {
            return QPointF(slotRadius + 2.0, -slotRadius);
        } else {
            double w = labelText.size().width();
            return QPointF(-w - slotRadius - 2.0f, -slotRadius);
        }
    }

    void setDefCursor() {
        if (type == Output) {
            self.setCursor(Qt::OpenHandCursor);
        } else {
            self.setCursor({});
        }
    }
};

Slot::Slot(Type type, const QString &label)
    : m_impl(new Impl(*this, type, label)) {
    m_impl->setDefCursor();
    setLabel(label);
    setFlag(ItemSendsScenePositionChanges);
}

Slot::~Slot() { delete m_impl; }

Node *Slot::node() const { return dynamic_cast<Node *>(parentItem()); }

Slot::Type Slot::slotType() const { return m_impl->type; }

void Slot::setLabel(const QString &label) {
    if (m_impl->label != label) {
        m_impl->label = label;
        m_impl->labelText.setText(m_impl->label);
        emit labelChanged(label);
        update();
    }
}

QString Slot::label() const { return m_impl->label; }

QRectF Slot::boundingRect() const {
    double srm = slotRadius + 0.5; // slot radius with margin
    QRectF br(-srm, -srm, srm * 2.0, srm * 2.0);

    QPointF text_pos = m_impl->labelPos();
    QSizeF text_sz = m_impl->labelText.size();

    br = br.united(QRectF(text_pos, text_sz));
    return br;
}

QPainterPath Slot::shape() const {
    QPainterPath path;
    path.addEllipse(QPointF(0.0, 0.0), slotRadius + 1.0, slotRadius + 1.0);
    return path;
}

void Slot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
    ((void)option);
    ((void)widget);

    QPalette plt = scene()->palette();

    painter->setPen(QPen(plt.color(QPalette::WindowText), 1.0));
    painter->setBrush(plt.color(QPalette::Base));

    painter->drawEllipse(QPointF(), slotRadius, slotRadius);
    painter->drawStaticText(m_impl->labelPos(), m_impl->labelText);
}

QVariant Slot::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemScenePositionHasChanged) {
        emit scenePosChanged();
    }

    return QGraphicsObject::itemChange(change, value);
}

void Slot::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_impl->type == Input) {
        return;
    }

    m_impl->newConnection = std::make_unique<Connection>(this);
    m_impl->newConnection->setTargetPos(event->scenePos());
    scene()->addItem(m_impl->newConnection.get());

    setCursor(Qt::ClosedHandCursor);
}

void Slot::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    Slot *targetSlot = nullptr;

    QList<QGraphicsItem *> possibleTargets = scene()->items(event->scenePos());
    for (QGraphicsItem *item : possibleTargets) {
        Slot *slot = dynamic_cast<Slot *>(item);
        if (slot && slot->acceptConnectionFrom(this)) {
            targetSlot = slot;
            break;
        }
    }

    if (targetSlot && m_impl->newConnection) {
        m_impl->newConnection->setTargetSlot(targetSlot);
        emit connectionAdded(m_impl->newConnection.release());
    }

    m_impl->newConnection.reset();
    m_impl->setDefCursor();
}

void Slot::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_impl->newConnection) {
        m_impl->newConnection->setTargetPos(event->scenePos());
    }
}

bool Slot::acceptConnectionFrom(const Slot *other) const {
    if (slotType() != Input) {
        return false;
    }

    if (node() == other->node()) {
        return false;
    }

    auto items = other->childItems();
    for (QGraphicsItem *item : items) {
        Connection *conn = dynamic_cast<Connection *>(item);
        if (conn && (conn->targetSlot() == this)) {
            return false;
        }
    }

    return true;
}

} // namespace qnodes
