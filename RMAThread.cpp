#include "RMAThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
RMAThread::RMAThread()
{
}

/**
 *	スレッド処理実行
 */
void RMAThread::run()
{
	Calcuration(mN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 *
 *	@param[in] n			サンプル数
 */
void RMAThread::ReadParam(int n)
{
	mN			= n;
}

/**
 *	計算およびDB構築
 *
 *	@param[in] n			サンプル数
 */
void RMAThread::Calcuration(int n)
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <PliceData> pliceData;	// 時系列データ
	double ma_open;					// MA(Open)
	double ma_high;					// MA(High)
	double ma_low;					// MA(Low)
	double ma_close;				// MA(Close)
	double ma_volume;				// MA(Volume)
	double ma_aClose;				// MA(AClose)
	double pMa_open;				// 前日のMA(Open)
	double pMa_high;				// 前日のMA(High)
	double pMa_low;					// 前日のMA(Low)
	double pMa_close;				// 前日のMA(Close)
	double pMa_volume;				// 前日のMA(Volume)
	double pMa_aClose;				// 前日のMA(AClose)
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("DB\\RMA_" + temp_1.setNum(n).toLocal8Bit());

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
			writeFileName = "DB\\RMA_" + temp_1.setNum(n) + "\\" + temp_2.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = n; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// RMA計算
				ma_open		= 0;
				ma_high		= 0;
				ma_low		= 0;
				ma_close	= 0;
				ma_volume	= 0;
				ma_aClose	= 0;
				if (j == n) {
					for (int k = 0; k < n; k++) {
						ma_open		+= pliceData[j-k].mOpen;
						ma_high		+= pliceData[j-k].mHigh;
						ma_low		+= pliceData[j-k].mLow;
						ma_close	+= pliceData[j-k].mClose;
						ma_volume	+= pliceData[j-k].mVolume;
						ma_aClose	+= pliceData[j-k].mAClose;
					}
					ma_open		= ma_open / n;
					ma_high		= ma_high / n;
					ma_low		= ma_low / n;
					ma_close	= ma_close / n;
					ma_volume	= ma_volume / n;
					ma_aClose	= ma_aClose / n;
				}
				else {
					ma_open		= ((n - 1) * pMa_open + pliceData[j].mOpen) / n;
					ma_high		= ((n - 1) * pMa_high + pliceData[j].mHigh) / n;
					ma_low		= ((n - 1) * pMa_low + pliceData[j].mLow) / n;
					ma_close	= ((n - 1) * pMa_close + pliceData[j].mClose) / n;
					ma_volume	= ((n - 1) * pMa_volume + pliceData[j].mVolume) / n;
					ma_aClose	= ((n - 1) * pMa_aClose + pliceData[j].mAClose) / n;
				}

				// RMAおよび乖離率の書き込み
				out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
					<< ma_open << ","
					<< ma_high << ","
					<< ma_low << ","
					<< ma_close << ","
					<< ma_volume << ","
					<< ma_aClose << ","
					<< (pliceData[j-1].mOpen - ma_open) / ma_open * 100 << ","
					<< (pliceData[j-1].mHigh - ma_high) / ma_high * 100 << ","
					<< (pliceData[j-1].mLow - ma_low) / ma_low * 100 << ","
					<< (pliceData[j-1].mClose - ma_close) / ma_close * 100 << ","
					<< (pliceData[j-1].mVolume - ma_volume) / ma_volume * 100 << ","
					<< (pliceData[j-1].mAClose - ma_aClose) / ma_aClose * 100 << "\n";
				pMa_open = ma_open;
				pMa_high = ma_high;
				pMa_low = ma_low;
				pMa_close = ma_close;
				pMa_volume = ma_volume;
				pMa_aClose = ma_aClose;
			}

			writeFile.close();
		}

		readFile.close();
	}
}
