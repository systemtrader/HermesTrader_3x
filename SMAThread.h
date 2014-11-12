#ifndef SMATHREAD_H
#define SMATHREAD_H

#include <QtGui>

using namespace std;

class SMAThread	:	public QThread
{
public:
								SMAThread();																		// コンストラクタ
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(int n);																	// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// サンプル数
};

#endif // SMATHREAD_H
