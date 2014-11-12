#include "BollingerBandThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
BollingerBandThread::BollingerBandThread()
{
}

/**
 *	スレッド処理実行
 */
void BollingerBandThread::run()
{
	Calcuration(mN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 */
void BollingerBandThread::ReadParam(int n)
{
	mN			= n;
}

/**
 *	計算およびDB構築
 */
void BollingerBandThread::Calcuration(int n)
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
	double sta_close;				// 標準偏差(Close)
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("DB\\BollingerBand_" + temp_1.setNum(n).toLocal8Bit());

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
			writeFileName = "DB\\BollingerBand_" + temp_1.setNum(n) + "\\" + temp_2.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = n; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// SMA計算
				ma_open		= 0;
				ma_high		= 0;
				ma_low		= 0;
				ma_close	= 0;
				ma_volume	= 0;
				ma_aClose	= 0;
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

				// 標準偏差計算
				for (int k = 0; k < n; k++) {
					sta_close += (pliceData[j-k].mClose - ma_close) * (pliceData[j-k].mClose - ma_close);
				}
				sta_close = sqrt(sta_close) / (n * (n - 1));

				// ボリンジャーバンドの書き込み
				out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
					<< ma_close << ","															// SMA
					<< ma_close + (3 * sta_close) << ","										// +3σ
					<< ma_close + (2 * sta_close) << ","										// +2σ
					<< ma_close + (1 * sta_close) << ","										// +σ
					<< ma_close + (-1 * sta_close) << ","										// -σ
					<< ma_close + (-2 * sta_close) << ","										// -2σ
					<< ma_close + (-3 * sta_close) << "\n";										// -3σ
			}

			writeFile.close();
		}

		readFile.close();
	}
}
