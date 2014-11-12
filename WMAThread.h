#ifndef WMATHREAD_H
#define WMATHREAD_H

#include <QtGui>

using namespace std;

class WMAThread	:	public QThread
{
public:
								WMAThread();																		// コンストラクタ
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(int n);																	// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// サンプル数
};

#endif // WMATHREAD_H
