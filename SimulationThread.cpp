#include "SimulationThread.h"
#include "BackTestThread.h"
#include <direct.h>

/**
 *	コンストラクタ
 */
SimulationThread::SimulationThread()
{
}

/**
 *	スレッド処理実行
 */
void SimulationThread::run()
{
	Calcuration(mThresh, mMoney, mStartDate, mEndDate);
}

/**
 *	アルゴリズムで使用するパラメータを読み込む
 */
void SimulationThread::ReadParam(int thresh, double money, QDate startDate, QDate endDate)
{
	mThresh			= thresh;
	mMoney			= money;
	mStartDate		= startDate;
	mEndDate		= endDate;
}

/**
 *	計算およびDB構築
 *	エントリー：単純移動平均(終値)乖離率が閾値を超えた翌日の寄付
 *	エグジット：単純移動平均(終値)乖離率がエントリーの逆側に0を超えた翌日の寄付
 *
 *	@param[in] thresh		閾値
 *	@param[in] money		初期資金
 *	@param[in] startDate	開始日
 *	@param[in] endDate		終了日
 */
void SimulationThread::Calcuration(int thresh, double money, QDate startDate, QDate endDate)
{
	// 変数宣言
	QString backTestFileName;		// バックテストファイル名
	QFile backTestFile;				// バックテストファイル
	QString backTestFileLine;		// バックテストファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	vector <BackTest> testData;		// バックテストデータ
	BackTest tempData;				// 一時保存用バックテストデータ
	QString temp_1;					// 一時保存用文字列

	// ディレクトリ生成
	_mkdir("Simulation");

	// 各IDについて処理実行
	for (int i = 1000; i < 1000000; i++) {
		if (((i > 10000) && !((i == 998405) || (i == 998407)))) {
			continue;
		}

		// 変数初期化
		testData.clear();

		// バックテストファイルオープン
		backTestFileName = "BackTest\\" + temp_1.setNum(i) + ".dat";
		backTestFile.setFileName(backTestFileName);

		// バックテストファイルがない場合は次のIDへ
		if (!backTestFile.exists()) {
			continue;
		}

		if (backTestFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			QTextStream backTestFileReader(&backTestFile);
			backTestFileLine = "";

			// バックテストデータの読み取り
			while (!backTestFileReader.atEnd()) {
				backTestFileLine = backTestFileReader.readLine();
				// 開始日から終了日までのバックテストデータ取得
				if (backTestFileLine != "") {
					if ((QDate::fromString((backTestFileLine.split(",").at(0)), "yyyy/M/d").toJulianDay() > startDate.toJulianDay()) &&
						(QDate::fromString((backTestFileLine.split(",").at(0)), "yyyy/M/d").toJulianDay() < endDate.toJulianDay())) {
						tempData.mDate			= QDate::fromString((backTestFileLine.split(",").at(0)), "yyyy/M/d");
						tempData.mEMoney		= money;
						tempData.mMoney			= money;
						tempData.mBuySignal		= backTestFileLine.split(",").at(3).contains("1");
						tempData.mSellSignal	= backTestFileLine.split(",").at(4).contains("1");
						tempData.mBuyNum		= backTestFileLine.split(",").at(5).toInt();
						tempData.mSellNum		= backTestFileLine.split(",").at(6).toInt();
						tempData.mBuyPlice		= backTestFileLine.split(",").at(7).toDouble();
						tempData.mSellPlice		= backTestFileLine.split(",").at(8).toDouble();
						tempData.mOpen			= backTestFileLine.split(",").at(9).toDouble();
						tempData.mHigh			= backTestFileLine.split(",").at(10).toDouble();
						tempData.mLow			= backTestFileLine.split(",").at(11).toDouble();
						tempData.mClose			= backTestFileLine.split(",").at(12).toDouble();
						tempData.mVolume		= backTestFileLine.split(",").at(13).toDouble();
						tempData.mAClose		= backTestFileLine.split(",").at(14).toDouble();
						tempData.mSMAR_close	= backTestFileLine.split(",").at(15).toDouble();
						tempData.mTHigh			= backTestFileLine.split(",").at(16).toDouble();
						tempData.mTLow			= backTestFileLine.split(",").at(17).toDouble();
						testData.push_back(tempData);
					}
				}
			}
		}
		backTestFile.close();

		// testDataがない場合は次のIDへ
		if (testData.size() == 0) {
			continue;
		}

		// 再度バックテスト実行
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
		writeFileName = "Simulation\\" + temp_1.setNum(i) + ".dat";
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
