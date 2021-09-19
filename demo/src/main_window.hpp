#ifndef MAIN_WINDOW_HPP_INCLUDED
#define MAIN_WINDOW_HPP_INCLUDED

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <memory>
#include <qnodes/node.hpp>

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    std::unique_ptr<QGraphicsScene> m_scene;
    QGraphicsView *m_view;

    void initMenuBar();

    void showAddNodeMenu(const QPoint &pos);

    void setDefaultStyle();
    void setStyleFromFile(const QString &file);
};

#endif // MAIN_WINDOW_HPP_INCLUDED
