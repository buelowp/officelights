#include <HueEventTimeout.h>

HueEventTimeout::HueEventTimeout(QState *parent) : QState(parent)
{
}

void HueEventTimeout::onEntry(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void HueEventTimeout::onExit(QEvent*)
{
    qDebug() << __PRETTY_FUNCTION__;
}
