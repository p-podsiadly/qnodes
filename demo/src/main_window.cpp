#include "main_window.hpp"
#include "demo_nodes.hpp"
#include <QAction>
#include <QApplication>
#include <QFile>
#include <QGraphicsView>
#include <QMenu>
#include <QMenuBar>
#include <QTextStream>
#include <QVBoxLayout>
#include <qnodes/connection.hpp>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_scene = std::make_unique<QGraphicsScene>();

    DemoNode *node = new Vec3Node();
    node->setPos(-100.0, -100.0);
    m_scene->addItem(node);

    node = new FloatNode();
    node->setPos(-50.0, 200.0);
    m_scene->addItem(node);

    node = new BinaryNode(BinaryNode::Add);
    node->setPos(200.0, 0.0);
    m_scene->addItem(node);

    QWidget *central_widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(central_widget);
    setCentralWidget(central_widget);

    m_view = new QGraphicsView();
    m_view->setScene(m_scene.get());
    m_view->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(m_view);

    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QWidget::customContextMenuRequested, this,
            &MainWindow::showAddNodeMenu);

    initMenuBar();
}

void MainWindow::initMenuBar() {
    QMenuBar *bar = menuBar();

    QMenu *menu = bar->addMenu("Style");

    QAction *action = menu->addAction("Default");
    connect(action, &QAction::triggered, this, [&]() { setDefaultStyle(); });

    action = menu->addAction("Dark");
    connect(action, &QAction::triggered, this,
            [&]() { setStyleFromFile(":qdarkstyle/dark/style.qss"); });
}

void MainWindow::showAddNodeMenu(const QPoint &pos) {
    QMenu menu;

    auto nodeTypeList = DemoNode::getTypes();
    for (const auto &type : nodeTypeList) {
        QAction *action = menu.addAction(type.name);

        QPixmap pm(8, 8);
        pm.fill(type.color);
        action->setIcon(QIcon(pm));

        connect(action, &QAction::triggered, [&]() {
            DemoNode *node = type.factoryFn();
            m_scene->addItem(node);
            node->setPos(m_view->mapToScene(pos));
        });
    }

    menu.exec(m_view->mapToGlobal(pos));
}

void MainWindow::setDefaultStyle() {
    setStyleSheet({});
    m_scene->setPalette(palette());
}

void MainWindow::setStyleFromFile(const QString &file) {
    QFile f(file);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&f);
    QString stylesheet = stream.readAll();
    qInfo(stylesheet.toStdString().c_str());
    setStyleSheet(stylesheet);

    m_scene->setPalette(palette());
}
