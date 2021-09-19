#include "demo_nodes.hpp"
#include <QGraphicsProxyWidget>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>

Vec3Editor::Vec3Editor() {
    setAttribute(Qt::WA_TranslucentBackground);

    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(new QLabel("X:"), 0, 0);
    layout->addWidget(new QLineEdit("0"), 0, 1);

    layout->addWidget(new QLabel("Y:"), 1, 0);
    layout->addWidget(new QLineEdit("0"), 1, 1);

    layout->addWidget(new QLabel("Z:"), 2, 0);
    layout->addWidget(new QLineEdit("0"), 2, 1);
}

DemoNode::DemoNode() {
    connect(this, &qnodes::Node::contextMenuRequested, this,
            &DemoNode::showContextMenu);
}

std::vector<DemoNode::TypeListItem> DemoNode::getTypes() {
    return {
        TypeListItem{"Float", FloatNode::bgColor(),
                     []() -> DemoNode * { return new FloatNode(); }},
        TypeListItem{"Vector3", Vec3Node::bgColor(),
                     []() -> DemoNode * { return new Vec3Node(); }},
        TypeListItem{
            "Add", BinaryNode::bgColor(BinaryNode::Add),
            []() -> DemoNode * { return new BinaryNode(BinaryNode::Add); }},
        TypeListItem{"Subtract", BinaryNode::bgColor(BinaryNode::Subtract),
                     []() -> DemoNode * {
                         return new BinaryNode(BinaryNode::Subtract);
                     }},
        TypeListItem{"Multiply", BinaryNode::bgColor(BinaryNode::Multiply),
                     []() -> DemoNode * {
                         return new BinaryNode(BinaryNode::Multiply);
                     }},
        TypeListItem{
            "Divide", BinaryNode::bgColor(BinaryNode::Divide),
            []() -> DemoNode * { return new BinaryNode(BinaryNode::Divide); }},
        TypeListItem{
            "Dot product", BinaryNode::bgColor(BinaryNode::Dot),
            []() -> DemoNode * { return new BinaryNode(BinaryNode::Dot); }},
        TypeListItem{
            "Cross product", BinaryNode::bgColor(BinaryNode::Cross),
            []() -> DemoNode * { return new BinaryNode(BinaryNode::Cross); }},
    };
}

void DemoNode::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        deleteLater();
    }
}

void DemoNode::showContextMenu(const QPoint &screenPos) {
    QMenu menu;

    QAction *action = menu.addAction("Delete node");
    connect(action, &QAction::triggered, this, &QObject::deleteLater);

    menu.exec(screenPos);
}

FloatNode::FloatNode() {
    setLabel("Float");

    auto slot = addSlot(qnodes::Slot::Output, "value");
    slot->setToolTip("float");

    QGraphicsProxyWidget *editor_proxy = new QGraphicsProxyWidget();
    editor_proxy->setWidget(new QLineEdit("0"));
    setContent(editor_proxy);

    setBackgroundBrush(bgColor());
}

QColor FloatNode::bgColor() { return QColor(26, 188, 156); }

Vec3Node::Vec3Node() {
    setLabel("Vector3");

    auto slot = addSlot(qnodes::Slot::Output, "value");
    slot->setToolTip("vec3");
    slot = addSlot(qnodes::Slot::Output, "x");
    slot->setToolTip("float");
    slot = addSlot(qnodes::Slot::Output, "y");
    slot->setToolTip("float");
    addSlot(qnodes::Slot::Output, "z");
    slot->setToolTip("float");

    QGraphicsProxyWidget *editor_proxy = new QGraphicsProxyWidget();
    editor_proxy->setWidget(new Vec3Editor());
    setContent(editor_proxy);

    setBackgroundBrush(bgColor());
}

QColor Vec3Node::bgColor() { return QColor(46, 204, 113); }

struct BinaryNodeType {
    QString label;
    QColor color;
    QString in1Type, in2Type;
    QString outType;
};

static const BinaryNodeType g_types[] = {
    {"Add", QColor(52, 152, 219), "float or vec3", "float or vec3",
     "float or vec3"},
    {"Subtract", QColor(155, 89, 182), "float or vec3", "float or vec3",
     "float or vec3"},
    {"Multiply", QColor(22, 160, 133), "float or vec3", "float",
     "float or vec3"},
    {"Divide", QColor(39, 174, 96), "float or vec3", "float", "float or vec3"},
    {"Dot product", QColor(41, 128, 185), "vec3", "vec3", "float"},
    {"Cross product", QColor(142, 68, 173), "vec3", "vec3", "vec3"}};

BinaryNode::BinaryNode(Type type) {
    const auto &bnt = g_types[type];

    setLabel(bnt.label);

    auto slot = addSlot(qnodes::Slot::Input, "a");
    slot->setToolTip(bnt.in1Type);
    slot = addSlot(qnodes::Slot::Input, "b");
    slot->setToolTip(bnt.in2Type);
    slot = addSlot(qnodes::Slot::Output, "result");
    slot->setToolTip(bnt.outType);

    setBackgroundBrush(bgColor(type));
}

QColor BinaryNode::bgColor(Type type) { return g_types[type].color; }
