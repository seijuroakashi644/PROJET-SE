#ifndef ALGORITHMBASE_H
#define ALGORITHMBASE_H

#include "process.h"
#include "statistics.h"
#include <QObject>
#include <QTimer>



class AlgorithmBase : public QObject
{
        Q_OBJECT
signals:
        void processArrived(Process p);       // Quand un processus arrive
        void processExecuting(Process p);       // Quand un processus s'exécute
        void processCompleted(Process p, int numcompleted);  // Quand terminé
        void clockUpdated(int time);            // Mise à jour horloge
        void algorithmFinished();               // Algo terminé
        void logMessage(QString message);

public:
    explicit AlgorithmBase(QString algorithName , QObject *parent = nullptr);
    virtual ~AlgorithmBase() {}
    void start(QVector<Process> processes);
    int getclock();

   // Contrôles de lecture
   void play();                    // Lance/reprend la simulation
   void pause();                   // Met en pause
   void reset();                   // Réinitialise tout
   void setSpeed(int multiplier);  // Change la vitesse (1x, 5x, 10x, etc.)

   // Récupère les statistiques finales
   Statistics getStatistics() const { return m_stats; }

   // Récupère le nom de l'algorithme
   QString getAlgorithmName() const { return m_algorithmName; }
   int getNumCompleted();
   bool get_isrunning();
   QVector<Process> getQueue() const { return m_queue;}
protected:


    // Chaque algo doit définir sa propre logique ici
    // = 0 signifie "cette fonction est obligatoire dans les classes filles"
    virtual void executeStep() = 0;

    // === ATTRIBUTS PROTÉGÉS (accessibles par les classes filles) ===

    QString m_algorithmName;            // Nom de l'algo (ex: "FCFS")
    QTimer *m_timer;                    // Timer pour la simulation
    int m_speed;                        // Vitesse de simulation (ex: 10 = 10ms par tick)
    int m_clock;                        // Horloge en millisecondes

    QVector<Process> m_processes;       // Processus pas encore arrivés
    QVector<Process> m_queue;           // File d'attente (ready queue)

    int m_numCompleted;                 // Nombre de processus terminés
    int m_totalProcesses;               // Nombre total de processus
    bool m_isRunning;                   // true si simulation en cours

    QVector<Process> m_originalProcesses;  // Sauvegarde pour reset
    Statistics m_stats;                 // Statistiques de cet algo
    int m_lastPid;                      // PID du dernier processus exécuté

    // === MÉTHODES UTILITAIRES (utilisables par les classes filles) ===

    void checkArrivals();               // Vérifie si des processus arrivent
    void recordCompletion(const Process &p);  // Enregistre la fin d'un processus

private slots:
    // SLOT privé appelé automatiquement par le timer
    void tick();  // Fait avancer la simulation d'un pas
};


#endif // ALGORITHMBASE_H
