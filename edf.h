#ifndef EDF_H
#define EDF_H
#include "algorithmbase.h"


class EDF : public AlgorithmBase
{
     Q_OBJECT
public:
    explicit EDF(QObject *parent = nullptr);
   protected:
       void executeStep() override;
};

#endif // EDF_H
