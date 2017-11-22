#include <QtCore/QtCore>

class HueEventTimeout : public QState
{
    Q_OBJECT
    
public:
    HueEventTimeout(QState *parent = Q_NULLPTR);
    
protected:
    void onEntry(QEvent*);
    void onExit(QEvent*);
};
