#include "IchimokuThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
IchimokuThread::IchimokuThread()
{
}

/**
 *	スレッド処理実行
 */
void IchimokuThread::run()
{
	Calcuration();
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 */
void IchimokuThread::ReadParam()
{
}

/**
 *	計算およびDB構築
 */
void IchimokuThread::Calcuration()
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <PliceData> pliceData;	// 時系列データ
	double ichimoku_base;			// 基準線
	double ichimoku_conversion;		// 転換線
	double ichimoku_early_1[27];	// 先行スパン1
	double ichimoku_early_2[27];	// 先行スパン2
	double ichimoku_later;			// 遅行スパン
	PliceData tempData;				// 一時保存用時系列データ
	QString temp_1;					// 一時保存用文字列
	double max26, min26, max9, min9, max52, min52;

	// ディレクトリ生成
	_mkdir("DB\\Ichimoku");

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
			writeFileName = "DB\\Ichimoku\\" + temp_1.setNum(i) + ".dat";
			writeFile.setFileName(writeFileName);
			if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int j = 1; j < (int)pliceData.size(); j++) {
				QTextStream out(&writeFile);

				// 基準線計算(当日を含め、過去26日間の高値と安値の中値)
				if (j > 26) {
					max26 = 0;
					min26 = 10000000;
					for (int k = 0; k < 26; k++) {
						max26 = max(max26, pliceData[j-k].mHigh);		// 過去26日間の高値
						min26 = min(min26, pliceData[j-k].mLow);		// 過去26日間の安値
					}
					ichimoku_base = (max26 + min26) / 2;
				}
				else {
					ichimoku_base = 0;
				}

				// 転換線計算(その日を含め、過去9日間の高値と安値の中値)
				if (j > 9) {
					max9 = 0;
					min9 = 10000000;
					for (int k = 0; k < 9; k++) {
						max9 = max(max9, pliceData[j-k].mHigh);			// 過去9日間の高値
						min9 = min(min9, pliceData[j-k].mLow);			// 過去9日間の安値
					}
					ichimoku_conversion = (max9 + min9) / 2;
				}
				else {
					ichimoku_conversion = 0;
				}

				// 先行スパン1計算(基準線と転換線の中値を26日先行した値)
				ichimoku_early_1[0] = (ichimoku_base + ichimoku_conversion) / 2;
				for (int k = 0; k < 26; k++) {
					ichimoku_early_1[k+1] = ichimoku_early_1[k];
				}

				// 先行スパン2計算(当日を含め過去52日間の高値と安値の中値を26日先行した値)
				if (j > 52) {
					max52 = 0;
					min52 = 10000000;
					for (int k = 0; k < 52; k++) {
						max52 = max(max52, pliceData[j-k].mHigh);		// 過去52日間の高値
						min52 = min(min52, pliceData[j-k].mLow);		// 過去52日間の安値
					}
					ichimoku_early_2[0] = (max52 + min52) / 2;
				}
				else {
					ichimoku_early_2[0] = 0;
				}
				for (int k = 0; k < 26; k++) {
					ichimoku_early_2[k+1] = ichimoku_early_2[k];
				}


				// 遅行スパン計算(当日の終値を26日遡った値)
				if (j < (int)pliceData.size() - 26) {
					ichimoku_later = pliceData[j+26].mClose;
				}
				else {
					ichimoku_later = 0;
				}

				// 一目均衡表の書き込み
				out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
					<< ichimoku_base << ","
					<< ichimoku_conversion << ","
					<< ichimoku_early_1[26] << ","
					<< ichimoku_early_2[26] << ","
					<< ichimoku_later << "\n";
			}

			writeFile.close();
		}

		readFile.close();
	}
}
