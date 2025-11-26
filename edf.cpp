#include "edf.h"
#include <algorithm>


EDF::EDF(QObject *parent)
    : AlgorithmBase("Earliest Deadline First (EDF)", parent)
{
}

void EDF::executeStep()
{
    if (m_queue.isEmpty())
        return;

    // ✅ Trier par deadline (la plus proche d'abord)
    if (m_queue.size() > 1) {
        std::sort(m_queue.begin() + 1, m_queue.end(),
                  [](const Process &a, const Process &b) {
                      // Si les deux ont des deadlines
                      if (a.hasDeadline() && b.hasDeadline()) {
                          return a.get_deadline() < b.get_deadline();
                      }
                      // Si seulement a a une deadline → a en premier
                      if (a.hasDeadline()) return true;
                      // Si seulement b a une deadline → b en premier
                      if (b.hasDeadline()) return false;
                      // Aucun n'a de deadline → FCFS
                      return a.get_arrival() < b.get_arrival();
                  });
    }

    Process &current = m_queue[0];

    // Premier accès au CPU
    if (!current.get_executed()) {
        int waitTime = m_clock - current.get_arrival();
        current.set_executed(waitTime);

        QString msg = QString("[%1ms] Process %2 accessed CPU (burst: %3ms")
            .arg(m_clock)
            .arg(current.get_pid())
            .arg(current.get_burst());

        if (current.hasDeadline()) {
            msg += QString(", deadline: %1ms)").arg(current.get_deadline());
        } else {
            msg += ")";
        }

        emit logMessage(msg);
    }

    // Exécute le processus
    int toDecrement = std::min(m_speed, current.get_burst_left());
    current.dec_burst(toDecrement);

    emit processExecuting(current);

    // Vérifie si terminé
    if (current.get_burst_left() == 0) {
        // ✅ Vérifier si deadline respectée
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

        if (!m_queue.isEmpty()) {
            m_stats.contextSwitches++;
        }
    }
}
