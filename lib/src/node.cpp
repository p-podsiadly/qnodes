#include <QCursor>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QPainter>
#include <QWidget>
#include <memory>
#include <qnodes/node.hpp>
#include <qnodes/slot.hpp>
#include <vector>

namespace qnodes {

const double Node::borderWidth = 1.5;
const double Node::cornerRadius = 10.0;

struct Node::Impl {
    Node &self;
    QString label;
    QSizeF size = {100.0, 100.0};
    QBrush backgroundBrush;
    std::vector<std::unique_ptr<Slot>> slotList;
    std::unique_ptr<QGraphicsWidget> content;

    explicit Impl(Node &self) : self(self) {}

    QSizeF computeMinSize() const {
        // 1) Compute max number of in/out slots
        int numInSlots = 0, numOutSlots = 0;
        for (const auto &slot : slotList) {
            switch (slot->slotType()) {
            case Slot::Input:
                ++numInSlots;
                break;
            case Slot::Output:
                ++numOutSlots;
                break;
            }
        }

        int numSlots = std::max(numInSlots, numOutSlots);

        // 2) Get min size of the content
        QSizeF minContentSize;
        if (content) {
            minContentSize = content->minimumSize();
        }

        // 3) Compute min size for slots and content
        QSizeF minSize;
        minSize.setWidth(minContentSize.width());
        minSize.setHeight(getSlotYPos(numSlots) + 2.0 * Slot::slotRadius +
                          minContentSize.height());

        // add left, right and bottom margins
        minSize.setWidth(minSize.width() + 2.0 * Slot::slotRadius);
        minSize.setHeight(minSize.height() + Slot::slotRadius);

        return minSize;
    }

    void updateLayout() {
        self.prepareGeometryChange();

        // 1) Compute min size and resize if needed
        QSizeF minSize = computeMinSize();
        size = QSizeF(std::max(minSize.width(), size.width()),
                      std::max(minSize.height(), size.height()));

        // 2) Set positions of slots
        int inSlotIdx = 0, outSlotIdx = 0;
        for (auto &slot : slotList) {
            switch (slot->slotType()) {
            case Slot::Input:
                slot->setPos(0.0, getSlotYPos(inSlotIdx));
                ++inSlotIdx;
                break;
            case Slot::Output:
                slot->setPos(size.width(), getSlotYPos(outSlotIdx));
                ++outSlotIdx;
                break;
            }
        }

        // 3) Update content geometry
        if (content) {
            double topY = getSlotYPos(std::max(inSlotIdx, outSlotIdx));
            double bottomY = size.height() - Slot::slotRadius;

            double leftX = Slot::slotRadius;
            double rightX = size.width() - Slot::slotRadius;

            QRectF contentGeom;
            if ((topY < bottomY) && (leftX < rightX)) {
                contentGeom =
                    QRectF(QPointF(leftX, topY), QPointF(rightX, bottomY));
            }

            content->setGeometry(contentGeom);
        }
    }

    double getSlotYPos(int index) const {
        return 3.0 * cornerRadius + Slot::slotRadius * 3.0 * index;
    }
};

Node::Node(QGraphicsItem *parent)
    : QGraphicsObject(parent), m_impl(new Impl(*this)) {
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemIsMovable);
}

Node::~Node() { delete m_impl; }

void Node::setLabel(const QString &label) {
    if (m_impl->label == label) {
        return;
    }

    m_impl->label = label;
    update();
}

QString Node::label() const { return m_impl->label; }

void Node::resize(const QSizeF &size) {
    if (m_impl->size != size) {
        m_impl->size = size;
        m_impl->updateLayout();
        emit sizeChanged(size);
    }
}

QSizeF Node::size() const { return m_impl->size; }

void Node::setBackgroundBrush(const QBrush &brush) {
    m_impl->backgroundBrush = brush;
    update();
}

QBrush Node::backgroundBrush() const { return m_impl->backgroundBrush; }

Slot *Node::addSlot(std::unique_ptr<Slot> slot) {
    Slot::Type type = slot->slotType();

    int idx = 0;
    for (auto &otherSlot : m_impl->slotList) {
        if (otherSlot->slotType() == type) {
            ++idx;
        }
    }

    slot->setParent(this);
    slot->setParentItem(this);

    slot->setPos(slotPos(type, idx));

    m_impl->slotList.push_back(std::move(slot));

    m_impl->updateLayout();

    return m_impl->slotList.back().get();
}

Slot *Node::addSlot(Slot::Type type, const QString &label) {
    return addSlot(std::make_unique<Slot>(type, label));
}

Slot *Node::slot(int index) {
    if (index >= 0 && index < static_cast<int>(m_impl->slotList.size())) {
        return m_impl->slotList[static_cast<size_t>(index)].get();
    }

    return nullptr;
}

Slot *Node::slot(Slot::Type type, int index) {
    int currentIndex = 0;

    for (auto &slot : m_impl->slotList) {
        if (slot->slotType() != type) {
            continue;
        }

        if (currentIndex == index) {
            return slot.get();
        }

        ++currentIndex;
    }

    return nullptr;
}

QPointF Node::slotPos(Slot::Type type, int idx) const {
    double x = (type == Slot::Input) ? 0.0 : m_impl->size.width();
    double y = m_impl->getSlotYPos(idx);
    return {x, y};
}

void Node::setContent(QGraphicsWidget *content) {
    m_impl->content.reset(content);

    if (m_impl->content) {
        m_impl->content->setParentItem(this);
        m_impl->updateLayout();
    }
}

QGraphicsWidget *Node::content() { return m_impl->content.get(); }

QRectF Node::boundingRect() const {
    double m = borderWidth;

    return QRectF({}, m_impl->size)
        .marginsAdded(
            QMarginsF(m + Slot::slotRadius, m, m + Slot::slotRadius, m));
}

QPainterPath Node::shape() const {
    QPainterPath path;
    path.addRoundedRect(boundingRect(), cornerRadius, cornerRadius);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
    ((void)option);
    ((void)widget);

    QPalette plt = scene()->palette();

    if (isEnabled()) {
        plt.setCurrentColorGroup(QPalette::Active);
    } else {
        plt.setCurrentColorGroup(QPalette::Disabled);
    }

    if (isSelected()) {
        painter->setPen(QPen(plt.color(QPalette::Highlight), borderWidth));
    } else {
        painter->setPen(QPen(plt.color(QPalette::WindowText), borderWidth));
    }

    if (m_impl->backgroundBrush.style() != Qt::NoBrush) {
        painter->setBrush(m_impl->backgroundBrush);
    } else {
        painter->setBrush(plt.window());
    }

    painter->drawRoundedRect(QRectF({borderWidth, borderWidth}, m_impl->size),
                             cornerRadius, cornerRadius);

    painter->setPen(plt.color(QPalette::WindowText));
    QPointF text_pos(cornerRadius + borderWidth,
                     cornerRadius + borderWidth * 2.0);
    painter->drawText(text_pos, m_impl->label);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemSceneHasChanged) {
        m_impl->updateLayout();
    }

    return QGraphicsObject::itemChange(change, value);
}

void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    emit contextMenuRequested(event->screenPos());
}

} // namespace qnodes
