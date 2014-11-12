#ifndef ICHIMOKUTHREAD_H
#define ICHIMOKUTHREAD_H

#include <QtGui>

using namespace std;

class IchimokuThread	:	public QThread
{
public:
								IchimokuThread();
	void						run();																				// スレッド処理の実行
	void						ReadParam();																		// パラメータ読み込み
	void						Calcuration();																		// 計算
};

#endif // ICHIMOKUTHREAD_H
