#include <QtCore/QtCore>

class HueInitState : public QState
{
    Q_OBJECT
    
public:
    HueInitState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*);
    void onExit(QEvent*);
};
