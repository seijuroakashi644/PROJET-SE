#ifndef CIRCULARQUEUEWIDGET_H
#define CIRCULARQUEUEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include "process.h"

// Widget qui affiche les processus en cercle
class CircularQueueWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CircularQueueWidget(QWidget *parent = nullptr);

    void setProcesses(const QVector<Process> &processes);
    void setExecutingProcess(const Process *process);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<Process> m_processes;
    const Process *m_executing;  // Pointeur vers le processus en cours

    // Calcule la position d'un processus sur le cercle
    QPoint getCirclePosition(int index, int total);

    // Dessine un processus
    void drawProcess(QPainter &painter, const Process &p, const QPoint &pos, bool isExecuting);

    // Rayon du cercle
    int m_radius;
};

#endif // CIRCULARQUEUEWIDGET_H
