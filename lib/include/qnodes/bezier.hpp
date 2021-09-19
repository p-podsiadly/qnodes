#ifndef QNODES_BEZIER_HPP_INCLUDED
#define QNODES_BEZIER_HPP_INCLUDED

#include <QRectF>

namespace qnodes {

class QuadBezier {
public:
    QuadBezier() = default;
    QuadBezier(const QuadBezier &) = default;
    QuadBezier(const QPointF &p0, const QPointF &p1, const QPointF &p2);

    QuadBezier &operator=(const QuadBezier &) = default;

    void set(const QPointF &p0, const QPointF &p1, const QPointF &p2);

    QPointF point(int index) const;
    QPointF endPoint() const;

    QPointF closestPointTo(const QPointF &pos) const;

private:
    QPointF m_pts[3];
};

inline qreal squaredDistance(const QPointF &p0, const QPointF &p1) {
    const QPointF v = p0 - p1;
    return QPointF::dotProduct(v, v);
}

} // namespace qnodes

#endif // QNODES_BEZIER_HPP_INCLUDED
