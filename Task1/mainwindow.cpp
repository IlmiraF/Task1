#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
{
    dotRadius = 7;
    edgeWidth = 5;

    dotColor = QColor(0, 183, 235, 127);
    edgeColor = QColor(0, 183, 235, 127);
    figureColor = QColor(128, 128, 128, 127);
    selectColor = QColor(255, 255, 255, 127);

    selectedFigureIndex = -1;
    selectedDotIndex = -1;

    button = new QPushButton("Open Image", this);
    button->move(0,0);

    connect(button, SIGNAL(clicked(bool)), this, SLOT(chooseImage()));

    setMinimumWidth(1280);
    setMinimumHeight(720);

    Q_UNUSED(parent);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(file.isEmpty())
    {
        return;
    }

    QPoint p = event->pos();
    pressed = true;
    lastPoint = this->mapToGlobal(p);

    if(currentPoints.isEmpty()) {
        bool selectedDot = false;
        selectedDotIndex = -1;
        if (selectedFigureIndex != -1) {
            for(int i = myFigures[selectedFigureIndex].pos.size() - 1; i >= 0; --i) {
                if(length(p, myFigures[selectedFigureIndex].pos[i]) <= 36) {
                    selectedDot = true;
                    selectedDotIndex = i;
                    dotPos = myFigures[selectedFigureIndex].pos[i];
                    break;
                }
            }
        }

        if(selectedDot) {
            return;
        }

        selectedFigureIndex = -1;
        for(int i = myFigures.size() - 1; i >= 0; --i) {
            myFigures[i].selected = checkFigureSelected(myFigures[i].pos, p.x(), p.y());
            if(myFigures[i].selected) {
                if(selectedFigureIndex == -1) {
                    selectedFigureIndex = i;
                    pressedFigure = myFigures[i];
                } else {
                    myFigures[i].selected = false;
                }
            }
        }

        this->update();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    tempPoint = event->pos();
    if(pressed && selectedFigureIndex != -1) {
        QPoint d = this->mapToGlobal(tempPoint) - lastPoint;
        int size = myFigures[selectedFigureIndex].pos.size();

        if(selectedDotIndex != -1) {
            myFigures[selectedFigureIndex].pos[selectedDotIndex] = dotPos + d;
        } else if(selectedFigureIndex != -1) {
            for(int i = 0; i < size; ++i) {
                myFigures[selectedFigureIndex].pos[i] = pressedFigure.pos[i] + d;
            }
        }
    }

    this->update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(file.isEmpty())
    {
        return;
    }

    if (event->button() == Qt::RightButton) {
        clearCurr();
        return;
    }

    pressed = false;
    if(selectedFigureIndex != -1) {
        return;
    }

    QPoint point = event->pos();
    if(currentPoints.count() > 0) {
        qreal len = (qPow(currentPoints.first().x() - point.x() , 2.0)
                     + qPow(currentPoints.first().y() - point.y() , 2.0) );
        if(len < 100) {
            if(currentPoints.size() >= 3) {
                MyFigure pol;
                pol.pos = currentPoints.toVector();
                pol.selected = false;
                myFigures.append(pol);
            }

            currentPoints.clear();
            this->update();
            return;
        }
    }

    currentPoints.append(point);
    this->update();
}

void MainWindow::paintEvent(QPaintEvent * event)
{
    if(file.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);

    drawImage(&painter);

    foreach(const MyFigure& figure, myFigures) {
        drawFigure(&painter, figure);
    }

    drawEdges(&painter, currentPoints);

    Q_UNUSED(event);
}

void MainWindow::drawFigure(QPainter *painter, const MyFigure &figure)
{
    painter->save();

    painter->setPen(QPen(edgeColor, edgeWidth));
    figure.selected ? painter->setBrush(selectColor) : painter->setBrush(figureColor);
    painter->drawPolygon(figure.pos.data(), figure.pos.size());

    if(figure.selected) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(dotColor);
        foreach(const QPoint &point, figure.pos) {
            painter->drawEllipse(point, dotRadius, dotRadius);
        }
    }

    painter->restore();
}

void MainWindow::drawEdges(QPainter *painter, const QList<QPoint> &list)
{
    painter->save();

    int count = list.count();
    if (count > 0) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(dotColor);
        for(int i = 0; i < count; ++i) {
            painter->drawEllipse(list[i], dotRadius, dotRadius);
        }

        painter->setPen(QPen(edgeColor, edgeWidth));
        painter->setBrush(Qt::NoBrush);
        for(int i = 0; i < count - 1; ++i) {
            painter->drawLine(list[i], list[i + 1]);
        }
    }

    painter->restore();
}

void MainWindow::drawImage(QPainter *painter)
{
    painter->save();

    if(!file.isEmpty())
    {
        QPixmap pix;
        pix.load(file);
        painter->drawPixmap(0, 0, pix.scaled(1280, 720, Qt::KeepAspectRatio));
        painter->setBrush(QColor(255, 255, 255, 100));
    }

    painter->restore();
}

double MainWindow::length(const QPoint &point1, const QPoint &point2)
{
    return qPow(point1.x() - point2.x(), 2.0) + qPow(point1.y() - point2.y(), 2.0);
}

bool MainWindow::checkFigureSelected(const QVector<QPoint> &points, int cpX, int cpY)
{
    const int count = points.size();
    if(count < 3) {
        return false;
    }

    QList<int> vX, vY;
    for(int i = 0; i < count; ++i) {
        vX.append(points[i].x());
        vY.append(points[i].y());
    }

    bool result = false;
    for (int i = 0, j = count - 1; i < count; j = i++) {
        bool b1 = (vY[i] > cpY) != (vY[j] > cpY);
        bool b2 = (cpX < ((vX[j] - vX[i]) * (cpY - vY[i])
                    / (vY[j] - vY[i])
                          + vX[i]));
        if (b1 && b2) {
            result = !result;
        }
    }

    return result;
}

void MainWindow::clearCurr()
{
    currentPoints.clear();
    selectedDotIndex = -1;
    selectedFigureIndex = -1;
    this->update();
}

void MainWindow::chooseImage()
{
    QString openedFile = QFileDialog::getOpenFileName(nullptr, "", "C:\\", "*.png *.jpg");
    if(!openedFile.isEmpty())
    {
        file = openedFile;
        this->update();
    }
}
