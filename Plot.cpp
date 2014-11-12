#include "Plot.h"
#include <QtGui>

/**
 *	コンストラクタ
 */
Plot::Plot(QWidget *parent) :
	QwtPlot(parent)
{
	// Background
	setCanvasBackground(QColor(Qt::black));

	// Grid
	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
	grid->attach(this);
}
