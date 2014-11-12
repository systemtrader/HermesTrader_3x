#ifndef MOVINGAVERAGETHREAD_H
#define MOVINGAVERAGETHREAD_H

#include <QtGui>

using namespace std;

class MovingAverageThread	:	public QThread
{
public:
								MovingAverageThread();																// コンストラクタ
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(QString alg, int n);													// 移動平均計算

	// アルゴリズムで使用するパラメータ
	QString						mAlg;																				// アルゴリズム
	int							mN;																					// サンプル数
};

#endif // MOVINGAVERAGETHREAD_H
