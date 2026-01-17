#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include "algorithmbase.h"

class round_robin : public AlgorithmBase
{
    Q_OBJECT
public:
    explicit round_robin(QObject *parent = nullptr ,int quantum = 50 );
    void getQuantum() const ;
    void setQuantum(int new_quantum);
protected:
    void executeStep() override;
private :
    int quantum;
    int quantum_used;

};

#endif // ROUND_ROBIN_H
