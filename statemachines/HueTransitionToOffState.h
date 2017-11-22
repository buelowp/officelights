#include <QtCore/QtCore>

class HueTransitionToOffState : public QState
{
    Q_OBJECT
    
public:
    HueTransitionToOffState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*);
    void onExit(QEvent*);
};
