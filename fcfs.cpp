#include "fcfs.h"
#include "algorithmbase.h"
#include <QDebug>
fcfs::fcfs(QObject *parent):AlgorithmBase("First Come First Serve" , parent){}
void fcfs::executeStep()
{
    // Si la file est vide, on ne fait rien
    if (m_queue.isEmpty())
        return;

    // FCFS = toujours exécuter le PREMIER processus de la file
    Process &current = m_queue[0];  // Référence au premier processus

    // === PREMIER ACCÈS AU CPU ===
    if (!current.get_executed()) {
        int waitTime = m_clock - current.get_arrival();
        current.set_executed(waitTime);
        qDebug() << m_clock << "ms  Process " << current.get_pid() << "  accessed CPU (initial wait: " <<waitTime << "ms " ;
        emit logMessage(QString("[%1ms] Process %2 accessed CPU (initial wait: %3ms)")
            .arg(m_clock)
            .arg(current.get_pid())
            .arg(waitTime));
    }

    // === EXÉCUTION ===
    // Décrémente le burst de m_speed ms (selon la vitesse)
    int decremente_value = std::min(m_speed , current.get_burst_left());
    current.dec_burst(decremente_value);

    // Notifie l'interface que ce processus est en cours d'exécution
    emit processExecuting(current);

    // === VÉRIFICATION DE COMPLÉTION ===
    if (current.get_burst_left() == 0) {
        // Processus terminé !
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

        // Enregistre les stats (min/max, moyennes, etc.)
        recordCompletion(current);

        // Sauvegarde le PID pour le context switch
        m_lastPid = current.get_pid();

        // Retire le processus de la file
        m_queue.removeFirst();

        // Si il reste des processus, compte un context switch
        if (!m_queue.isEmpty()) {
            m_stats.contextSwitches++;
        }
    }
}
