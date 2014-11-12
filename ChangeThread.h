#ifndef CHANGETHREAD_H
#define CHANGETHREAD_H

#include <QtGui>

using namespace std;

class ChangeThread	:	public QThread
{
public:
								ChangeThread();
	void						run();																				// スレッド処理の実行
	void						ReadParam();																		// パラメータ読み込み
	void						Calcuration();																		// 計算

	// アルゴリズムで使用するパラメータ
	int							mN;																					// サンプル数
};

#endif // CHANGETHREAD_H
