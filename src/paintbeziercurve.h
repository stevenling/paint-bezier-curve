#ifndef PAINTBEZIERCURVE_H
#define PAINTBEZIERCURVE_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QVector>
#include <QtMath>
#include <QList>
#include <QMouseEvent>

// @class 绘制贝塞尔曲线
// -----------------------------------
class PaintBezierCurve: public QWidget
{
public:
    PaintBezierCurve(QWidget *parent);

    /// @brief 绘制
    virtual void paintEvent(QPaintEvent *event);

    /// @brief 设置为数学坐标系
    void SetMathCoordinate(QPainter* painter);

    /// @brief 恢复默认坐标系
    void ResetDefaultCoordinate(QPainter* painter);

    /// @brief 生成贝塞尔平滑曲线
    QPainterPath GenerateBezierSmoothCurve(const QList<QPointF> &points);

    /// @brief 计算控制点
    void CalculateControlPoints(const QList<QPointF> &knots,QList<QPointF> *firstControlPoints,QList<QPointF> *secondControlPoints);

    /// @brief 计算第一个控制点
    void CalculateFirstControlPoints(double *&result, const double *rhs, int n);

    /// @brief 初始化当前离散点的集合
    void InitCurrentPointList();

    /// @brief 获取曲线上的点坐标
    QList<QPointF> GetCurvePoint(int pointNum);
protected:
    /// @brief 鼠标事件
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QPainterPath m_bezierCurve;         // 贝塞尔曲线
    QList<QPointF> m_currentPointList;  // 离散的点的集合
    int m_pressedControlPointIndex;
};

#endif // PAINTBEZIERCURVE_H
