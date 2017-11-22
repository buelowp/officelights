#include <QtCore/QtCore>

class HueOnState : public QState
{
    Q_OBJECT
    
public:
    HueOnState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*);
    void onExit(QEvent*);
};
