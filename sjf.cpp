#include "sjf.h"
#include <algorithm>
#include <QDebug>


sjf::sjf(QObject *parent):AlgorithmBase("Shortest Job First (SJF)", parent)
{

}

void sjf::executeStep()
{
    if (m_queue.isEmpty()){

        qDebug() << "queue is empty";
           return;}



       // ✅ LA DIFFÉRENCE : Trier par burst_time (sauf le premier qui est déjà en cours)
       if (m_queue.size() > 1) {
           std::sort(m_queue.begin() + 1, m_queue.end(),
                     [](const Process &a, const Process &b) {
                         return a.get_burst() < b.get_burst();  // Plus court d'abord
                     });
       }

       // Le reste est IDENTIQUE à FCFS
       Process &current = m_queue[0];

       // Premier accès au CPU
       if (!current.get_executed()) {
           int waitTime = m_clock - current.get_arrival();
           current.set_executed(waitTime);
           emit logMessage(QString("[%1ms] Process %2 accessed CPU (initial wait: %3ms)")
               .arg(m_clock)
               .arg(current.get_pid())
               .arg(waitTime));
       }

       // Décrémente de m_speed (avec protection)
       int toDecrement = std::min(m_speed, current.get_burst_left());
       current.dec_burst(toDecrement);

       emit processExecuting(current);

       // Vérifie si terminé
       if (current.get_burst_left() == 0) {
           recordCompletion(current);
           m_lastPid = current.get_pid();
           m_queue.removeFirst();

           if (!m_queue.isEmpty()) {
               m_stats.contextSwitches++;
           }
       }
}
