#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QVector>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include <QFileDialog>
#include <QPushButton>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    struct MyFigure
    {
        QVector<QPoint> pos;
        bool selected;
    };

    explicit MainWindow(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void drawImage(QPainter* painter);
    void drawFigure(QPainter *painter, const MyFigure &figure);
    void drawEdges(QPainter *painter, const QList<QPoint> &list);

private:
    QPushButton* button;
    QString file;

    QList<MyFigure> myFigures;
    QList<QPoint> currentPoints;

    QPoint lastPoint;
    QPoint dotPos;
    QPoint tempPoint;
    bool pressed;
    int selectedDotIndex;
    int selectedFigureIndex;
    MyFigure pressedFigure;

    int dotRadius;
    int edgeWidth;

    QColor dotColor;
    QColor edgeColor;
    QColor figureColor;
    QColor selectColor;

private:
    double length(const QPoint &point1, const QPoint &point2);
    bool checkFigureSelected(const QVector<QPoint> &points, int cpX, int cpY);
    void clearCurr();

private slots:
    void chooseImage();
};
#endif // MAINWINDOW_H
