#include "algorithmbase.h"
#include <QVector>
#include <QDebug>

AlgorithmBase :: AlgorithmBase(QString algorithmName , QObject *parent )  : QObject(parent),
    m_algorithmName(algorithmName),
    m_speed(50),
    m_clock(0),
    m_numCompleted(0),
    m_totalProcesses(0),
    m_isRunning(false),
    m_stats(algorithmName),
    m_lastPid(0)
{  m_timer = new QTimer(this);
connect(m_timer, &QTimer::timeout, this, &AlgorithmBase::tick);
}

void AlgorithmBase::start(QVector<Process> processes)
{
    m_processes = processes;
    m_originalProcesses = processes;
    m_queue.clear();
    m_clock = 0;
    m_numCompleted = 0;
    m_totalProcesses = processes.size();
    m_stats.reset();  // Réinitialise les stats
    m_lastPid = 0;

    emit logMessage(QString("Starting %1 algorithm...").arg(m_algorithmName));
    emit clockUpdated(m_clock);

    play();
}
void AlgorithmBase::play()
{
    if (!m_isRunning) {
        m_isRunning = true;
        m_timer->start(100);  // Tick tous les 100ms
        emit logMessage("Simulation resumed");
    }
}

void AlgorithmBase::pause()
{
    if (m_isRunning) {
        m_isRunning = false;
        m_timer->stop();
        emit logMessage("Simulation paused");
    }
}
void AlgorithmBase::reset()
{
    pause();
    m_processes = m_originalProcesses;
    m_queue.clear();
    m_clock = 0;
    m_numCompleted = 0;
    m_stats.reset();
    m_lastPid = 0;

    emit clockUpdated(m_clock);
    emit logMessage("Simulation reset");
}
void AlgorithmBase::setSpeed(int multiplier)
{
    m_speed = multiplier;
    emit logMessage(QString("Speed changed to %1x").arg(multiplier));
}
void AlgorithmBase::tick()
{

    // Avance l'horloge de simulation
    m_clock += m_speed;
    emit clockUpdated(m_clock);

    // Vérifie si des processus arrivent
    checkArrivals();

    // Exécute la logique spécifique de l'algo (FCFS, SJF, RR)
    executeStep();  // ← Appelle la fonction définie par la classe fille
}
void AlgorithmBase::checkArrivals()
{
    // Parcourt les processus pas encore arrivés (en ordre inverse pour pouvoir supprimer)
    for (int i = 0; i  < m_processes.size() ; i++) {

        // Si le temps d'arrivée est atteint
        if (m_processes[i].get_arrival() <= m_clock) {

            // Ajoute à la file d'attente
            m_queue.append(m_processes[i]);

            // Émet un signal pour l'interface
            emit processArrived(m_processes[i]);

            // Log
            emit logMessage(QString("[%1ms] Process %2 created (burst: %3ms, priority: %4)")
                .arg(m_clock)
                .arg(m_processes[i].get_pid())
                .arg(m_processes[i].get_burst())
                .arg(m_processes[i].get_priority()));

            // Retire de la liste des arrivants
            m_processes.removeAt(i);
        }
    }
}
int AlgorithmBase::getNumCompleted()
{
    return m_numCompleted;
}
void AlgorithmBase::recordCompletion(const Process &p)
{
    // Calcule les métriques
    int turnaround = m_clock - p.get_arrival();
    int totalWait = turnaround - p.get_burst();

    // Enregistre dans les statistiques (fait les comparaisons min/max automatiquement)
    m_stats.recordProcess(turnaround, totalWait, p.get_initial_wait());

    // Log
    emit logMessage(QString("[%1ms] Process %2 completed (turnaround: %3ms, wait: %4ms)")
        .arg(m_clock)
        .arg(p.get_pid())
        .arg(turnaround)
        .arg(totalWait));




    // Incrémente le compteur
    m_numCompleted++;
     // Émet un signal pour l'interface
    emit processCompleted(p, m_numCompleted);
    qDebug() << "processus terminés " << m_numCompleted;

    // Vérifie si tous les processus sont terminés
    if (m_numCompleted == m_totalProcesses) {
        pause();
        emit algorithmFinished();
        emit logMessage(QString("%1 algorithm completed!").arg(m_algorithmName));
    }
}
int AlgorithmBase::getclock()
{
    return m_clock;
}
bool AlgorithmBase :: get_isrunning()
{
    return m_isRunning;
}
