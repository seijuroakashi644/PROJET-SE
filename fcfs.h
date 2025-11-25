#ifndef FCFS_H
#define FCFS_H

#include "algorithmbase.h"


class fcfs : public AlgorithmBase
{
    Q_OBJECT
public:
    explicit fcfs(QObject *parent = nullptr);
private :
    void executeStep() override;
};

#endif // FCFS_H
