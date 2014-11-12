#include "FibonacciThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
FibonacciThread::FibonacciThread()
{
}

/**
 *	スレッド処理実行
 */
void FibonacciThread::run()
{
	Calcuration(mN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 */
void FibonacciThread::ReadParam(int n)
{
	mN			= n;
}

/**
 *	計算およびDB構築
 */
void FibonacciThread::Calcuration(int n)
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <PliceData> pliceData;	// 時系列データ
	double fib_recent_high;			// 直近高値
	double fib_recent_low;			// 直近安値
	QDate fib_recent_high_date;		// 直近高値日
	QDate fib_recent_low_date;		// 直近安値日
	int fib_current_condition;		// 現在値の状態(現在終値が直近高値の場合は0,直近安値の場合は1。直近高値が直近安値よりも先なら2,そうでなければ3。直近高値日と直近安値日が同一日なら4)
	double fib_push_382;			// 押し(38.2%)
	double fib_push_500;			// 押し(50.0%)
	double fib_push_618;			// 押し(61.8%)
	double fib_pull_382;			// 戻し(38.2%)
	double fib_pull_500;			// 戻し(50.0%)
	double fib_pull_618;			// 戻し(61.8%)
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列
	double maxRec, minRec;

	// ディレクトリ生成
	_mkdir("DB\\Fibonacci_" + temp_1.setNum(n).toLocal8Bit());

	// 各IDについて処理実行
	for (int i = 1000; i < 1000000; i++) {
		if (((i > 10000) && !((i == 998405) || (i == 998407)))) {
			continue;
		}

		// 変数初期化
		pliceData.clear();

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
				if (readFileLine != "") {
					tempData.mDate		= QDate::fromString((readFileLine.split(",").at(0)), "yyyy/M/d");
					tempData.mOpen		= readFileLine.split(",").at(1).toDouble();
					tempData.mHigh		= readFileLine.split(",").at(2).toDouble();
					tempData.mLow		= readFileLine.split(",").at(3).toDouble();
					tempData.mClose		= readFileLine.split(",").at(4).toDouble();
					tempData.mVolume	= readFileLine.split(",").at(5).toDouble();
					tempData.mAClose	= readFileLine.split(",").at(6).toDouble();
					pliceData.push_back(tempData);
				}
			}

			// 書き込み用ファイルオープン
			writeFileName = "DB\\Fibonacci_" + temp_1.setNum(n) + "\\" + temp_2.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = n; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// サンプルn日間の直近高値と直近安値を取得
				maxRec = 0;
				minRec = 10000000;
				for (int k = 0; k < n; k++) {
					maxRec = max(maxRec, pliceData[j-k].mHigh);
					minRec = min(minRec, pliceData[j-k].mLow);
				}
				fib_recent_high = maxRec;
				fib_recent_low = minRec;

				// サンプルn日間の直近高値日と直近安値日を取得
				for (int k = 0; k < n; k++) {
					if (pliceData[j-k].mHigh == fib_recent_high) {
						fib_recent_high_date = pliceData[j-k].mDate;
					}
					if (pliceData[j-k].mLow == fib_recent_low) {
						fib_recent_low_date = pliceData[j-k].mDate;
					}
				}

				// 現在価格の状態値取得
				// 現在終値が直近高値の場合
				if (pliceData[j].mClose == fib_recent_high) {
					fib_current_condition = 0;
				}
				// 現在終値が直近安値の場合
				else if (pliceData[j].mClose == fib_recent_low) {
					fib_current_condition = 1;
				}
				// 直近高値日が直近安値日よりも最近の場合
				else if (fib_recent_high_date.toJulianDay() > fib_recent_low_date.toJulianDay()) {
					fib_current_condition = 2;
				}
				// 直近安値日が直近高値日よりも最近の場合
				else if (fib_recent_high_date.toJulianDay() < fib_recent_low_date.toJulianDay()) {
					fib_current_condition = 3;
				}
				// 直近高値日が直近安値日と同一の場合
				else if (fib_recent_high_date == fib_recent_low_date) {
					fib_current_condition = 4;
				}
				// それ以外の場合（想定していない）
				else {
					fib_current_condition = 5;
				}

				// 押しと戻しの計算
				fib_push_382 = fib_recent_high - ((fib_recent_high - fib_recent_low) * 0.382);
				fib_push_500 = fib_recent_high - ((fib_recent_high - fib_recent_low) * 0.500);
				fib_push_618 = fib_recent_high - ((fib_recent_high - fib_recent_low) * 0.618);
				fib_pull_382 = fib_recent_low + ((fib_recent_high - fib_recent_low) * 0.382);
				fib_pull_500 = fib_recent_low + ((fib_recent_high - fib_recent_low) * 0.500);
				fib_pull_618 = fib_recent_low + ((fib_recent_high - fib_recent_low) * 0.618);

				// フィボナッチトレースメントの書き込み
				out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
					<< fib_current_condition << ","												// 現在状態
					<< fib_push_382 << ","														// 押し(38.2%)
					<< fib_push_500 << ","														// 押し(50.0%)
					<< fib_push_618 << ","														// 押し(61.8%)
					<< fib_pull_382 << ","														// 戻し(38.2%)
					<< fib_pull_500 << ","														// 戻し(50.0%)
					<< fib_pull_618 << "\n";													// 戻し(61.8%)
			}

			writeFile.close();
		}

		readFile.close();
	}
}
