#include <HueInitState.h>

HueInitState::HueInitState(QState *parent) : QState(parent)
{
}

void HueInitState::onEntry(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void HueInitState::onExit(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}
