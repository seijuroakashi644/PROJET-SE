#ifndef STATISTICS_H
#define STATISTICS_H

#include <QString>
struct Statistics
{
    QString algorithmName ;
    double totalTurnaround;     // Somme des turnaround times
    double totalWait;           // Somme des temps d'attente totaux
    double totalInitialWait;    // Somme des temps d'attente initiaux

        // Min et Max
    double minTurnaround;
        double maxTurnaround;
        double minWait;
        double maxWait;
        double minInitialWait;
        double maxInitialWait;

        int numProcesses ;
        int contextSwitches;


        // Constructeur avec valeurs par défaut
        Statistics(QString name = "Unknown")
            : algorithmName(name),
              totalTurnaround(0.0),
              totalWait(0.0),
              totalInitialWait(0.0),
              minTurnaround(999999.0),
              maxTurnaround(0.0),
              minWait(999999.0),
              maxWait(0.0),
              minInitialWait(999999.0),
              maxInitialWait(0.0),
              numProcesses(0),
              contextSwitches(0)
        {}
        // Calcule les moyennes
           double getAvgTurnaround() const {
               return numProcesses > 0 ? totalTurnaround / numProcesses : 0.0;
           }

           double getAvgWait() const {
               return numProcesses > 0 ? totalWait / numProcesses : 0.0;
           }

           double getAvgInitialWait() const {
               return numProcesses > 0 ? totalInitialWait / numProcesses : 0.0;
           }
           double getTotalTurnaround() const {
               return  totalTurnaround;
           }
           // Enregistre les métriques d'un processus terminé
           void recordProcess(int turnaround, int totalWaitTime, int initialWait) {
               numProcesses++;

               // Turnaround time
               totalTurnaround += turnaround;
               if (turnaround < minTurnaround) minTurnaround = turnaround;
               if (turnaround > maxTurnaround) maxTurnaround = turnaround;

               // Total wait time
               totalWait += totalWaitTime;
               if (totalWaitTime < minWait) minWait = totalWaitTime;
               if (totalWaitTime > maxWait) maxWait = totalWaitTime;

               // Initial wait time
               totalInitialWait += initialWait;
               if (initialWait < minInitialWait) minInitialWait = initialWait;
               if (initialWait > maxInitialWait) maxInitialWait = initialWait;
           }

           // Réinitialise toutes les statistiques
           void reset() {
               totalTurnaround = 0.0;
               totalWait = 0.0;
               totalInitialWait = 0.0;
               minTurnaround = 999999.0;
               maxTurnaround = 0.0;
               minWait = 999999.0;
               maxWait = 0.0;
               minInitialWait = 999999.0;
               maxInitialWait = 0.0;
               numProcesses = 0;
               contextSwitches = 0;
           }


};

#endif // STATISTICS_H
