#include "paintbeziercurve.h"

// ---------------------------------
PaintBezierCurve::PaintBezierCurve(QWidget *parent) : QWidget(parent)
{
    InitCurrentPointList();
}

// ------------------------------------------
void PaintBezierCurve::InitCurrentPointList()
{
    /*
     * @brief 初始化离散点集合
     */
    QPoint temp;
    temp.setX(0);
    temp.setY(0);
    m_currentPointList.append(temp * 2);

    temp.setX(17);
    temp.setY(29);
    m_currentPointList.append(temp * 2);

    temp.setX(34);
    temp.setY(51);
    m_currentPointList.append(temp * 2);

    temp.setX(51);
    temp.setY(70);
    m_currentPointList.append(temp * 2);

    temp.setX(68);
    temp.setY(88);
    m_currentPointList.append(temp * 2);

    temp.setX(85);
    temp.setY(106);
    m_currentPointList.append(temp * 2);

    temp.setX(102);
    temp.setY(122);
    m_currentPointList.append(temp * 2);

    temp.setX(119);
    temp.setY(139);
    m_currentPointList.append(temp * 2);

    temp.setX(136);
    temp.setY(154);
    m_currentPointList.append(temp * 2);

    temp.setX(153);
    temp.setY(170);
    m_currentPointList.append(temp * 2);

    temp.setX(170);
    temp.setY(185);
    m_currentPointList.append(temp * 2);

    temp.setX(187);
    temp.setY(199);
    m_currentPointList.append(temp * 2);

    temp.setX(204);
    temp.setY(214);
    m_currentPointList.append(temp * 2);

    temp.setX(221);
    temp.setY(228);
    m_currentPointList.append(temp * 2);

    temp.setX(238);
    temp.setY(242);
    m_currentPointList.append(temp * 2);

    temp.setX(256);
    temp.setY(256);
    m_currentPointList.append(temp * 2);
}

// --------------------------------------------------------
void PaintBezierCurve::SetMathCoordinate(QPainter* painter)
{
    /*
     * @brief 转换为数学坐标系，原点在左下角
     */
    painter->setWindow(0,height(),width(),-height());
}

// -------------------------------------------------------------
void PaintBezierCurve::ResetDefaultCoordinate(QPainter* painter)
{
    /*
     * @brief 恢复默认坐标系，原点在左上角
     */
    painter->setWindow(0, 0, this->width(), this->height());
}

// --------------------------------------------------
void PaintBezierCurve::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿

    // 绘制曲线上的点
    QPen pen;
    pen.setColor(Qt::black);
    painter.setPen(pen);

    SetMathCoordinate(&painter);
    // 生成贝塞尔曲线
    m_bezierCurve = GenerateBezierSmoothCurve(m_currentPointList);
    painter.drawPath(m_bezierCurve);

    pen.setColor(Qt::black);
    pen.setWidth(4);
    painter.setPen(pen);
    for(int i = 0; i < m_currentPointList.size(); i++)
    {
        // 添加一个圆
        QPainterPath path;
        path.addEllipse(m_currentPointList.at(i), 4, 4);
        painter.drawPath(path);
    }
    GetCurvePoint(256);
}

// -----------------------------------------------------------------------------------
QPainterPath PaintBezierCurve::GenerateBezierSmoothCurve(const QList<QPointF> &points)
{
    /*
     * @brief 生成贝塞尔平滑曲线
     * @param [in] points 离散的点的 QList
     */
    QPainterPath path;
    int len = points.size();

    if (len < 2)
    {
        // 散点少于两个
        return path;
    }

    // 两个控制点
    QList<QPointF> firstControlPoints;
    QList<QPointF> secondControlPoints;

    // 计算控制点
    CalculateControlPoints(points, &firstControlPoints, &secondControlPoints);

    path.moveTo(points[0].x(), points[0].y());

    // 使用贝塞尔曲线生成平滑曲线
    for (int i = 0; i < len - 1; ++i)
    {
        path.cubicTo(firstControlPoints[i], secondControlPoints[i], points[i+1]);
    }
    return path;
}

