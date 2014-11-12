#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QtGui>

using namespace std;

class SimulationThread	:	public QThread
{
public:
								SimulationThread();
	void						run();																				// スレッド処理の実行
	void						ReadParam(int thresh, double money, QDate startDate, QDate endDate);				// パラメータ読み込み
	void						Calcuration(int thresh, double money, QDate startDate, QDate endDate);				// 計算

	// アルゴリズムで使用するパラメータ
	int							mThresh;																			// 閾値
	int							mMoney;																				// 初期資金
	QDate						mStartDate;																			// 開始日
	QDate						mEndDate;																			// 終了日
};

#endif // SIMULATIONTHREAD_H
