#ifndef EMATHREAD_H
#define EMATHREAD_H

#include <QtGui>

using namespace std;

class EMAThread	:	public QThread
{
public:
								EMAThread();																		// コンストラクタ
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(int n);																	// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// サンプル数
};

#endif // EMATHREAD_H
