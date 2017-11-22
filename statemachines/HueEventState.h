#include <QtCore/QtCore>

class HueEventState : public QState
{
    Q_OBJECT
    
public:
    HueEventState(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*);
    void onExit(QEvent*);
};
