#ifndef WNDMAIN_H
#define WNDMAIN_H

#include <QMainWindow>
#include <QDate>
#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"
#include "qwt/qwt_scale_draw.h"
#include <QtGui>
#include "SMAThread.h"
#include "EMAThread.h"
#include "RMAThread.h"
#include "WMAThread.h"
#include "ChangeThread.h"
#include "TermHighThread.h"
#include "TermLowThread.h"
#include "IchimokuThread.h"
#include "BollingerBandThread.h"
#include "FibonacciThread.h"
#include "BackTestThread.h"
#include "SimulationThread.h"

#define			PLOT_DATA_SIZE		20

using namespace std;

namespace Ui {
	class						WndMain;
}

// IndexDataクラス
class IndexData {
public:
	QString						mID;
	QString						mMarket;
	QString						mName;
	QString						mKind;
	double						mOpen;
	double						mHigh;
	double						mLow;
	double						mClose;
	double						mVolume;
	double						mAClose;
	double						mRate;
};

// PliceDataクラス
class PliceData {
public:
	QDate						mDate;
	double						mDateJulian;
	double						mOpen;
	double						mHigh;
	double						mLow;
	double						mClose;
	double						mVolume;
	double						mAClose;
};

// FilteredScoreDataクラス
class ScoreData {
public:
	int							mTargetID;
	int							mTemplateID;
	QDate						mDate;
	double						mScore;
};

// DateScaleDrawクラス
class DateScaleDraw: public QwtScaleDraw
{
public:
								DateScaleDraw(const QDate &base):
								baseTime(base)
								{
								}

	virtual QwtText				label(double v) const
								{
									return baseTime.fromJulianDay((int)v).toString("yyyy/MM/dd");
								}
private:
	QDate						baseTime;
};

class WndMain : public QMainWindow
{
	Q_OBJECT

public:
	explicit					WndMain(QWidget *parent = 0);
								~WndMain();
	QDate						upTime() const;																		// グラフ描画用日付データ変換関数
	void						AddPliceData(QTableWidget *pTableWidget, QString date, QString open, QString high,
											 QString low, QString close, QString volume, QString aClose);			// Targetリスト/Templateリストにデータを追加する関数
	SMAThread*					mpSMAThread;																		// SMAスレッド
	EMAThread*					mpEMAThread;																		// EMAスレッド
	RMAThread*					mpRMAThread;																		// RMAスレッド
	WMAThread*					mpWMAThread;																		// WMAスレッド
	ChangeThread*				mpChangeThread;																		// 前日比スレッド
	TermHighThread*				mpTermHighThread;																	// 期間高値スレッド
	TermLowThread*				mpTermLowThread;																	// 期間安値スレッド
	IchimokuThread*				mpIchimokuThread;																	// 一目均衡表スレッド
	BollingerBandThread*		mpBollingerBandThread;																// ボリンジャーバンドスレッド
	FibonacciThread*			mpFibonacciThread;																	// フィボナッチスレッド
	BackTestThread*				mpBackTestThread;																	// バックテストスレッド
	SimulationThread*			mpSimulationThread;																	// シミュレーションスレッド

protected:
	void						changeEvent(QEvent *e);
	void						dragEnterEvent(QDragEnterEvent *);
	void						dropEvent(QDropEvent *);
	void						AddIndexData(QTableWidget *pTableWidget, QString id, QString market,
											 QString name, QString kind, QString open, QString high, QString low,
											 QString close, QString volume, QString aClose, QString rate);			// 銘柄リストにデータを追加する関数
	void						AddScoreData(QTableWidget *pTableWidget, QString targetID,
											 QString templateID, QString date, QString score);						// Scoreリストにデータを追加する関数
	void						AddPredictionData(QTableWidget *pTableWidget, QString day, QString bestCaseClose,
												  QString worstCaseClose, QString centerCaseClose);					// Predictionリストにデータを追加する関数
	void						ReadIndexCSV(QString filePath);														// IndexCSVファイルを読み込んでデータ配列にデータを格納する関数
	void						ReadTargetCSV(QString filePath, int fragmentLength);								// TargetCSVファイルを読み込んでデータ配列にデータを格納する関数
	void						ReadTemplateCSV(QString filePath, int pltLength, QDate date);						// TemplateCSVファイルを読み込んでデータ配列にデータを格納する関数
	void						ReadScore(QString filePath);														// FilteredScoreファイルを読み込んでデータ配列にデータを格納する関数
	void						PredictionTableMapping();															// Prediction後のテーブル反映関数
	void						DrawTargetCSVPlot(int fragmentLength, int start);									// Targetデータグラフ描画関数
	void						DrawTemplateCSVPlot(int pltLength, QDate date);										// Templateデータグラフ描画関数
	void						DrawBestCasePlot();																	// BestCaseデータグラフ描画関数
	void						DrawWorstCasePlot();																// WorstCaseデータグラフ描画関数
	void						DrawCenterCasePlot();																// CenterCaseデータグラフ描画関数

private slots:
	void						on_mnuOpen_triggered();																// [Open]ボタンのclicked()シグナルを受け取るスロット
	void						on_mnuTemplateRestruct_triggered();													// [FA]->[Template]->[Restruct]メニューのtriggered()シグナルを受け取るスロット
	void						on_mnuTargetRestruct_triggered();													// [FA]->[Target]->[Restruct]メニューのtriggered()シグナルを受け取るスロット
	void						on_mnuCalculateScore_triggered();													// [FA]->[Predict]->[CalculateScore]メニューのtriggered()シグナルを受け取るスロット
	void						on_btnCalculation_clicked();														// [ScoreCalculation]ボタンのclicked()シグナルを受け取るスロット
	void						on_btnDrawPlot_clicked();															// [DrawPlot]ボタンのclicked()シグナルを受け取るスロット
	void						on_btnPrediction_clicked();															// [Prediction]ボタンのclicked()シグナルを受け取るスロット
	void						on_tblIndexData_itemDoubleClicked(QTableWidgetItem *item);							// 銘柄リスト上でダブルクリックするとチャートダイアログ表示

	void on_btnDB_clicked();
	void on_btnBackTest_clicked();
	void on_btnSimulation_clicked();

private:
	Ui::WndMain					*ui;
	QFileInfo					msOpenFile;																			// 読み込みファイル
	QString						mRootPath;																			// ルートパス
	QString						mScoreDirName;																		// スコアディレクトリ名
	QwtPlotCurve*				mTargetPlotCurve;																	// Targetグラフ描画ポインタ
	QwtPlotCurve*				mTemplatePlotCurve;																	// Templateグラフ描画ポインタ
	QwtPlotCurve*				mBestCasePlotCurve;																	// BestCaseグラフ描画ポインタ
	QwtPlotCurve*				mWorstCasePlotCurve;																// WorstCaseグラフ描画ポインタ
	QwtPlotCurve*				mCenterCasePlotCurve;																// CenterCaseグラフ描画ポインタ
	vector <IndexData>			mIndexData;																			// Indexデータ配列
	vector <PliceData>			mTargetPliceData;																	// Target価格データ配列
	vector <PliceData>			mTemplatePliceData;																	// Template価格データ配列
	vector <ScoreData>			mScoreData;																			// FilteredScore配列
	QString						mTemp1;
};

extern WndMain*					gpWndMain;																			// WndMainのグローバルポインタ

#endif // WNDMAIN_H
