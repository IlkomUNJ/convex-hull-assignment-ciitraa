#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPoint>

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

    // Public slots to be called from the main window
public slots:
    void clearCanvas();
    void computeConvexHull();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    // Data storage
    QVector<QPoint> m_points;
    QVector<QPoint> m_hullPoints;

    // Iteration counters
    int m_slowIterations;
    int m_fastIterations;

    // Algorithm implementations
    QVector<QPoint> slowConvexHull(const QVector<QPoint>& points, int& iterations);
    QVector<QPoint> monotoneChain(const QVector<QPoint>& points, int& iterations);

    // Helper for algorithms
    static long long cross_product(QPoint a, QPoint b, QPoint c);
};

#endif // CANVASWIDGET_H
