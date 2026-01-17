#ifndef ALGORITHMRECOMMENDER_H
#define ALGORITHMRECOMMENDER_H

#include <QObject>
#include <QVector>
#include "process.h"
#include "statistics.h"

class AlgorithmBase;

class AlgorithmRecommender : public QObject
{
    Q_OBJECT

public:
    struct Recommendation {
        QString algorithmName;
        int algoIndex;  // 0=FCFS, 1=SJF, 2=RR, 3=RR_Multi, 4=EDF
        double avgWait;
        double avgTurnaround;
        int deadlinesRespected;
        int deadlinesMissed;
        int contextSwitches;
        double score;  // Score global (0-100)

        QString getDeadlineText() const {
            int total = deadlinesRespected + deadlinesMissed;
            if (total == 0) return "N/A";
            return QString("%1/%2 (%3%)")
                .arg(deadlinesRespected)
                .arg(total)
                .arg((deadlinesRespected * 100.0 / total), 0, 'f', 0);
        }

        QString getScoreColor() const {
            if (score >= 80) return "#2ecc71";  // Vert
            if (score >= 60) return "#f39c12";  // Orange
            return "#e74c3c";  // Rouge
        }
    };

    explicit AlgorithmRecommender(QObject *parent = nullptr);

    // Analyse tous les algos avec la nouvelle commande
    QVector<Recommendation> analyzeNewOrder(
        const QVector<Process>& currentQueue,
        const Process* runningProcess,
        const Process& newOrder,
        int currentClock
    );

    // Retourne la meilleure recommandation
    Recommendation getBestRecommendation(const QVector<Recommendation>& results);

private:
    // Simule un algorithme spécifique en mode rapide
    Statistics simulateAlgorithm(
        int algoIndex,
        QVector<Process> processes,
        int startClock
    );

    // Calcule le score d'un algorithme (0-100)
    double calculateScore(const Statistics& stats, int totalProcesses);

    // Clone les processus pour simulation
    QVector<Process> cloneProcesses(const QVector<Process>& original);
};

#endif // ALGORITHMRECOMMENDER_H
