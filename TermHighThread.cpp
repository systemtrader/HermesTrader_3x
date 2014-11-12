#include "TermHighThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
TermHighThread::TermHighThread()
{
}

/**
 *	スレッド処理実行
 */
void TermHighThread::run()
{
	Calcuration(mN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 *
 *	@param[in] n			サンプル数
 */
void TermHighThread::ReadParam(int n)
{
	mN			= n;
}

/**
 *	計算およびDB構築
 *
 *	@param[in] n			サンプル数
 */
void TermHighThread::Calcuration(int n)
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <PliceData> pliceData;	// 時系列データ
	double th_open;					// 期間高値(Open)
	double th_high;					// 期間高値(High)
	double th_low;					// 期間高値(Low)
	double th_close;				// 期間高値(Close)
	double th_volume;				// 期間高値(Volume)
	double th_aClose;				// 期間高値(AClose)
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("DB\\TermHigh_" + temp_1.setNum(n).toLocal8Bit());

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
			writeFileName = "DB\\TermHigh_" + temp_1.setNum(n) + "\\" + temp_2.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = n; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// 期間高値計算
				th_open		= 0;
				th_high		= 0;
				th_low		= 0;
				th_close	= 0;
				th_volume	= 0;
				th_aClose	= 0;
				for (int k = 0; k < n; k++) {
					th_open		= max(pliceData[j-k].mOpen, th_open);
					th_high		= max(pliceData[j-k].mHigh, th_high);
					th_low		= max(pliceData[j-k].mLow, th_low);
					th_close	= max(pliceData[j-k].mClose, th_close);
					th_volume	= max(pliceData[j-k].mVolume, th_volume);
					th_aClose	= max(pliceData[j-k].mAClose, th_aClose);
				}

				// 期間高値の書き込み
				out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
					<< th_open << ","
					<< th_high << ","
					<< th_low << ","
					<< th_close << ","
					<< th_volume << ","
					<< th_aClose << "\n";
			}

			writeFile.close();
		}

		readFile.close();
	}
}
