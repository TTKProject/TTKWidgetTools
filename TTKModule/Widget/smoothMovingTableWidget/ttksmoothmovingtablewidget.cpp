#include "ttksmoothmovingtablewidget.h"

#include <QTimer>
#include <QScrollBar>
#include <QWheelEvent>
#include <QPropertyAnimation>

TTKSmoothMovingTableWidget::TTKSmoothMovingTableWidget(QWidget *parent)
    : QTableWidget(parent),
      m_isFirstInit(true),
      m_previousValue(0),
      m_deltaValue(0),
      m_scrollBar(nullptr),
      m_slowAnimation(nullptr)
{
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(100 * TTK_DN_MS);

    connect(m_animationTimer, SIGNAL(timeout()), SLOT(updateRender()));
}

TTKSmoothMovingTableWidget::~TTKSmoothMovingTableWidget()
{
    m_animationTimer->stop();
    delete m_animationTimer;
    delete m_slowAnimation;
}

void TTKSmoothMovingTableWidget::setMovedScrollBar(QScrollBar *bar)
{
    m_scrollBar = bar;
    delete m_slowAnimation;
    m_slowAnimation = new QPropertyAnimation(m_scrollBar, "value", this);
    m_slowAnimation->setDuration(1000);
    connect(m_scrollBar, SIGNAL(valueChanged(int)), SLOT(valueChanged(int)));
}

QSize TTKSmoothMovingTableWidget::sizeHint() const
{
    return QSize(200, 200);
}

void TTKSmoothMovingTableWidget::updateRender()
{
    m_isFirstInit = true;
    m_animationTimer->stop();

    const float delta = (rowCount() > 0) ? (height()*1.0/rowCount()) : 0;
    m_deltaValue = (m_deltaValue/480.0)*(m_deltaValue < 0 ? m_deltaValue + 120 : -m_deltaValue + 120);

    m_slowAnimation->setStartValue(m_previousValue);
    m_slowAnimation->setEndValue(m_scrollBar->value() + m_deltaValue*delta/30);
    m_slowAnimation->start();
}

void TTKSmoothMovingTableWidget::valueChanged(int value)
{
    m_previousValue = value;
}

void TTKSmoothMovingTableWidget::wheelEvent(QWheelEvent *event)
{
    QTableWidget::wheelEvent(event);

    if(!m_slowAnimation)
    {
        return;
    }

    m_animationTimer->stop();
    m_slowAnimation->stop();
    if(m_isFirstInit)
    {
        m_deltaValue = 0;
        m_previousValue = m_scrollBar->value();
        m_isFirstInit = false;
    }

    m_deltaValue += QtWheelDelta(event);
    m_animationTimer->start();
}
