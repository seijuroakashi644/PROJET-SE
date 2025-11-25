#ifndef SJF_H
#define SJF_H
#include "algorithmbase.h"

class sjf : public AlgorithmBase
{
    Q_OBJECT
public:
    sjf(QObject *parent = nullptr);
protected :
    void executeStep() override;
};

#endif // SJF_H
