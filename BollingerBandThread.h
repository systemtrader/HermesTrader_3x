#ifndef BOLLINGERBANDTHREAD_H
#define BOLLINGERBANDTHREAD_H

#include <QtGui>

using namespace std;

class BollingerBandThread	:	public QThread
{
public:
								BollingerBandThread();
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(int n);																	// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// サンプル数
};

#endif // BOLLINGERBANDTHREAD_H
