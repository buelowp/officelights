#include <HueTransitionToOffState.h>

HueTransitionToOffState::HueTransitionToOffState(QState *parent) : QState(parent)
{
}

void HueTransitionToOffState::onEntry(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void HueTransitionToOffState::onExit(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}
