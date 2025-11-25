#ifndef ROUND_ROBIN_MULTINIVEAUX_H
#define ROUND_ROBIN_MULTINIVEAUX_H

#include "algorithmbase.h"

class round_robin_multiniveaux:public AlgorithmBase
{
    Q_OBJECT
public:
    explicit round_robin_multiniveaux(QObject *parent = nullptr ,int quantum = 50  );
    void getQuantum() const ;
    void setQuantum(int new_quantum);
protected:
    void executeStep() override;
private :
    int quantum;
    int quantum_used;
    void sortQueueByPriority();
};

#endif // ROUND_ROBIN_MULTINIVEAUX_H
