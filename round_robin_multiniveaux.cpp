#include "round_robin_multiniveaux.h"
#include "algorithmbase.h"
round_robin_multiniveaux::round_robin_multiniveaux(QObject *parent , int quantum):AlgorithmBase("Round Robin multiniveaux" , parent),quantum(50), quantum_used(0)
{

}
void round_robin_multiniveaux::executeStep()
{
    if (m_queue.isEmpty())
        return;

    // ✅ TRI uniquement les processus EN ATTENTE (pas celui en cours)
    if (m_queue.size() > 1) {
        std::sort(m_queue.begin() + 1, m_queue.end(),
                  [](const Process &a, const Process &b) {
                      // Plus haute priorité d'abord
                      if (a.get_priority() != b.get_priority()) {
                          return a.get_priority() < b.get_priority();
                      }
                      // Si même priorité, FCFS
                      return a.get_arrival() < b.get_arrival();
                  });
    }

    Process &current = m_queue[0];

    // Premier accès au CPU
    if (!current.get_executed()) {
        int waitTime = m_clock - current.get_arrival();
        current.set_executed(waitTime);
        quantum_used= 0;

        emit logMessage(QString("[%1ms] Process %2 (priority %3) accessed CPU")
            .arg(m_clock)
            .arg(current.get_pid())
            .arg(current.get_priority()));
    }

    // Exécute le processus
    int toDecrement = std::min(m_speed, current.get_burst_left());
    current.dec_burst(toDecrement);
    quantum_used += m_speed;

    emit processExecuting(current);

    // CAS 1 : Processus terminé
    if (current.get_burst_left() == 0) {
        recordCompletion(current);
        m_queue.removeFirst();
        quantum_used = 0;

        // ✅ Le prochain processus sera automatiquement le plus prioritaire
        // car la queue est triée
    }
    // CAS 2 : Time slice écoulé
    else if (quantum_used >= quantum) {
        emit logMessage(QString("[%1ms] Process %2 time slice expired")
            .arg(m_clock)
            .arg(current.get_pid()));

        // ✅ Déplace à la fin
        m_queue.append(current);
        m_queue.removeFirst();

        quantum_used = 0;

        // ✅ Au prochain tick, la queue sera retriée
        // Le processus le plus prioritaire passera en [0]
    }
}
