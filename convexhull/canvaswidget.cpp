#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <algorithm> // For std::sort
#include <QSet>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget{parent}, m_slowIterations(0), m_fastIterations(0)
{}

void CanvasWidget::clearCanvas()
{
    m_points.clear();
    m_hullPoints.clear();
    m_slowIterations = 0;
    m_fastIterations = 0;
    update(); // Schedule a repaint
}

void CanvasWidget::computeConvexHull()
{
    if (m_points.size() < 3) {
        m_hullPoints.clear();
        m_slowIterations = 0;
        m_fastIterations = 0;
        update();
        return;
    }

    // Run both algorithms on the same dataset
    slowConvexHull(m_points, m_slowIterations); // Run for iteration count
    m_hullPoints = monotoneChain(m_points, m_fastIterations); // Run and get hull for drawing

    update(); // Schedule a repaint to show the hull and text
}


void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    // Add a point where the user clicked
    m_points.append(event->pos());
    update(); // Schedule a repaint to show the new point
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Draw all the points
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    for (const QPoint& p : m_points) {
        painter.drawEllipse(p, 3, 3);
    }

    // 2. Draw the convex hull polygon if it has been calculated
    if (!m_hullPoints.isEmpty()) {
        painter.setPen(QPen(Qt::red, 2));
        painter.setBrush(Qt::NoBrush);
        QPolygon polygon(m_hullPoints);
        painter.drawPolygon(polygon);
    }

    // 3. Wrote the iteration numbers on the canvas
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 12));
    QString slowText = QString("Slow Algorithm Iterations: %1").arg(m_slowIterations);
    QString fastText = QString("Fast Algorithm Iterations: %1").arg(m_fastIterations);
    painter.drawText(10, 20, slowText);
    painter.drawText(10, 40, fastText);
}

// --- Algorithm Implementations ---

// Helper function to find orientation of ordered triplet (a, b, c).
// Returns > 0 for counter-clockwise turn (left turn)
// Returns < 0 for clockwise turn (right turn)
// Returns 0 for collinear points
long long CanvasWidget::cross_product(QPoint a, QPoint b, QPoint c)
{
    // Using long long to avoid overflow
    return (long long)(b.x() - a.x()) * (c.y() - a.y()) -
           (long long)(b.y() - a.y()) * (c.x() - a.x());
}

// O(n^3) - Brute-force algorithm
QVector<QPoint> CanvasWidget::slowConvexHull(const QVector<QPoint>& points, int& iterations)
{
    iterations = 0;
    int n = points.size();
    if (n < 3) return {};

    QSet<int> hullPointIndices;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            bool isEdge = true;
            int firstSide = 0;

            for (int k = 0; k < n; ++k) {
                if (k == i || k == j) continue;

                iterations++; // Count the main check operation
                long long cp = cross_product(points[i], points[j], points[k]);

                if (firstSide == 0 && cp != 0) {
                    firstSide = (cp > 0) ? 1 : -1;
                } else if ((firstSide == 1 && cp < 0) || (firstSide == -1 && cp > 0)) {
                    isEdge = false;
                    break;
                }
            }

            if (isEdge) {
                hullPointIndices.insert(i);
                hullPointIndices.insert(j);
            }
        }
    }

    QVector<QPoint> hull;
    for (int index : hullPointIndices) {
        hull.append(points[index]);
    }
    return hull; // Note: This doesn't return points in order for drawing a polygon
}


// O(n log n) - Monotone Chain (Andrew's Algorithm)
QVector<QPoint> CanvasWidget::monotoneChain(const QVector<QPoint>& points, int& iterations)
{
    iterations = 0;
    int n = points.size();
    if (n <= 3) return points;

    QVector<QPoint> sortedPoints = points;
    // Sort points lexicographically (by x, then by y)
    std::sort(sortedPoints.begin(), sortedPoints.end(), [](QPoint a, QPoint b) {
        return a.x() < b.x() || (a.x() == b.x() && a.y() < b.y());
    });

    QVector<QPoint> hull;

    // Build lower hull
    for (int i = 0; i < n; ++i) {
        while (hull.size() >= 2) {
            iterations++; // Count the cross-product check
            if (cross_product(hull[hull.size()-2], hull.back(), sortedPoints[i]) > 0) {
                break; // Left turn, keep point
            }
            hull.pop_back(); // Right turn, remove point
        }
        hull.push_back(sortedPoints[i]);
    }

    // Build upper hull
    // The last point of the lower hull is the first of the upper hull
    int lowerHullSize = hull.size();
    for (int i = n - 2; i >= 0; --i) {
        while (hull.size() > lowerHullSize) {
            iterations++; // Count the cross-product check
            if (cross_product(hull[hull.size()-2], hull.back(), sortedPoints[i]) > 0) {
                break; // Left turn, keep point
            }
            hull.pop_back(); // Right turn, remove point
        }
        hull.push_back(sortedPoints[i]);
    }

    // The last point is the same as the first, remove it.
    hull.pop_back();

    return hull;
}
