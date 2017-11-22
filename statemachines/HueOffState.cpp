#include <HueOffState.h>

HueOffState::HueOffState(QState *parent) : QState(parent)
{
}

void HueOffState::onEntry(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void HueOffState::onExit(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}
