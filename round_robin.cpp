#include "round_robin.h"
#include <QDebug>


round_robin::round_robin( QObject *parent , int quantum ):AlgorithmBase("Round Robin (RR)", parent),
    quantum(50)
{ }

void round_robin::executeStep()
{
    if (m_queue.isEmpty())
        return;

    Process &current = m_queue[0];
     qDebug() <<  "pid "  << current.get_pid();
    // Premier accès au CPU
    if (!current.get_executed()) {
        qDebug() << "new" ;
        int waitTime = m_clock - current.get_arrival();
        current.set_executed(waitTime);
        quantum_used= 0;  // Réinitialise le compteur de slice

        emit logMessage(QString("[%1ms] Process %2 accessed CPU (initial wait: %3ms)")
            .arg(m_clock)
            .arg(current.get_pid())
            .arg(waitTime));
    }

    // Exécute le processus

    int toDecrement = std::min(m_speed, current.get_burst_left());
    current.dec_burst(m_speed);
    quantum_used += toDecrement;
    qDebug() <<  "quantum used vaut " << quantum_used;



    //quantum += m_speed;  // Avance le compteur de slice

    emit processExecuting(current);

    // ✅ CAS 1 : Processus terminé
    if (current.get_burst_left() == 0) {
        if (current.hasDeadline()) {
            if (m_clock <= current.get_deadline()) {
                m_stats.deadlinesRespected++;
                emit logMessage("✅ Deadline respectée");
                emit logMessage(QString("✅ [%1ms] Process %2 completed ON TIME (deadline: %3ms)")
                    .arg(m_clock)
                    .arg(current.get_pid())
                    .arg(current.get_deadline()));
            } else {
                m_stats.deadlinesMissed++;  // ← AJOUTE
                emit logMessage("❌ Deadline manquée");
                int lateness = m_clock - current.get_deadline();
                emit logMessage(QString("❌ [%1ms] Process %2 MISSED DEADLINE by %3ms (was: %4ms)")
                    .arg(m_clock)
                    .arg(current.get_pid())
                    .arg(lateness)
                    .arg(current.get_deadline()));
            }
        }

        recordCompletion(current);
        m_lastPid = current.get_pid();
        m_queue.removeFirst();
        quantum_used = 0;  // Réinitialise pour le prochain

        if (!m_queue.isEmpty()) {
            m_stats.contextSwitches++;
        }
    }
    // ✅ CAS 2 : Time slice écoulé (et processus pas terminé)
    else if (quantum_used >= quantum) {
        emit logMessage(QString("[%1ms] Process %2 time slice expired, moving to back of queue")
            .arg(m_clock)
            .arg(current.get_pid()));

        m_lastPid = current.get_pid();

        // Déplace le processus à la fin de la file
        m_queue.append(current);
        m_queue.removeFirst();

        quantum_used = 0;  // Réinitialise le compteur
        m_stats.contextSwitches++;
    }
}
