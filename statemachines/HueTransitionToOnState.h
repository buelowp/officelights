#include <QtCore/QtCore>

class HueTransitionToOnState : public QState
{
    Q_OBJECT
    
public:
    HueTransitionToOnState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*);
    void onExit(QEvent*);
};
