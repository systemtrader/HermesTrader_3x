#ifndef DLGCHART_H
#define DLGCHART_H

#include <QDialog>
#include "WndMain.h"

namespace Ui {
    class DlgChart;
}

class DlgChart : public QDialog
{
	Q_OBJECT
public:
	explicit					DlgChart(QWidget *parent = 0);														// コンストラクタ
								~DlgChart();																		// デストラクタ
	void						ReadPliceCSV(QString filePath);														// 時系列CSVファイルを読み込んでデータ配列にデータを格納する関数
	void						DrawPliceCSVPlot();																	// 時系列データグラフ描画関数

protected:
	void						changeEvent(QEvent *e);																// ChangeEvent

private:
	Ui::DlgChart				*ui;
	vector <PliceData>			mPliceData;																			// 時系列データ配列
	QwtPlotCurve*				mPlicePlotCurve;																	// 時系列グラフ描画ポインタ
};

#endif // DLGCHART_H
