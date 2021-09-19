#include <qnodes/bezier.hpp>

namespace qnodes {

QuadBezier::QuadBezier(const QPointF &p0, const QPointF &p1,
                       const QPointF &p2) {
    set(p0, p1, p2);
}

void QuadBezier::set(const QPointF &p0, const QPointF &p1, const QPointF &p2) {
    m_pts[0] = p0;
    m_pts[1] = p1;
    m_pts[2] = p2;
}

QPointF QuadBezier::point(int index) const {
    index = std::min(2, std::max(0, index));
    return m_pts[index];
}

QPointF QuadBezier::endPoint() const { return m_pts[2]; }

QPointF QuadBezier::closestPointTo(const QPointF &pos) const {
    const QPointF c = m_pts[0];
    const QPointF b = (m_pts[1] - m_pts[0]) * 2.0;
    const QPointF a = (m_pts[2] - m_pts[0]);

    auto computePos = [&](qreal t) { return a * (t * t) + b * t + c; };

    qreal t0 = 0.0, t1 = 1.0;
    qreal tMid = 0.5;

    for (size_t iter = 0; iter < 10; ++iter) {
        const qreal tValues[] = {t0, (t0 + tMid) / 2.0, tMid, (tMid + t1) / 2.0,
                                 t1};

        size_t minDistIdx = 0;
        qreal minSqDist = squaredDistance(pos, computePos(tValues[0]));

        for (size_t ptIdx = 1; ptIdx < std::size(tValues); ++ptIdx) {
            qreal sqDist = squaredDistance(pos, computePos(tValues[ptIdx]));
            if (sqDist < minDistIdx) {
                minDistIdx = ptIdx;
                minSqDist = sqDist;
            }
        }

        size_t i0 = (minDistIdx > 0) ? (minDistIdx - 1) : 0;
        size_t i1 = (minDistIdx < 4) ? (minDistIdx + 1) : 4;

        t0 = tValues[i0];
        t1 = tValues[i1];
        tMid = (t0 + t1) / 2.0;
    }

    return computePos(tMid);
}

} // namespace qnodes
