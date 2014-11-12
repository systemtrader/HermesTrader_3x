#include "BackTestThread.h"
#include <direct.h>
#include "WndMain.h"

/**
 *	コンストラクタ
 */
BackTestThread::BackTestThread()
{
}

/**
 *	スレッド処理実行
 */
void BackTestThread::run()
{
	Calcuration(mThresh, mMoney, mMaN, mTN);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 */
void BackTestThread::ReadParam(int thresh, double money, int maN, int tN)
{
	mThresh			= thresh;
	mMoney			= money;
	mMaN			= maN;
	mTN				= tN;
}

/**
 *	計算およびDB構築
 *	エントリー：単純移動平均(終値)乖離率が閾値を超えた翌日の寄付
 *	エグジット：単純移動平均(終値)乖離率がエントリーの逆側に0を超えた翌日の寄付
 *
 *	@param[in] thresh		閾値
 *	@param[in] money		初期資金
 *	@param[in] maN			移動平均のサンプル数
 *	@param[in] tN			期間高値、期間安値の期間
 */
void BackTestThread::Calcuration(int thresh, double money, int maN, int tN)
{
	// 変数宣言
	QString pliceFileName;			// 時系列ファイル名
	QFile pliceFile;				// 時系列ファイル
	QString pliceFileLine;			// 時系列ファイル行
	QString smaFileName;			// 単純移動平均ファイル名
	QFile smaFile;					// 単純移動平均ファイル
	QString smaFileLine;			// 単純移動平均ファイル行
	QString thFileName;				// 期間高値ファイル名
	QFile thFile;					// 期間高値ファイル
	QString thFileLine;				// 期間高値ファイル行
	QString tlFileName;				// 期間安値ファイル名
	QFile tlFile;					// 期間安値ファイル
	QString tlFileLine;				// 期間安値ファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <BackTest> testData;		// バックテストデータ
	BackTest tempData;				// 一時保存用バックテストデータ
	QString temp_1;					// 一時保存用文字列
	QString temp_2;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("BackTest");

	// 各IDについて処理実行
	for (int i = 1000; i < 1000000; i++) {
		if (((i > 10000) && !((i == 998405) || (i == 998407)))) {
			continue;
		}

		// 変数初期化
		testData.clear();

		// 時系列ファイルオープン
		pliceFileName = "Yahoo\\" + temp_1.setNum(i) + ".csv";
		pliceFile.setFileName(pliceFileName);

		// 時系列ファイルがない場合は次のIDへ
		if (!pliceFile.exists()) {
			continue;
		}

		if (pliceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			QTextStream pliceFileReader(&pliceFile);
			pliceFileLine = "";

			// 時系列データの読み取り
			while (!pliceFileReader.atEnd()) {
				pliceFileLine = pliceFileReader.readLine();
				// 時系列データ取得
				if (pliceFileLine != "") {
					tempData.mDate			= QDate::fromString((pliceFileLine.split(",").at(0)), "yyyy/M/d");
					tempData.mEMoney		= money;
					tempData.mMoney			= money;
					tempData.mBuySignal		= false;
					tempData.mSellSignal	= false;
					tempData.mBuyNum		= 0;
					tempData.mSellNum		= 0;
					tempData.mBuyPlice		= 0;
					tempData.mSellPlice		= 0;
					tempData.mOpen			= pliceFileLine.split(",").at(1).toDouble();
					tempData.mHigh			= pliceFileLine.split(",").at(2).toDouble();
					tempData.mLow			= pliceFileLine.split(",").at(3).toDouble();
					tempData.mClose			= pliceFileLine.split(",").at(4).toDouble();
					tempData.mVolume		= pliceFileLine.split(",").at(5).toDouble();
					tempData.mAClose		= pliceFileLine.split(",").at(6).toDouble();
					tempData.mSMAR_close	= 0;
					tempData.mTHigh			= 0;
					tempData.mTLow			= 0;
					testData.push_back(tempData);
				}
			}
		}
		pliceFile.close();

		// testDataのデータ量が使用する統計データのサンプル数の2倍に満たない場合は次のIDへ
		if (((int)testData.size() < mMaN * 2) || ((int)testData.size() < mTN * 2)) {
			continue;
		}

		// SMAファイルオープン
		smaFileName = "DB\\SMA_" + temp_1.setNum(maN) + "\\" + temp_2.setNum(i) + ".dat";
		smaFile.setFileName(smaFileName);
		if (smaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			int j = maN;
			QTextStream smaFileReader(&smaFile);
			smaFileLine = "";

			// SMAデータの読み取り
			while (!smaFileReader.atEnd() || j < (int)testData.size()) {
				smaFileLine = smaFileReader.readLine();
				// SMA乖離率(終値)データ取得
				if (smaFileLine != "") {
					if (testData[j].mDate == QDate::fromString((smaFileLine.split(",").at(0)), "yyyy/M/d")) {
						testData[j].mSMAR_close = smaFileLine.split(",").at(10).toDouble();
					}
					j++;
				}
			}
		}
		smaFile.close();

		// 期間高値ファイルオープン
		thFileName = "DB\\TermHigh_" + temp_1.setNum(tN) + "\\" + temp_2.setNum(i) + ".dat";
		thFile.setFileName(thFileName);
		if (thFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			int j = tN;
			QTextStream thFileReader(&thFile);
			thFileLine = "";

			// 期間高値データの読み取り
			while (!thFileReader.atEnd() || j < (int)testData.size()) {
				thFileLine = thFileReader.readLine();
				if (thFileLine != "") {
					if (testData[j].mDate == QDate::fromString((thFileLine.split(",").at(0)), "yyyy/M/d")) {
						testData[j].mTHigh = thFileLine.split(",").at(4).toDouble();
					}
					j++;
				}
			}
		}
		thFile.close();

		// 期間安値ファイルオープン
		tlFileName = "DB\\TermLow_" + temp_1.setNum(tN) + "\\" + temp_2.setNum(i) + ".dat";
		tlFile.setFileName(tlFileName);
		if (tlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			int j = tN;
			QTextStream tlFileReader(&tlFile);
			tlFileLine = "";

			// 期間安値データの読み取り
			while (!tlFileReader.atEnd() || j < (int)testData.size()) {
				tlFileLine = tlFileReader.readLine();
				if (tlFileLine != "") {
					if (testData[j].mDate == QDate::fromString((tlFileLine.split(",").at(0)), "yyyy/M/d")) {
						testData[j].mTLow = tlFileLine.split(",").at(4).toDouble();
					}
					j++;
				}
			}
		}
		tlFile.close();

		// バックテスト実行
		for (int j = 1; j < (int)testData.size(); j++) {
			// 前日に売りシグナル点灯中
			if (testData[j-1].mSellSignal) {
				// シグナルと持ち株数と資金の初期値は前日のものを継続
				testData[j].mBuySignal		= testData[j-1].mBuySignal;
				testData[j].mSellSignal		= testData[j-1].mSellSignal;
				testData[j].mBuyNum			= testData[j-1].mBuyNum;
				testData[j].mSellNum		= testData[j-1].mSellNum;
				testData[j].mBuyPlice		= testData[j-1].mBuyPlice;
				testData[j].mSellPlice		= testData[j-1].mSellPlice;
				testData[j].mMoney			= testData[j-1].mMoney;

				// 評価額は売り価格から前日終値を引いて、株数を掛けたものを、売り価格*保有株数に足して、資金に足す
				testData[j].mEMoney = testData[j].mMoney + ((testData[j].mSellPlice - testData[j-1].mClose) * testData[j].mSellNum) + (testData[j].mSellPlice * testData[j].mSellNum);

				// 前日SMA乖離率が負なら始値で利益確定してシグナルを消す
				if (testData[j-1].mSMAR_close < 0) {
					// 資金は売り価格*保有株数から当日始値*保有株数を引いて、売り価格*保有株数に足して、資金に足す
					testData[j].mMoney += ((testData[j].mSellPlice - testData[j].mOpen) * testData[j].mSellNum) + (testData[j].mSellPlice * testData[j].mSellNum);
					// シグナルを消して持ち株数と持ち株価格を0にする
					testData[j].mSellNum = 0;
					testData[j].mSellPlice = 0;
					testData[j].mSellSignal = false;
					// 評価額は資金と同値
					testData[j].mEMoney = testData[j].mMoney;
				}
			}
			// 買いシグナル点灯中
			else if (testData[j-1].mBuySignal) {
				// シグナルと持ち株数と資金の初期値は前日のものを継続
				testData[j].mBuySignal		= testData[j-1].mBuySignal;
				testData[j].mSellSignal		= testData[j-1].mSellSignal;
				testData[j].mBuyNum			= testData[j-1].mBuyNum;
				testData[j].mSellNum		= testData[j-1].mSellNum;
				testData[j].mBuyPlice		= testData[j-1].mBuyPlice;
				testData[j].mSellPlice		= testData[j-1].mSellPlice;
				testData[j].mMoney			= testData[j-1].mMoney;

				// 評価額は前日終値から買い価格を引いて、株数を掛けたものを、買い価格*保有株数に足して、資金に足す
				testData[j].mEMoney = testData[j].mMoney + ((testData[j-1].mClose - testData[j].mBuyPlice) * testData[j].mBuyNum) + (testData[j].mBuyPlice * testData[j].mBuyNum);

				// 前日SMA乖離率が正なら始値で利益確定してシグナルを消す
				if (testData[j-1].mSMAR_close > 0) {
					// 資金は当日始値*保有株数から買い価格*保有株数を引いて、買い価格*保有株数に足して、資金に足す
					testData[j].mMoney += ((testData[j].mOpen - testData[j].mBuyPlice) * testData[j].mBuyNum) + (testData[j].mBuyPlice * testData[j].mBuyNum);
					// シグナルを消して持ち株数と持ち株価格を0にする
					testData[j].mBuyNum = 0;
					testData[j].mBuyPlice = 0;
					testData[j].mBuySignal = false;
					// 評価額は資金と同値
					testData[j].mEMoney = testData[j].mMoney;
				}
			}
			// シグナルなし
			else {
				// シグナルと持ち株数と資金の初期値は前日のものを継続
				testData[j].mBuySignal = testData[j-1].mBuySignal;
				testData[j].mSellSignal = testData[j-1].mSellSignal;
				testData[j].mBuyNum = testData[j-1].mBuyNum;
				testData[j].mSellNum = testData[j-1].mSellNum;
				testData[j].mMoney = testData[j-1].mMoney;

				// 評価額は資金と同値
				testData[j].mEMoney = testData[j].mMoney;

				// エントリー
				// 前日SMA乖離率が閾値を上に超えた場合、売りシグナル発生。寄付に売り
				if (testData[j-1].mSMAR_close > thresh) {
					testData[j].mSellSignal = true;
					testData[j].mSellNum = (int)(testData[j].mMoney / testData[j].mOpen);
					testData[j].mSellPlice = testData[j].mOpen;
					testData[j].mMoney -= (double)testData[j].mSellNum * testData[j].mOpen;
					// 評価額は売り価格から当日終値を引いて、株数を掛けたものを、売り価格*保有株数に足して、資金に足す
					testData[j].mEMoney = testData[j].mMoney + ((testData[j].mSellPlice - testData[j].mClose) * testData[j].mSellNum) + (testData[j].mSellPlice * testData[j].mSellNum);
				}
				// 前日SMA乖離率が閾値を下に超えた場合、買いシグナル発生。寄付に買い
				else if (testData[j-1].mSMAR_close < -thresh) {
					testData[j].mBuySignal = true;
					testData[j].mBuyNum = (int)(testData[j].mMoney / testData[j].mOpen);
					testData[j].mBuyPlice = testData[j].mOpen;
					testData[j].mMoney -= (double)testData[j].mBuyNum * testData[j].mOpen;
					// 評価額は当日終値から買い価格を引いて、株数を掛けたものを、買い価格*保有株数に足して、資金に足す
					testData[j].mEMoney = testData[j].mMoney + ((testData[j].mClose - testData[j].mBuyPlice) * testData[j].mBuyNum) + (testData[j].mBuyPlice * testData[j].mBuyNum);
				}
			}
		}

		// 書き込み用ファイルオープン
		writeFileName = "BackTest\\" + temp_1.setNum(i) + ".dat";
		writeFile.setFileName(writeFileName);
		if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		// 書き込み処理
		for (int j = 0; j < (int)testData.size(); j++) {
			QTextStream out(&writeFile);

			// バックテスト1データの書き込み
			out << (testData[j].mDate.toString("yyyy/M/d")) << ","
				<< (int)testData[j].mEMoney << ","						// doubleだと誤差が出る
				<< (int)testData[j].mMoney << ","						// doubleだと誤差が出る
				<< testData[j].mBuySignal << ","
				<< testData[j].mSellSignal << ","
				<< testData[j].mBuyNum << ","
				<< testData[j].mSellNum << ","
				<< testData[j].mBuyPlice << ","
				<< testData[j].mSellPlice << ","
				<< testData[j].mOpen << ","
				<< testData[j].mHigh << ","
				<< testData[j].mLow << ","
				<< testData[j].mClose << ","
				<< testData[j].mVolume << ","
				<< testData[j].mAClose << ","
				<< testData[j].mSMAR_close << ","
				<< testData[j].mTHigh << ","
				<< testData[j].mTLow << "\n";
		}

		writeFile.close();
	}
}
