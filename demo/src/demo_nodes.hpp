#ifndef DEMO_NODES_HPP_INCLUDED
#define DEMO_NODES_HPP_INCLUDED

#include <QWidget>
#include <qnodes/node.hpp>

class Vec3Editor : public QWidget {
public:
    Vec3Editor();
};

class DemoNode : public qnodes::Node {
public:
    struct TypeListItem {
        QString name;
        QColor color;
        DemoNode *(*factoryFn)();
    };

    DemoNode();

    static std::vector<TypeListItem> getTypes();

protected:
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void showContextMenu(const QPoint &screenPos);
};

class FloatNode : public DemoNode {
public:
    FloatNode();

    static QColor bgColor();
};

class Vec3Node : public DemoNode {
public:
    Vec3Node();

    static QColor bgColor();
};

class BinaryNode : public DemoNode {
public:
    enum Type { Add, Subtract, Multiply, Divide, Dot, Cross };

    explicit BinaryNode(Type type);

    static QColor bgColor(Type type);

private:
    Type m_type;
};

#endif // DEMO_NODES_HPP_INCLUDED
