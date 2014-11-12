#ifndef FIBONACCITHREAD_H
#define FIBONACCITHREAD_H

#include <QtGui>

using namespace std;

class FibonacciThread	:	public QThread
{
public:
								FibonacciThread();
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(int n);																	// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// サンプル数
};

#endif // FIBONACCITHREAD_H
