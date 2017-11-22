#include <HueOnState.h>

HueOnState::HueOnState(QState *parent) : QState(parent)
{
}

void HueOnState::onEntry(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void HueOnState::onExit(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}
