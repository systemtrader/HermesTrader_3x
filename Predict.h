#ifndef PREDICT_H
#define PREDICT_H

#include <QtGui>
#include <QDate>

using namespace std;

// TemplateScoreDataクラス
class TemplateScoreData {
public:
	int							mID;																				// ID(ファイル番号)
	double						mScore;																				// スコア
	QDate						mOriginDate;																		// フラグメントアセンブリ開始日
	QDate						mStartDate;																			// 予測開始日(=フラグメントアセンブリ終了日)
	double						mStartOpen;																			// 開始日の始値
	double						mStartHigh;																			// 開始日の最高値
	double						mStartLow;																			// 開始日の最安値
	double						mStartClose;																		// 開始日の終値
	double						mStartVolume;																		// 開始日のボラリティ
	double						mStartAClose;																		// 開始日の調整後終値
	QDate						mEndDate;																			// 予測終了日
	double						mEndOpen;																			// 終了日の始値
	double						mEndHigh;																			// 終了日の最高値
	double						mEndLow;																			// 終了日の最安値
	double						mEndClose;																			// 終了日の終値
	double						mEndVolume;																			// 終了日のボラリティ
	double						mEndAClose;																		// 終了日の調整後終値
	double						mPrediction;																		// 予測騰落率(終値-前日終値)
};

// PliceDataクラス
class CasePliceData {
public:
	QDate						mDate;
	double						mDateJulian;
	double						mOpen;
	double						mHigh;
	double						mLow;
	double						mClose;
	double						mVolume;
	double						mAClose;
	double						mStartClose;																		// 開始日の終値
	double						mEndClose;																			// 終了日の終値
	double						mPrediction;																		// 予測騰落率
};

// ScreeningDataクラス
class ScreeningData {
public:
	int							mID;
	double						mMaxPredictionRate;
	double						mMinPredictionRate;
	double						mCenterPredictionRate;
	double						mAveragePredictionRate;
	double						mUpProbability;
	double						mDownProbability;
	double						mSameProbability;
};

class Predict
{
public:
								Predict();
	void						CalculateScore(QString rootPath, int fragmentLength, double minScore, double minS, int start);	// スコア計算
	void						Prediction(QString rootPath, int targetID, int fragmentLength, int predictLength, int start);	// 予測
	void						Screening(QString rootPath, int start);															// 予測結果のスクリーニング
	vector <CasePliceData>		mBestCasePliceData;																				// BestCase価格データ配列
	vector <CasePliceData>		mWorstCasePliceData;																			// WorstCase価格データ配列
	vector <CasePliceData>		mCenterCasePliceData;																			// CenterCase価格データ配列

private:
	vector <QDate>				mTargetDate;
	vector <QDate>				mTemplateDate;
	vector <double>				mTargetRate;
	vector <double>				mTemplateRate;
	vector <double>				mCurrentTargetRate;
	vector <double>				mCurrentTemplateRate;
	vector <double>				mScore;
	vector <TemplateScoreData>	mTemplateScoreData;
	vector <ScreeningData>		mScreeningData;
	QString						mTemp1;
	QString						mTemp2;
	QString						mTemp3;
};

// TemplateScoreDataの予測騰落率比較関数(降順)
template <class TemplateScoreData> class comparePrediction:
		public binary_function<TemplateScoreData, TemplateScoreData, bool>
{
public:
	bool operator()(TemplateScoreData a, TemplateScoreData b)
	{
		return a.mPrediction > b.mPrediction;
	}
};

extern Predict*					gpPredict;																			// Predictのグローバルポインタ

#endif // PREDICT_H
