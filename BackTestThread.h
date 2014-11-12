#ifndef BACKTESTTHREAD_H
#define BACKTESTTHREAD_H

#include <QtGui>

using namespace std;

// BackTest1クラス
class BackTest {
public:
	QDate						mDate;																				// 日付
	double						mEMoney;																			// 評価額
	double						mMoney;																				// 資金
	bool						mBuySignal;																			// 買いシグナル
	bool						mSellSignal;																		// 売りシグナル
	int							mBuyNum;																			// 買い株数
	int							mSellNum;																			// 売り株数
	double						mBuyPlice;																			// 買い価格
	double						mSellPlice;																			// 売り価格
	double						mOpen;																				// 始値
	double						mHigh;																				// 高値
	double						mLow;																				// 安値
	double						mClose;																				// 終値
	double						mVolume;																			// 出来高
	double						mAClose;																			// 調整後終値
	double						mSMAR_close;																		// SMA(終値)乖離率
	double						mTHigh;																				// 期間高値
	double						mTLow;																				// 期間安値
};

class BackTestThread	:	public QThread
{
public:
								BackTestThread();
	void						run();																				// スレッド処理の実行
	void						ReadParam(int thresh, double money, int maN, int tN);								// パラメータ読み込み
	void						Calcuration(int thresh, double money, int maN, int tN);								// 計算

	// アルゴリズムで使用するパラメータ
	int							mThresh;																			// 閾値
	int							mMoney;																				// 初期資金
	int							mMaN;																				// 移動平均のサンプル数
	int							mTN;																				// 期間高値、期間安値の期間
};

#endif // BACKTESTTHREAD_H
