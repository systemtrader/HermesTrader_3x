#include <QtGui/QApplication>
#include "WndMain.h"
#include "Plot.h"
#include "Predict.h"
#include "Target.h"
#include "Template.h"

// プロトタイプ宣言
void DeleteAll();

// 各種クラスのグローバルインスタンスポインタ
WndMain*	gpWndMain	= NULL;			// メインフォーム
Plot*		gpPlot		= NULL;			// プロットクラス
Predict*	gpPredict	= NULL;			// 予測計算処理クラス
Target*		gpTarget	= NULL;			// ターゲットデータ処理クラス
Template*	gpTemplate	= NULL;			// テンプレートデータ処理クラス

/**
 *  メインルーチン
 *
 *  @param[in] argc コマンドライン引数の数
 *  @param[in] argv コマンドライン引数配列へのポインタ
 *
 *  @retval int 終了コード
 */
int main(int argc, char *argv[])
{
	// アプリケーションクラスインスタンス生成
	QApplication* pApp = new QApplication(argc, argv);

	// 各種クラスのグローバルインスタンスの生成
	gpWndMain	= new WndMain();
	gpTarget	= new Target();
	gpTemplate	= new Template();
	gpPredict	= new Predict();

	// 表示
	gpWndMain->show();

	// アプリケーション実行
	int iResult = pApp->exec();							// 実行

	// 終了処理
	DeleteAll();										// グローバルインスタンス破棄
	delete pApp;
	return iResult;										// 終了
}

/**
 *  各種クラスのグローバルインスタンスの破棄
 */
void DeleteAll()
{
	delete gpWndMain;
	delete gpPlot;
	delete gpPredict;
	delete gpTarget;
	delete gpTemplate;
}
