#ifndef RMATHREAD_H
#define RMATHREAD_H

#include <QtGui>

using namespace std;

class RMAThread	:	public QThread
{
public:
								RMAThread();																		// コンストラクタ
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(int n);																	// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// サンプル数
};

#endif // RMATHREAD_H
