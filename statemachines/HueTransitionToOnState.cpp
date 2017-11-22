#include <HueTransitionToOnState.h>

HueTransitionToOnState::HueTransitionToOnState(QState *parent) : QState(parent)
{
}

void HueTransitionToOnState::onEntry(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void HueTransitionToOnState::onExit(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}
