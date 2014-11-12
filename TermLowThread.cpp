#include "TermLowThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
TermLowThread::TermLowThread()
{
}

/**
 *	スレッド処理実行
 */
void TermLowThread::run()
{
	Calcuration(mN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 *
 *	@param[in] n			サンプル数
 */
void TermLowThread::ReadParam(int n)
{
	mN			= n;
}

/**
 *	計算およびDB構築
 *
 *	@param[in] n			サンプル数
 */
void TermLowThread::Calcuration(int n)
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <PliceData> pliceData;	// 時系列データ
	double tl_open;					// 期間安値(Open)
	double tl_high;					// 期間安値(High)
	double tl_low;					// 期間安値(Low)
	double tl_close;				// 期間安値(Close)
	double tl_volume;				// 期間安値(Volume)
	double tl_aClose;				// 期間安値(AClose)
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("DB\\TermLow_" + temp_1.setNum(n).toLocal8Bit());

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
			writeFileName = "DB\\TermLow_" + temp_1.setNum(n) + "\\" + temp_2.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = n; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// 期間安値計算
				tl_open		= 100000000;
				tl_high		= 100000000;
				tl_low		= 100000000;
				tl_close	= 100000000;
				tl_volume	= 100000000;
				tl_aClose	= 100000000;
				for (int k = 0; k < n; k++) {
					tl_open		= min(pliceData[j-k].mOpen, tl_open);
					tl_high		= min(pliceData[j-k].mHigh, tl_high);
					tl_low		= min(pliceData[j-k].mLow, tl_low);
					tl_close	= min(pliceData[j-k].mClose, tl_close);
					tl_volume	= min(pliceData[j-k].mVolume, tl_volume);
					tl_aClose	= min(pliceData[j-k].mAClose, tl_aClose);
				}

				// 期間安値の書き込み
				out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
					<< tl_open << ","
					<< tl_high << ","
					<< tl_low << ","
					<< tl_close << ","
					<< tl_volume << ","
					<< tl_aClose << "\n";
			}

			writeFile.close();
		}

		readFile.close();
	}
}
