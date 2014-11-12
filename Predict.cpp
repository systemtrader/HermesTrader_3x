#include "Predict.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <direct.h>
#include <math.h>
#include <algorithm>

/**
 *	コンストラクタ
 */
Predict::Predict()
{
}

/**
 *	スコア計算
 */
void Predict::CalculateScore(QString rootPath, int fragmentLength, double minScore, double minS, int start)
{
	// 変数宣言
	QString targetRateFileName;					// TargetRateファイル名
	QString templateRateFileName;				// TemplateRateファイル名
	QString scoreFileName;						// Scoreファイル名
	QString readTargetLine;						// TargetTextリーダー
	QString readTemplateLine;					// TemplateTextリーダー
	double	sumTargetRate, averageTargetRate, sumPowerTargetRate, squareSumPowerTargetRate,
			sumTemplateRate, averageTemplateRate, sumPowerTemplateRate, squareSumPowerTemplateRate,
			numerator, denominate, score, targetS, templateS;
	vector <double> remainderTargetRate, powerTargetRate, remainderTemplateRate, powerTemplateRate;
	bool flag;

	// Scoreディレクトリ生成
	_mkdir(rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Score\\");

	// TargetRate処理ループ
	for (int targetRateFileNum = 1000; targetRateFileNum < 1000000; targetRateFileNum++) {
		if (((targetRateFileNum > 10000) && !((targetRateFileNum == 998405) || (targetRateFileNum == 998407)))) {
			continue;
		}

		// 読み込み用ファイルオープン
		targetRateFileName = rootPath + "\\TargetRate\\" + mTemp1.setNum(targetRateFileNum) + ".rate";
		QFile targetRateFile(targetRateFileName);
		if (targetRateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 書き込み用ファイルオープン
			scoreFileName = scoreFileName = rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Score\\" + mTemp2.setNum(targetRateFileNum) + ".score";
			QFile scoreFile(scoreFileName);
			if (!scoreFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 宣言&初期化
			QTextStream readTargetRate(&targetRateFile);
			readTargetLine = "";
			mTargetDate.clear();
			mTargetRate.clear();

			// rateファイルの読み取り
			while (!readTargetRate.atEnd()) {
				readTargetLine = readTargetRate.readLine();
				// 各カラムのデータ取得
				mTargetDate.push_back(QDate::fromString((readTargetLine.split(",").at(0)), "yyyy/MM/dd"));
				mTargetRate.push_back(readTargetLine.split(",").at(1).toDouble());
			}

			// 計算用変数初期化
			sumTargetRate = 0;
			averageTargetRate = 0;
			remainderTargetRate.clear();
			powerTargetRate.clear();
			sumPowerTargetRate = 0;
			squareSumPowerTargetRate = 0;

			// CurrentTargetRateをベクターに再格納（計算時にTemplateRateとの整合性を保つため）
			mCurrentTargetRate.clear();
			for (int TargetRateItemCount = mTargetRate.size()-fragmentLength-start; TargetRateItemCount < (int)mTargetRate.size()-start; TargetRateItemCount++) {
				mCurrentTargetRate.push_back(mTargetRate[TargetRateItemCount]);
			}

			// TargetRateのフラグメント長分の和を計算
			for (int targetRateItemCount = 0; targetRateItemCount < fragmentLength; targetRateItemCount++) {
				sumTargetRate += mCurrentTargetRate[targetRateItemCount];
			}

			// TargetRateのフラグメント長分の相加平均
			averageTargetRate = sumTargetRate / fragmentLength;

			// TargetRateの相加平均との差を算出して二乗する
			for (int targetRateItemCount = 0; targetRateItemCount < fragmentLength; targetRateItemCount++) {
				remainderTargetRate.push_back(mCurrentTargetRate[targetRateItemCount] - averageTargetRate);						// TargetRateの相加平均との差
				powerTargetRate.push_back(remainderTargetRate[targetRateItemCount] * remainderTargetRate[targetRateItemCount]);	// TargetRateのRemainderの二乗
			}

			// TargetRateのPowerの和
			for (int targetRateItemCount = 0; targetRateItemCount < fragmentLength; targetRateItemCount++) {
				sumPowerTargetRate += powerTargetRate[targetRateItemCount];
			}

			// TargetRateのSumPowerの平方根
			squareSumPowerTargetRate = sqrt(sumPowerTargetRate);

			// TemplateRate処理ループ
			for (int templateRateFileNum = 1000; templateRateFileNum < 1000000; templateRateFileNum++) {
				if (((templateRateFileNum > 10000) && !((templateRateFileNum == 998405) || (templateRateFileNum == 998407)))) {
					continue;
				}

				// 読み込み用ファイルオープン
				templateRateFileName = rootPath + "\\TemplateRate\\" + mTemp1.setNum(templateRateFileNum) + ".rate";
				QFile templateRateFile(templateRateFileName);

				if (templateRateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
					// 宣言&初期化
					QTextStream readTemplateRate(&templateRateFile);
					readTemplateLine = "";
					mTemplateDate.clear();
					mTemplateRate.clear();

					// rateファイルの読み取り
					while (!readTemplateRate.atEnd()) {
						readTemplateLine = readTemplateRate.readLine();
						// 各カラムのデータ取得
						mTemplateDate.push_back(QDate::fromString((readTemplateLine.split(",").at(0)), "yyyy/MM/dd"));
						mTemplateRate.push_back(readTemplateLine.split(",").at(1).toDouble());
					}

					// TemplateRateファイルの開始行を一行ずつ変更
					for (int TemplateRateItemCount = 0; TemplateRateItemCount < (int)mTemplateRate.size() - fragmentLength + 1; TemplateRateItemCount++) {
						// 計算用変数初期化
						sumTemplateRate = 0;
						averageTemplateRate = 0;
						remainderTemplateRate.clear();
						powerTemplateRate.clear();
						sumPowerTemplateRate = 0;
						squareSumPowerTemplateRate = 0;
						numerator = 0;
						denominate = 0;
						score = 0;

						// CurrentTemplateRateをベクターに再格納（計算時にTargetRateとの整合性を保つため）
						mCurrentTemplateRate.clear();
						for (int currentTemplateRateItemCount = TemplateRateItemCount; currentTemplateRateItemCount < TemplateRateItemCount + fragmentLength; currentTemplateRateItemCount++) {
							mCurrentTemplateRate.push_back(mTemplateRate[currentTemplateRateItemCount]);
						}

						// TargetRateとTemplateRateを比較して、差が閾値以下でない場合は即座に次のデータに映る
						flag = false;
						for (int fACount = 0; fACount < fragmentLength; fACount++) {
							if (abs(mCurrentTemplateRate[fACount] - mCurrentTargetRate[fACount]) < 0.001) {
								flag = true;
								break;
							}
						}
						if (!flag) {
							continue;
						}

						// TemplateRateの和を計算
						for (int fACount = 0; fACount < fragmentLength; fACount++) {
							sumTemplateRate += mCurrentTemplateRate[fACount];
						}

						// TemplateRateの相加平均を計算
						averageTemplateRate = sumTemplateRate / fragmentLength;

						// TemplateRateの相加平均との差を計算して二乗
						for (int fACount = 0; fACount < fragmentLength; fACount++) {
							remainderTemplateRate.push_back(mCurrentTemplateRate[fACount] - averageTemplateRate);				// TemplateRateの相加平均との差を計算
							powerTemplateRate.push_back(remainderTemplateRate[fACount] * remainderTemplateRate[fACount]);		// TemplateRateのRemainderの二乗
						}

						// TemplateRateのPowerの和
						for (int fACount = 0; fACount < fragmentLength; fACount++) {
							sumPowerTemplateRate += powerTemplateRate[fACount];
							numerator += (mCurrentTargetRate[fACount] - averageTargetRate) * (mCurrentTemplateRate[fACount] - averageTemplateRate); // 相関係数の分子の計算
						}

						// TemplateRateのSumPowerの平方根
						squareSumPowerTemplateRate = sqrt(sumPowerTemplateRate);

						// 相関係数の分母の計算
						denominate = squareSumPowerTargetRate * squareSumPowerTemplateRate;

						// 相関係数の計算
						score = numerator / denominate;

						// Template,Target各々の開始価格と終了価格の騰落率差計算
						targetS = 10000;
						templateS = 10000;
						for (int fACount = 0; fACount < fragmentLength; fACount++) {
							targetS = targetS + (targetS * mCurrentTargetRate[fACount]);
							templateS = templateS + (templateS * mCurrentTemplateRate[fACount]);
						}

						// 相関係数がminScoreよりも大きければ出力
						if (score >= minScore) {
							// 開始価格と終了価格の騰落率差が閾値未満なら出力
							if (abs(targetS - templateS) < minS) {
								// 書き込み用ストリーム作成
								QTextStream out(&scoreFile);
								// TargetとTemplateのRateファイル名書き込み
								out << targetRateFileNum << ",";
								out << templateRateFileNum << ",";
								// TemplateRate内の現在日付書き込み
								out << mTemplateDate[TemplateRateItemCount].toString("yyyy/MM/dd") << ",";
								// 相関係数の書き込み
								out << "Score" << "=" << score << "\n";
							}
						}
					}
				}
				templateRateFile.close();
			}

			scoreFile.close();
		}

		targetRateFile.close();
	}
}

/**
 *	Prediction()
 *
 *	@param[in] filteredScoreFilePath		抽出するスコアの最小値
 *	@param[in] predictLength				処理を行うScoreディレクトリ内の（ターゲット）ディレクトリ番号
 */
void Predict::Prediction(QString rootPath, int targetID, int fragmentLength, int predictLength, int start)
{
	// 変数宣言
	TemplateScoreData tempData;
	QString scoreFilePath;
	QString readScoreLine;
	QString templateFilePath;
	QString readTemplateFileLine;
	QString predictionFilePath;
	QFile scoreFile;
	QFile templateFile;
	QFile predictionFile;
	int rowFromOrigin;
	int rowOfStartDate;
	vector <TemplateScoreData>	templateScoreData;
	double upCount, downCount, sameCount;
	double sumPrediction;
	CasePliceData tempPlice;

	// 変数初期化
	mTemplateScoreData.clear();
	templateScoreData.clear();
	mBestCasePliceData.clear();
	mWorstCasePliceData.clear();
	mCenterCasePliceData.clear();

	// Predictionディレクトリ生成
	_mkdir(rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Prediction\\");

	// ScoreFile名取得
	scoreFilePath = rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Score\\" + mTemp2.setNum(targetID) + ".score";

	// ScoreFileを開く
	scoreFile.setFileName(scoreFilePath);
	if (!scoreFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	QTextStream readScore(&scoreFile);

	// ファイル内のデータ取得
	while (!readScore.atEnd()) {
		readScoreLine = readScore.readLine();

		// TemplateScoreData配列の作成
		if (readScoreLine != "") {
			tempData.mID			= readScoreLine.split(",").at(1).toInt();
			tempData.mScore			= readScoreLine.split(",").at(3).split("=").at(1).toDouble();
			tempData.mOriginDate	= QDate::fromString(readScoreLine.split(",").at(2), "yyyy/MM/dd");
			tempData.mStartDate		= QDate::fromString(readScoreLine.split(",").at(2), "yyyy/MM/dd");
			tempData.mStartOpen		= 0;
			tempData.mStartHigh		= 0;
			tempData.mStartLow		= 0;
			tempData.mStartClose	= 0;
			tempData.mStartVolume	= 0;
			tempData.mStartAClose	= 0;
			tempData.mEndDate		= QDate::fromString(readScoreLine.split(",").at(2), "yyyy/MM/dd");
			tempData.mEndOpen		= 0;
			tempData.mEndHigh		= 0;
			tempData.mEndLow		= 0;
			tempData.mEndClose		= 0;
			tempData.mEndVolume		= 0;
			tempData.mEndAClose		= 0;
			mTemplateScoreData.push_back(tempData);
		}
	}

	// データがない場合はreturn
	if (mTemplateScoreData.size() == 0) {
		return;
	}

	// 各TemplateScoreDataの詳細データを取得
	for (int i = 0; i < (int)mTemplateScoreData.size(); i++) {
		// 経過行を0に初期化
		rowFromOrigin = 0;
		rowOfStartDate = 0;

		// Templateファイル名取得
		templateFilePath = rootPath + "TemplateDB\\" + mTemp1.setNum(mTemplateScoreData[i].mID) + ".csv";

		// Templateファイルを開く
		templateFile.setFileName(templateFilePath);
		if (templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream readTemplateFile(&templateFile);

			// ファイル内のデータ取得
			while (!readTemplateFile.atEnd()) {
				readTemplateFileLine = readTemplateFile.readLine();

				// Templateファイルのフラグメントアセンブリ開始日を取得
				if (QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy") == mTemplateScoreData[i].mOriginDate) {
					// フラグメントアセンブリ開始日の経過行を取得
					rowOfStartDate = rowFromOrigin;
				}

				// 経過行をインクリメント
				rowFromOrigin++;

				if (rowOfStartDate != 0) {
					// 経過行が予測開始日(=フラグメントアセンブリ開始日+フラグメント長)になったらTemplateファイルの開始日データを取得
					if (rowFromOrigin == rowOfStartDate + fragmentLength) {
						// TemplateScoreData配列の修正
						mTemplateScoreData[i].mStartDate	= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy");
						mTemplateScoreData[i].mStartOpen	= readTemplateFileLine.split(",").at(1).toDouble();
						mTemplateScoreData[i].mStartHigh	= readTemplateFileLine.split(",").at(2).toDouble();
						mTemplateScoreData[i].mStartLow		= readTemplateFileLine.split(",").at(3).toDouble();
						mTemplateScoreData[i].mStartClose	= readTemplateFileLine.split(",").at(4).toDouble();
						mTemplateScoreData[i].mStartVolume	= readTemplateFileLine.split(",").at(5).toDouble();
						mTemplateScoreData[i].mStartAClose	= readTemplateFileLine.split(",").at(6).toDouble();
					}

					// 経過行が予測終了日になったらTemplateファイルの終了日データを取得
					if (rowFromOrigin == rowOfStartDate + fragmentLength + predictLength) {
						// TemplateScoreData配列の修正
						mTemplateScoreData[i].mEndDate		= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy");
						mTemplateScoreData[i].mEndOpen		= readTemplateFileLine.split(",").at(1).toDouble();
						mTemplateScoreData[i].mEndHigh		= readTemplateFileLine.split(",").at(2).toDouble();
						mTemplateScoreData[i].mEndLow		= readTemplateFileLine.split(",").at(3).toDouble();
						mTemplateScoreData[i].mEndClose		= readTemplateFileLine.split(",").at(4).toDouble();
						mTemplateScoreData[i].mEndVolume	= readTemplateFileLine.split(",").at(5).toDouble();
						mTemplateScoreData[i].mEndAClose	= readTemplateFileLine.split(",").at(6).toDouble();
						mTemplateScoreData[i].mPrediction	= ((mTemplateScoreData[i].mEndClose - mTemplateScoreData[i].mStartClose) / mTemplateScoreData[i].mStartClose);		// 終値-前終値
						//mTemplateScoreData[i].mPrediction2	= ((mTemplateScoreData[i].mEndClose - mTemplateScoreData[i].mEndOpen) / mTemplateScoreData[i].mStartClose);			// 終値-始値
					}
				}
			}
		}

		templateFile.close();

		// EndDateがStartDate以前にあるデータは有効データ外の可能性が高いため除外する
		if (mTemplateScoreData[i].mEndDate.toJulianDay() <= mTemplateScoreData[i].mStartDate.toJulianDay()) {
			templateScoreData = mTemplateScoreData;
			mTemplateScoreData.clear();
			for (int j = 0; j < i; j++) {
				mTemplateScoreData.push_back(templateScoreData[j]);
			}
			for (int j = i + 1; j < (int)templateScoreData.size(); j++) {
				mTemplateScoreData.push_back(templateScoreData[j]);
			}
			i = i - 1;
		}
	}

	scoreFile.close();

	// mTemplateScoreData配列を予測騰落率順にソート
	sort(mTemplateScoreData.begin(), mTemplateScoreData.end(), comparePrediction<TemplateScoreData>());

	//-------------------------
	// Prediction1
	//-------------------------
	// 予想騰落率の上昇個数と下落個数と一定個数を算出する
	upCount = 0;
	downCount = 0;
	sameCount = 0;
	sumPrediction = 0;
	for (int i = 0; i < (int)mTemplateScoreData.size(); i++) {
		if (mTemplateScoreData[i].mPrediction > 0) {
			upCount++;
		}
		else if (mTemplateScoreData[i].mPrediction < 0) {
			downCount++;
		}
		else {
			sameCount++;
		}

		// 予測騰落率平均値の計算
		sumPrediction += mTemplateScoreData[i].mPrediction;
	}

	// 予測騰落率平均値の計算
	sumPrediction = sumPrediction / (upCount + downCount + sameCount);

	// 書き込み用ファイルオープン
	predictionFilePath = rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Prediction\\" + mTemp2.setNum(targetID) + ".prediction";
	predictionFile.setFileName(predictionFilePath);
	if (!predictionFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return;
	}

	// 書き込み処理
	QTextStream out(&predictionFile);
	out << "Max Prediction Rate:" << mTemplateScoreData[0].mPrediction << "\n";											// 最大予測騰落率
	out << "Min Prediction Rate:" << mTemplateScoreData[mTemplateScoreData.size()-1].mPrediction << "\n";				// 最小予測騰落率
	out << "Center Prediction Rate:" << mTemplateScoreData[(mTemplateScoreData.size()-1)/2].mPrediction << "\n";		// 中央予測騰落率
	out << "Average Prediction Rate:" << sumPrediction << "\n";															// 平均予測騰落率
	out << "Up Probability:" << (upCount / (upCount + downCount + sameCount)) << "\n";									// 上昇確率
	out << "Down Probability:" << (downCount / (upCount + downCount + sameCount)) << "\n";								// 下落確率
	out << "Same Probability:" << (sameCount / (upCount + downCount + sameCount)) << "\n";								// 不変確率
	for (int i = 0; i < (int)mTemplateScoreData.size(); i++) {
		// Predictionデータの書き込み
		out << (mTemplateScoreData[i].mID) << ","
			<< (mTemplateScoreData[i].mScore) << ","
			<< (mTemplateScoreData[i].mStartDate.toString("yyyy/MM/dd")) << ","
			<< (mTemplateScoreData[i].mStartClose) << ","
			<< (mTemplateScoreData[i].mEndDate.toString("yyyy/MM/dd")) << ","
			<< (mTemplateScoreData[i].mEndClose) << ","
			<< (mTemplateScoreData[i].mPrediction) << "\n";
	}

	predictionFile.close();

	// BestCaseのグラフ用パラメータ
	// Templateファイル名取得
	templateFilePath = rootPath + "TemplateDB\\" + mTemp1.setNum(mTemplateScoreData[0].mID) + ".csv";
	// Templateファイルを開く
	templateFile.setFileName(templateFilePath);
	if (templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream readTemplateFile(&templateFile);

		// ファイル内のデータ取得
		while (!readTemplateFile.atEnd()) {
			readTemplateFileLine = readTemplateFile.readLine();

			// フラグメントアセンブリ開始日から予測終了日までのデータを取得
			if ((QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay() >= mTemplateScoreData[0].mOriginDate.toJulianDay()) &&
				(QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay() <= mTemplateScoreData[0].mEndDate.toJulianDay())) {
				tempPlice.mDate			= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy");
				tempPlice.mDateJulian	= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay();
				tempPlice.mOpen			= readTemplateFileLine.split(",").at(1).toDouble();
				tempPlice.mHigh			= readTemplateFileLine.split(",").at(2).toDouble();
				tempPlice.mLow			= readTemplateFileLine.split(",").at(3).toDouble();
				tempPlice.mClose		= readTemplateFileLine.split(",").at(4).toDouble();
				tempPlice.mVolume		= readTemplateFileLine.split(",").at(5).toDouble();
				tempPlice.mAClose		= readTemplateFileLine.split(",").at(6).toDouble();
				tempPlice.mStartClose	= mTemplateScoreData[0].mStartClose;
				tempPlice.mEndClose		= mTemplateScoreData[0].mEndClose;
				tempPlice.mPrediction	= mTemplateScoreData[0].mPrediction;
				mBestCasePliceData.push_back(tempPlice);
			}
		}
	}

	templateFile.close();

	// WorstCaseのグラフ用パラメータ
	// Templateファイル名取得
	templateFilePath = rootPath + "TemplateDB\\" + mTemp1.setNum(mTemplateScoreData[mTemplateScoreData.size()-1].mID) + ".csv";
	// Templateファイルを開く
	templateFile.setFileName(templateFilePath);
	if (templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream readTemplateFile(&templateFile);

		// ファイル内のデータ取得
		while (!readTemplateFile.atEnd()) {
			readTemplateFileLine = readTemplateFile.readLine();

			// フラグメントアセンブリ開始日から予測終了日までのデータを取得
			if ((QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay() >= mTemplateScoreData[mTemplateScoreData.size()-1].mOriginDate.toJulianDay()) &&
				(QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay() <= mTemplateScoreData[mTemplateScoreData.size()-1].mEndDate.toJulianDay())) {
				tempPlice.mDate			= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy");
				tempPlice.mDateJulian	= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay();
				tempPlice.mOpen			= readTemplateFileLine.split(",").at(1).toDouble();
				tempPlice.mHigh			= readTemplateFileLine.split(",").at(2).toDouble();
				tempPlice.mLow			= readTemplateFileLine.split(",").at(3).toDouble();
				tempPlice.mClose		= readTemplateFileLine.split(",").at(4).toDouble();
				tempPlice.mVolume		= readTemplateFileLine.split(",").at(5).toDouble();
				tempPlice.mAClose		= readTemplateFileLine.split(",").at(6).toDouble();
				tempPlice.mStartClose	= mTemplateScoreData[mTemplateScoreData.size()-1].mStartClose;
				tempPlice.mEndClose		= mTemplateScoreData[mTemplateScoreData.size()-1].mEndClose;
				tempPlice.mPrediction	= mTemplateScoreData[mTemplateScoreData.size()-1].mPrediction;
				mWorstCasePliceData.push_back(tempPlice);
			}
		}
	}

	templateFile.close();

	// CenterCaseのグラフ用パラメータ
	// Templateファイル名取得
	templateFilePath = rootPath + "TemplateDB\\" + mTemp1.setNum(mTemplateScoreData[(mTemplateScoreData.size()-1)/2].mID) + ".csv";
	// Templateファイルを開く
	templateFile.setFileName(templateFilePath);
	if (templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream readTemplateFile(&templateFile);

		// ファイル内のデータ取得
		while (!readTemplateFile.atEnd()) {
			readTemplateFileLine = readTemplateFile.readLine();

			// フラグメントアセンブリ開始日から予測終了日までのデータを取得
			if ((QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay() >= mTemplateScoreData[(mTemplateScoreData.size()-1)/2].mOriginDate.toJulianDay()) &&
				(QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay() <= mTemplateScoreData[(mTemplateScoreData.size()-1)/2].mEndDate.toJulianDay())) {
				tempPlice.mDate			= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy");
				tempPlice.mDateJulian	= QDate::fromString(readTemplateFileLine.split(",").at(0), "M/d/yyyy").toJulianDay();
				tempPlice.mOpen			= readTemplateFileLine.split(",").at(1).toDouble();
				tempPlice.mHigh			= readTemplateFileLine.split(",").at(2).toDouble();
				tempPlice.mLow			= readTemplateFileLine.split(",").at(3).toDouble();
				tempPlice.mClose		= readTemplateFileLine.split(",").at(4).toDouble();
				tempPlice.mVolume		= readTemplateFileLine.split(",").at(5).toDouble();
				tempPlice.mAClose		= readTemplateFileLine.split(",").at(6).toDouble();
				tempPlice.mStartClose	= mTemplateScoreData[(mTemplateScoreData.size()-1)/2].mStartClose;
				tempPlice.mEndClose		= mTemplateScoreData[(mTemplateScoreData.size()-1)/2].mEndClose;
				tempPlice.mPrediction	= mTemplateScoreData[(mTemplateScoreData.size()-1)/2].mPrediction;
				mCenterCasePliceData.push_back(tempPlice);
			}
		}
	}

	templateFile.close();
}

/**
 *	Screening()
 *
 *	@param[in] filteredScoreFilePath		抽出するスコアの最小値
 *	@param[in] predictLength				処理を行うScoreディレクトリ内の（ターゲット）ディレクトリ番号
 */
void Predict::Screening(QString rootPath, int start)
{
	// 変数宣言
	ScreeningData tempData;
	QString readPredictionLine;
	QString predictionFilePath;
	QFile predictionFile;
	QString screeningFilePath;
	QFile screeningFile;

	// 変数初期化
	mScreeningData.clear();

	// Screeningディレクトリ生成
	_mkdir(rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Screening\\");

	for (int i = 1000; i < 1000000; i++) {
		if (((i > 10000) && !((i == 998405) || (i == 998407)))) {
			continue;
		}

		// PredictionFile名取得
		predictionFilePath = rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Prediction\\" + mTemp2.setNum(i) + ".prediction";

		// PredictionFileを開く
		predictionFile.setFileName(predictionFilePath);
		if (predictionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream readPrediction(&predictionFile);

			// ファイル内のデータ取得
			while (!readPrediction.atEnd()) {
				readPredictionLine = readPrediction.readLine();

				// 統計データの取得
				tempData.mID = i;
				if (readPredictionLine.split(":").at(0) == "Max Prediction Rate") {
					tempData.mMaxPredictionRate = readPredictionLine.split(":").at(1).toDouble();
				}
				else if (readPredictionLine.split(":").at(0) == "Min Prediction Rate") {
					tempData.mMinPredictionRate = readPredictionLine.split(":").at(1).toDouble();
				}
				else if (readPredictionLine.split(":").at(0) == "Center Prediction Rate") {
					tempData.mCenterPredictionRate = readPredictionLine.split(":").at(1).toDouble();
				}
				else if (readPredictionLine.split(":").at(0) == "Average Prediction Rate") {
					tempData.mAveragePredictionRate = readPredictionLine.split(":").at(1).toDouble();
				}
				else if (readPredictionLine.split(":").at(0) == "Up Probability") {
					tempData.mUpProbability = readPredictionLine.split(":").at(1).toDouble();
				}
				else if (readPredictionLine.split(":").at(0) == "Down Probability") {
					tempData.mDownProbability = readPredictionLine.split(":").at(1).toDouble();
				}
				else if (readPredictionLine.split(":").at(0) == "Same Probability") {
					tempData.mSameProbability = readPredictionLine.split(":").at(1).toDouble();
				}
			}
			mScreeningData.push_back(tempData);
		}

		predictionFile.close();
	}

	// 書き込み用ファイルオープン
	screeningFilePath = rootPath.toLocal8Bit() + mTemp1.setNum(start).toLocal8Bit() + "_Screening\\target.csv";
	screeningFile.setFileName(screeningFilePath);
	if (!screeningFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return;
	}

	// 書き込み処理
	QTextStream out(&screeningFile);
	out << "Target ID,";
	out << "Max Prediction Rate,";
	out << "Min Prediction Rate,";
	out << "Center Prediction Rate,";
	out << "Average Prediction Rate,";
	out << "Up Probability,";
	out << "Down Probability,";
	out << "Same Probability\n";
	for (int i = 0; i < (int)mScreeningData.size(); i++) {
		// 統計データの書き込み
		out << (mScreeningData[i].mID) << ","
			<< (mScreeningData[i].mMaxPredictionRate) << ","
			<< (mScreeningData[i].mMinPredictionRate) << ","
			<< (mScreeningData[i].mCenterPredictionRate) << ","
			<< (mScreeningData[i].mAveragePredictionRate) << ","
			<< (mScreeningData[i].mUpProbability) << ","
			<< (mScreeningData[i].mDownProbability) << ","
			<< (mScreeningData[i].mSameProbability) << "\n";
	}

	screeningFile.close();
}