// ---------------------------------------------------------------------
void PaintBezierCurve::CalculateControlPoints(const QList<QPointF> &knots,QList<QPointF> *firstControlPoints,QList<QPointF> *secondControlPoints)
{
    /*
     * @brief 计算控制点的坐标
     */
    int n = knots.size() - 1;

    // 初始化 n 个空的控制点
    for (int i = 0; i < n; ++i)
    {
        firstControlPoints->append(QPointF());
        secondControlPoints->append(QPointF());
    }

    if (n == 1)
    {
        // 特殊情况: 贝塞尔曲线是一条直线
        // P1 = (2P0 + P3) / 3
        (*firstControlPoints)[0].rx() = (2 * knots[0].x() + knots[1].x()) / 3;
        (*firstControlPoints)[0].ry() = (2 * knots[0].y() + knots[1].y()) / 3;

        // P2 = 2P1 – P0
        (*secondControlPoints)[0].rx() = 2 * (*firstControlPoints)[0].x() - knots[0].x();
        (*secondControlPoints)[0].ry() = 2 * (*firstControlPoints)[0].y() - knots[0].y();

        return;
    }

    // 计算第一个控制点
    double *xs = 0;
    double *ys = 0;
    double *rhsx = new double[n]; // Right hand side vector
    double *rhsy = new double[n]; // Right hand side vector

    // 设置右侧值
    for (int i = 1; i < n - 1; ++i)
    {
        rhsx[i] = 4 * knots[i].x() + 2 * knots[i + 1].x();
        rhsy[i] = 4 * knots[i].y() + 2 * knots[i + 1].y();
    }

    rhsx[0] = knots[0].x() + 2 * knots[1].x();

    rhsx[n - 1] = (8 * knots[n - 1].x() + knots[n].x()) / 2.0;

    rhsy[0] = knots[0].y() + 2 * knots[1].y();

    rhsy[n - 1] = (8 * knots[n - 1].y() + knots[n].y()) / 2.0;

    // 计算第一个控制点的坐标
    CalculateFirstControlPoints(xs, rhsx, n);
    CalculateFirstControlPoints(ys, rhsy, n);

    // Fill output control points.
    for (int i = 0; i < n; ++i)
    {
        (*firstControlPoints)[i].rx() = xs[i];
        (*firstControlPoints)[i].ry() = ys[i];

        if (i < n - 1)
        {
            (*secondControlPoints)[i].rx() = 2 * knots[i + 1].x() - xs[i + 1];
            (*secondControlPoints)[i].ry() = 2 * knots[i + 1].y() - ys[i + 1];
        }
        else
        {
            (*secondControlPoints)[i].rx() = (knots[n].x() + xs[n - 1]) / 2;
            (*secondControlPoints)[i].ry() = (knots[n].y() + ys[n - 1]) / 2;
        }
    }

    delete xs;
    delete ys;
    delete[] rhsx;
    delete[] rhsy;
}

// ------------------------------------------------------------------------------------------
void PaintBezierCurve::CalculateFirstControlPoints(double *&result, const double *rhs, int n)
{
    /*
     * @brief 计算第一个控制点
     */

    result = new double[n];
    double *tmp = new double[n];
    double b = 2.0;
    result[0] = rhs[0] / b;

    // 分解和前向替换
    for (int i = 1; i < n; i++)
    {
        tmp[i] = 1 / b;
        b = (i < n - 1 ? 4.0 : 3.5) - tmp[i];
        result[i] = (rhs[i] - result[i - 1]) / b;
    }

    for (int i = 1; i < n; i++)
    {
        result[n - i - 1] -= tmp[n - i] * result[n - i]; // Backsubstitution.
    }

    delete[] tmp;
}

// ------------------------------------------------
void PaintBezierCurve::mousePressEvent(QMouseEvent *event)
{
    /*
     * @breif 鼠标按下事件
     */
    m_pressedControlPointIndex = -1;

    // 绘制贝塞尔曲线和控制点的坐标系做了偏移，鼠标按下的坐标也要相应的偏移
    // QPointF p = translatedPoint(event->pos());
    QPointF p = event->pos();

    QPointF newP;
    newP.setX(p.x());
    newP.setY(height() - p.y());

    // y 轴的方向反了
    //qDebug() << "mousePressEvent" << p << "new P" << newP;

    // 鼠标按下时，选择被按住的控制点
    for (int i = 0; i < m_currentPointList.size(); ++i)
    {
        QPainterPath path;
        // 添加一个圆
        path.addEllipse(m_currentPointList.at(i), 6, 6);

        if (path.contains(newP))
        {
            // 鼠标按下的点在控制点的范围内
            m_pressedControlPointIndex = i;
            break;
        }
    }
}

// -----------------------------------------------
void PaintBezierCurve::mouseMoveEvent(QMouseEvent *event)
{
    /*
     * @brief 移动选中的控制点
     */

    QPointF p = event->pos();
    QPointF newP;
    newP.setX(p.x());
    newP.setY(height() - p.y());
    if (m_pressedControlPointIndex != -1)
    {

        // 更新端点(控制点)改为当前鼠标点， 不更新 x 轴的值
        //m_currentGammaCurvePointList[m_pressedControlPointIndex].setX(newP.x());
        m_currentPointList[m_pressedControlPointIndex].setY(newP.y());

        m_bezierCurve = GenerateBezierSmoothCurve(m_currentPointList); // 坐标发生变化后重新生成贝塞尔曲线
        update(); // 刷新界面
    }
}

// ---------------------------------------------------------
QList<QPointF> PaintBezierCurve::GetCurvePoint(int pointNum)
{
    /*
     * @brief 获取曲线上点的坐标
     * @brief pointNum 曲线上点的数量
     */
    QList<QPointF> res;
    for(int i = 0; i < pointNum; i++)
    {
        QPointF tempPoint = m_bezierCurve.pointAtPercent((i * 1.0) / pointNum);
        res.append(tempPoint);
        qDebug() << tempPoint;
    }
    return res;
}
