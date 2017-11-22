#include <QtCore/QtCore>

class HueOffState : public QState
{
    Q_OBJECT
    
public:
    HueOffState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*);
    void onExit(QEvent*);
};
