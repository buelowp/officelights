#include <HueEventState.h>

HueEventState::HueEventState(QState *parent) : QState(parent)
{
}

void HueEventState::onEntry(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void HueEventState::onExit(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}
