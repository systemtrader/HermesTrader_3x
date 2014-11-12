#include "ChangeThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
ChangeThread::ChangeThread()
{
}

/**
 *	スレッド処理実行
 */
void ChangeThread::run()
{
	Calcuration();
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 */
void ChangeThread::ReadParam()
{
}

/**
 *	計算およびDB構築
 */
void ChangeThread::Calcuration()
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <PliceData> pliceData;	// 時系列データ
	double change_open;				// 前日比(Open)
	double change_high;				// 前日比(High)
	double change_low;				// 前日比(Low)
	double change_close;			// 前日比(Close)
	double change_volume;			// 前日比(Volume)
	double change_aClose;			// 前日比(AClose)
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("DB\\Change");

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
			writeFileName = "DB\\Change\\" + temp_1.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = 1; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// 前日比計算
				change_open		= (pliceData[j].mOpen - pliceData[j-1].mOpen) / pliceData[j-1].mOpen;
				change_high		= (pliceData[j].mHigh - pliceData[j-1].mHigh) / pliceData[j-1].mHigh;
				change_low		= (pliceData[j].mLow - pliceData[j-1].mLow) / pliceData[j-1].mLow;
				change_close	= (pliceData[j].mClose - pliceData[j-1].mClose) / pliceData[j-1].mClose;
				change_volume	= (pliceData[j].mVolume - pliceData[j-1].mVolume) / pliceData[j-1].mVolume;
				change_aClose	= (pliceData[j].mAClose - pliceData[j-1].mAClose) / pliceData[j-1].mAClose;

				// 前日比の書き込み
				out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
					<< change_open << ","
					<< change_high << ","
					<< change_low << ","
					<< change_close << ","
					<< change_volume << ","
					<< change_aClose << "\n";
			}

			writeFile.close();
		}

		readFile.close();
	}
}
