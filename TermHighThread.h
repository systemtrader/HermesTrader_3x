#ifndef TERMHIGHTHREAD_H
#define TERMHIGHTHREAD_H

#include <QtGui>

using namespace std;

class TermHighThread	:	public QThread
{
public:
								TermHighThread();
	void						run();																				// スレッド処理の実行
	void						ReadParam(int n);																	// パラメータ読み込み
	void						Calcuration(int n);																	// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// 期間
};

#endif // TERMHIGHTHREAD_H
