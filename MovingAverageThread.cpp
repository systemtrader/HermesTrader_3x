#include "MovingAverageThread.h"
#include <direct.h>

MovingAverageThread::MovingAverageThread()
{
}

/**
 *	スレッド処理実行
 */
void MovingAverageThread::run()
{
	Calcuration(mN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 *
 *	@param[in] n			サンプル数
 */
void MovingAverageThread::ReadParam(int n)
{
	mN			= n;
}

/**
 *	単純移動平均(SMA)計算
 *
 *	@param[in] alg			アルゴリズム(SMA, EMA, RMA, WMA)
 *	@param[in] n			サンプル数
 */
void MovingAverageThread::Calcuration(QString alg, int n)
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <QDate> date;			// 日付データ
	vector <double> close;			// 終値データ
	double ma;						// MA
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列

	// 変数初期化
	date.clear();
	close.clear();

	if (alg == "SMA") {
		// SMAディレクトリ生成
		_mkdir("DB\\SMA_" + temp_1.setNum(n).toLocal8Bit());
	}

	// 各IDについて処理実行
	for (int i = 1000; i < 1000000; i++) {
		if (((i > 10000) && !((i == 998405) || (i == 998407)))) {
			continue;
		}

		// 読み込み用ファイルオープン
		readFileName = "Yahoo\\" + temp_1.setNum(i) + ".csv";
		readFile.setFileName(readFileName);
		if (readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			QTextStream readFileReader(&readFile);
			readFileLine = "";

			// 時系列データの読み取り
			while (!readFileReader.atEnd()) {
				readFileLine = readFileReader.readLine();
				// データ取得
				date.push_back(QDate::fromString((readFileLine.split(",").at(0)), "yyyy/M/d"));
				close.push_back(readFileLine.split(",").at(6).toDouble());
			}

			if (alg == "SMA") {
				writeFileName = "DB\\SMA_" + temp_1.setNum(n) + "\\" + temp_2.setNum(i) + ".dat";
			}

			// 書き込み用ファイルオープン
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int i = n; i < (int)close.size() - n; i++) {
				QTextStream out(&writeFile);

				if (alg == "SMA") {
					// SMA計算
					ma = 0;
					for (int j = 0; j < n; j++) {
						ma += close[i-j];
					}
					ma = ma / n;
				}

				// SMAの書き込み
				out << (date[i].toString("yyyy/M/d")) << "," << ma << "\n";
			}

			writeFile.close();
		}

		readFile.close();
	}
}
