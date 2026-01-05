#include "circularqueuewidget.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QtMath>

CircularQueueWidget::CircularQueueWidget(QWidget *parent)
    : QWidget(parent),
      m_executing(nullptr),
      m_radius(150)  // Rayon du cercle
{
    setMinimumSize(400, 400);
}

void CircularQueueWidget::setProcesses(const QVector<Process> &processes)
{
    m_processes = processes;
    update();  // Redessine
}

void CircularQueueWidget::setExecutingProcess(const Process *process)
{
    m_executing = process;
    update();
}

void CircularQueueWidget::clear()
{
    m_processes.clear();
    m_executing = nullptr;
    update();
}

void CircularQueueWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Centre du widget
    QPoint center(width() / 2, height() / 2);

    // Dessine le cercle de guidage (optionnel)
    painter.setPen(QPen(QColor(200, 200, 200), 2, Qt::DashLine));
    painter.drawEllipse(center, m_radius, m_radius);

    // Dessine "Machine" au centre
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QColor(52, 152, 219));
    painter.drawEllipse(center, 50, 50);

    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(QRect(center.x() - 50, center.y() - 15, 100, 30),
                     Qt::AlignCenter, "Machine");

    // Dessine les processus en cercle
    int total = m_processes.size();
    for (int i = 0; i < total; i++) {
        QPoint pos = getCirclePosition(i, total);
        bool isExecuting = (m_executing != nullptr &&
                           m_processes[i].get_pid() == m_executing->get_pid());
        drawProcess(painter, m_processes[i], pos, isExecuting);
    }
}

QPoint CircularQueueWidget::getCirclePosition(int index, int total)
{
    if (total == 0) return QPoint(width() / 2, height() / 2);

    // Centre
    QPoint center(width() / 2, height() / 2);

    // Angle pour chaque processus
    // Commence en haut (270°) et tourne dans le sens horaire
    double angleStep = 360.0 / total;
    double angle = 270 + (index * angleStep);  // 270° = haut

    // Convertit en radians
    double radians = qDegreesToRadians(angle);

    // Position sur le cercle
    int x = center.x() + m_radius * qCos(radians);
    int y = center.y() + m_radius * qSin(radians);

    return QPoint(x, y);
}

void CircularQueueWidget::drawProcess(QPainter &painter, const Process &p,
                                     const QPoint &pos, bool isExecuting)
{
    // Taille du carré
    int size = 60;
    QRect rect(pos.x() - size/2, pos.y() - size/2, size, size);

    // Couleur selon l'état
    QColor bgColor;
    QPen borderPen;

    if (isExecuting) {
        bgColor = QColor(231, 76, 60);  // Rouge (en cours)
        borderPen = QPen(QColor(192, 57, 43), 4);
    } else {
        bgColor = QColor(149, 165, 166);  // Gris (en attente)
        borderPen = QPen(QColor(127, 140, 141), 2);
    }

    // Dessine le carré
    painter.setPen(borderPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(rect, 8, 8);

    // Texte
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(9);
    font.setBold(true);
    painter.setFont(font);

    // PID
    painter.drawText(rect.adjusted(0, 5, 0, 0), Qt::AlignHCenter | Qt::AlignTop,
                    QString("P%1").arg(p.get_pid()));

    // Burst left
    font.setPointSize(8);
    font.setBold(false);
    painter.setFont(font);
    painter.drawText(rect.adjusted(0, 0, 0, -5), Qt::AlignHCenter | Qt::AlignBottom,
                    QString("%1ms").arg(p.get_burst_left()));

    // Flèche vers le centre si en cours d'exécution
    if (isExecuting) {
        QPoint center(width() / 2, height() / 2);
        painter.setPen(QPen(QColor(231, 76, 60), 3, Qt::DashLine));
        painter.drawLine(pos, center);
    }
}
