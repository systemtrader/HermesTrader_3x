#include "WMAThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
WMAThread::WMAThread()
{
}

/**
 *	スレッド処理実行
 */
void WMAThread::run()
{
	Calcuration(mN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 *
 *	@param[in] n			サンプル数
 */
void WMAThread::ReadParam(int n)
{
	mN			= n;
}

/**
 *	計算およびDB構築
 *
 *	@param[in] n			サンプル数
 */
void WMAThread::Calcuration(int n)
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
	double dMa_open;				// MA計算時の分母(Open)
	double dMa_high;				// MA計算時の分母(High)
	double dMa_low;					// MA計算時の分母(Low)
	double dMa_close;				// MA計算時の分母(Close)
	double dMa_volume;				// MA計算時の分母(Volume)
	double dMa_aClose;				// MA計算時の分母(AClose)
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("DB\\WMA_" + temp_1.setNum(n).toLocal8Bit());

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
			writeFileName = "DB\\WMA_" + temp_1.setNum(n) + "\\" + temp_2.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = n; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// WMA計算
				ma_open		= 0;
				ma_high		= 0;
				ma_low		= 0;
				ma_close	= 0;
				ma_volume	= 0;
				ma_aClose	= 0;
				dMa_open	= 0;
				dMa_high	= 0;
				dMa_low		= 0;
				dMa_close	= 0;
				dMa_volume	= 0;
				dMa_aClose	= 0;
				for (int k = 0; k < n; k++) {
					ma_open		+= ((n - k) * pliceData[j-k].mOpen);
					ma_high		+= ((n - k) * pliceData[j-k].mHigh);
					ma_low		+= ((n - k) * pliceData[j-k].mLow);
					ma_close	+= ((n - k) * pliceData[j-k].mClose);
					ma_volume	+= ((n - k) * pliceData[j-k].mVolume);
					ma_aClose	+= ((n - k) * pliceData[j-k].mAClose);
					dMa_open	+= n;
					dMa_high	+= n;
					dMa_low		+= n;
					dMa_close	+= n;
					dMa_volume	+= n;
					dMa_aClose	+= n;
				}
				ma_open		= ma_open / dMa_open;
				ma_high		= ma_high / dMa_high;
				ma_low		= ma_low / dMa_low;
				ma_close	= ma_close / dMa_close;
				ma_volume	= ma_volume / dMa_volume;
				ma_aClose	= ma_aClose / dMa_aClose;

				// WMAおよび乖離率の書き込み
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
			}

			writeFile.close();
		}

		readFile.close();
	}
}
