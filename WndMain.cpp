#include "WndMain.h"
#include "ui_WndMain.h"
#include "Plot.h"
#include "Template.h"
#include "Target.h"
#include "Predict.h"
#include "DlgChart.h"
#include "DlgAppStart.h"

/**
 *	コンストラクタ
 */
WndMain::WndMain(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WndMain)
{
	ui->setupUi(this);

	// 変数宣言
	QDate getLastDataDate;

	// アルゴリズム用スレッドの初期化
	mpSMAThread				= 0;
	mpEMAThread				= 0;
	mpRMAThread				= 0;
	mpWMAThread				= 0;
	mpChangeThread			= 0;
	mpTermHighThread		= 0;
	mpTermLowThread			= 0;
	mpIchimokuThread		= 0;
	mpBollingerBandThread	= 0;
	mpFibonacciThread		= 0;
	mpBackTestThread		= 0;
	mpSimulationThread		= 0;
	mpSMAThread				= new SMAThread();
	mpEMAThread				= new EMAThread();
	mpRMAThread				= new RMAThread();
	mpWMAThread				= new WMAThread();
	mpChangeThread			= new ChangeThread();
	mpTermHighThread		= new TermHighThread();
	mpTermLowThread			= new TermLowThread();
	mpIchimokuThread		= new IchimokuThread();
	mpBollingerBandThread	= new BollingerBandThread();
	mpFibonacciThread		= new FibonacciThread();
	mpBackTestThread		= new BackTestThread();
	mpSimulationThread		= new SimulationThread();

	// UI初期化
	ui->dedtBackTestEnd->setDate(QDate::currentDate());

	// Targetプロット
	mTargetPlotCurve = new QwtPlotCurve("Plot");
	mTargetPlotCurve->setPen((QPen(Qt::yellow, 4)));
	ui->qwtpltTarget->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltTarget->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltTarget->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(upTime()));
	ui->qwtpltTarget->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltTarget->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

	// Templateプロット
	mTemplatePlotCurve = new QwtPlotCurve("Plot");
	mTemplatePlotCurve->setPen((QPen(Qt::yellow, 4)));
	ui->qwtpltTemplate->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltTemplate->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltTemplate->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(upTime()));
	ui->qwtpltTemplate->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltTemplate->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
	ui->qwtpltTemplate->replot();

	// BestCaseプロット
	mBestCasePlotCurve = new QwtPlotCurve("Plot");
	mBestCasePlotCurve->setPen((QPen(Qt::yellow, 4)));

	// WorstCaseプロット
	mWorstCasePlotCurve = new QwtPlotCurve("Plot");
	mWorstCasePlotCurve->setPen((QPen(Qt::yellow, 4)));

	// CenterCaseプロット
	mCenterCasePlotCurve = new QwtPlotCurve("Plot");
	mCenterCasePlotCurve->setPen((QPen(Qt::yellow, 4)));

	// テーブルリストのヘッダー可視化
	ui->tblIndexData->horizontalHeader()->setVisible(true);
	ui->tblTarget->horizontalHeader()->setVisible(true);
	ui->tblTemplate->horizontalHeader()->setVisible(true);
	ui->tblScore->horizontalHeader()->setVisible(true);
	ui->tblPrediction->horizontalHeader()->setVisible(true);

	// Config.iniの読み込み
	QSettings iniReader("Config.ini", QSettings::IniFormat);

	// DBセクション
	iniReader.beginGroup("DB");
	getLastDataDate = QDate::fromJulianDay(iniReader.value("GetLastDataDate", (int)QDate(1983,1,1).toJulianDay()).toInt());	// 最終DB更新日
	iniReader.endGroup();

	// 時系列DBを更新する
	int ret = 0;
	DlgAppStart *dlg = new DlgAppStart(this);
	dlg->SetData(getLastDataDate, QDate::currentDate());																	// DB更新ダイアログへのデータの受け渡し
	ret = dlg->exec();																										// ダイアログ表示
	if (ret == QDialog::Accepted) {																							// ダイアログ上での処理
	}
	delete dlg;
	dlg = NULL;

	// 銘柄リストを読み込む
	ReadIndexCSV("Config/IndexData.csv");
}

/**
 *	デストラクタ
 */
WndMain::~WndMain()
{
	// Config.iniへの書き込み
	QSettings iniWriter("Config.ini", QSettings::IniFormat);

	/*
	// DBセクション
	iniWriter.beginGroup("DB");
	//iniWriter.setValue("GetLastDataDate", QDate(1983,1,1).toJulianDay());													// 最終DB更新日
	iniWriter.setValue("GetLastDataDate", mLastUpdateDate.toJulianDay());													// 最終DB更新日
	iniWriter.endGroup();
	*/

	delete mTargetPlotCurve;
	delete mTemplatePlotCurve;
	delete mBestCasePlotCurve;
	delete mWorstCasePlotCurve;
	delete mCenterCasePlotCurve;
	delete mpSMAThread;
	delete mpEMAThread;
	delete mpRMAThread;
	delete mpWMAThread;
	delete mpChangeThread;
	delete mpTermHighThread;
	delete mpTermLowThread;
	delete mpIchimokuThread;
	delete mpBackTestThread;
	delete mpFibonacciThread;
	delete mpBollingerBandThread;
	delete mpSimulationThread;
	delete ui;
}

/**
 *	changeEvent()
 *
 *	@param[in] e 受け取ったイベント
 */
void WndMain::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

/**
 *	dragEnterEvent()
 *
 *	@param[in] e 受け取ったイベントのポインタ
 */
void WndMain::dragEnterEvent(QDragEnterEvent *e)
{
	e->mimeData()->hasFormat("text/uri-list");
	e->acceptProposedAction();
}

/**
 *	dropEvent()
 *
 *	@param[in] e 受け取ったイベントのポインタ
 */
void WndMain::dropEvent(QDropEvent *e)
{
	// dragEnterEventで受け取ったファイル情報の取得
	QList<QUrl> fileList = e->mimeData()->urls();
	QString filePath = QDir::toNativeSeparators(fileList[0].toLocalFile());
	QFileInfo fileInfo(filePath);
	QString dirName = fileInfo.dir().dirName();											// TargetDB or TemplateDB
	QString dirPath = fileInfo.dir().path();											// ~\TargetDB or ~\TemplateDB
	QString fileName = fileInfo.fileName();												// *.png
	QString rootPath = dirPath.remove(dirName);											// ~
	mRootPath = rootPath;

	// ディレクトリ名がTargetDBの場合
	if (fileInfo.dir().dirName().endsWith("TargetDB")) {
		// UI表示
		ui->ledtTargetFile->setText(filePath);
		ui->cmbTargetFile->addItems(fileInfo.dir().entryList(QDir::Files, QDir::Name));
		ui->cmbTargetFile->setCurrentIndex(ui->cmbTargetFile->findText(fileName));
		// 処理
		gpTarget->Restruct(rootPath);
	}
	// ディレクトリ名がTemplateDBの場合
	else if (fileInfo.dir().dirName().endsWith("TemplateDB")) {
		// UI表示
		ui->ledtTemplateFile->setText(filePath);
		ui->cmbTemplateFile->addItems(fileInfo.dir().entryList(QDir::Files, QDir::Name));
		ui->cmbTemplateFile->setCurrentIndex(ui->cmbTemplateFile->findText(fileName));
		// 処理
		gpTemplate->Restruct(rootPath);
	}
	// ディレクトリ名がscoreの場合
	else if (fileInfo.dir().dirName().endsWith("Score")) {
		mScoreDirName = dirName;
		// UI表示
		ReadScore(filePath);
	}
}

/**
 *	グラフ描画用日付データ変換関数
 */
QDate WndMain::upTime() const
{
	QDate t;
	for (int i = 1; i < (int)mTemplatePliceData.size(); i++) {
		t = mTemplatePliceData[i].mDate;
	}
	return t;
}

/**
 *	銘柄リストにデータを追加する関数
 */
void WndMain::AddIndexData(QTableWidget *pTableWidget, QString id, QString market, QString name, QString kind, QString open, QString high, QString low, QString close, QString volume, QString aClose, QString rate)
{
	QTableWidgetItem *newItem1	= new QTableWidgetItem(id);
	QTableWidgetItem *newItem2	= new QTableWidgetItem(market);
	QTableWidgetItem *newItem3	= new QTableWidgetItem(name);
	QTableWidgetItem *newItem4	= new QTableWidgetItem(kind);
	QTableWidgetItem *newItem5	= new QTableWidgetItem(open);
	QTableWidgetItem *newItem6	= new QTableWidgetItem(high);
	QTableWidgetItem *newItem7	= new QTableWidgetItem(low);
	QTableWidgetItem *newItem8	= new QTableWidgetItem(close);
	QTableWidgetItem *newItem9	= new QTableWidgetItem(volume);
	QTableWidgetItem *newItem10	= new QTableWidgetItem(aClose);
	QTableWidgetItem *newItem11	= new QTableWidgetItem(rate);
	pTableWidget->insertRow(pTableWidget->rowCount());
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 0, newItem1);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 1, newItem2);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 2, newItem3);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 3, newItem4);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 4, newItem5);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 5, newItem6);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 6, newItem7);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 7, newItem8);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 8, newItem9);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 9, newItem10);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 10, newItem11);
}

/**
 *	Targetリスト/Templateリストにデータを追加する関数
 */
void WndMain::AddPliceData(QTableWidget *pTableWidget, QString date, QString open, QString high, QString low, QString close, QString volume, QString aClose)
{
	QTableWidgetItem *newItem1 = new QTableWidgetItem(date);
	QTableWidgetItem *newItem2 = new QTableWidgetItem(open);
	QTableWidgetItem *newItem3 = new QTableWidgetItem(high);
	QTableWidgetItem *newItem4 = new QTableWidgetItem(low);
	QTableWidgetItem *newItem5 = new QTableWidgetItem(close);
	QTableWidgetItem *newItem6 = new QTableWidgetItem(volume);
	QTableWidgetItem *newItem7 = new QTableWidgetItem(aClose);
	pTableWidget->insertRow(pTableWidget->rowCount());
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 0, newItem1);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 1, newItem2);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 2, newItem3);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 3, newItem4);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 4, newItem5);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 5, newItem6);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 6, newItem7);
}

/**
 *	Scoreリストにデータを追加する関数
 */
void WndMain::AddScoreData(QTableWidget *pTableWidget, QString targetID, QString templateID, QString date, QString score)
{
	QTableWidgetItem *newItem1 = new QTableWidgetItem(targetID);
	QTableWidgetItem *newItem2 = new QTableWidgetItem(templateID);
	QTableWidgetItem *newItem3 = new QTableWidgetItem(date);
	QTableWidgetItem *newItem4 = new QTableWidgetItem(score);
	pTableWidget->insertRow(pTableWidget->rowCount());
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 0, newItem1);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 1, newItem2);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 2, newItem3);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 3, newItem4);
}

/**
 *	Predictionリストにデータを追加する関数
 */
void WndMain::AddPredictionData(QTableWidget *pTableWidget, QString day, QString bestCaseClose, QString worstCaseClose, QString centerCaseClose)
{
	QTableWidgetItem *newItem1 = new QTableWidgetItem(day);
	QTableWidgetItem *newItem2 = new QTableWidgetItem(bestCaseClose);
	QTableWidgetItem *newItem3 = new QTableWidgetItem(worstCaseClose);
	QTableWidgetItem *newItem4 = new QTableWidgetItem(centerCaseClose);
	pTableWidget->insertRow(pTableWidget->rowCount());
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 0, newItem1);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 1, newItem2);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 2, newItem3);
	pTableWidget->setItem(pTableWidget->rowCount() - 1, 3, newItem4);
}

/**
 *	IndexCSVファイルを読み込んでデータ配列にデータを格納する関数
 */
void WndMain::ReadIndexCSV(QString filePath)
{
	// ファイル読み込み
	QFile readCsvFile(filePath);
	if (!readCsvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

	// ファイル行読み込み用変数の宣言および配列の初期化
	QTextStream csvFileReader(&readCsvFile);
	QString csvFileLine;
	IndexData tempData;
	mIndexData.clear();

	// ファイル行を一行ずつ読み込みデータ配列にデータを格納する
	while (!csvFileReader.atEnd()) {
		csvFileLine = csvFileReader.readLine();
		tempData.mID			= csvFileLine.split(",").at(0);
		tempData.mMarket		= csvFileLine.split(",").at(1);
		tempData.mName			= csvFileLine.split(",").at(2);
		//tempData.mKind			= csvFileLine.split(",").at(3);
		if (csvFileLine.split(",").size() > 3) {
			tempData.mOpen		= csvFileLine.split(",").at(4).toDouble();
		}
		else {
			tempData.mOpen		= 0;
		}
		if (csvFileLine.split(",").size() > 4) {
			tempData.mHigh = csvFileLine.split(",").at(5).toDouble();
		}
		else {
			tempData.mHigh = 0;
		}
		if (csvFileLine.split(",").size() > 5) {
			tempData.mLow = csvFileLine.split(",").at(6).toDouble();
		}
		else {
			tempData.mLow = 0;
		}
		if (csvFileLine.split(",").size() > 6) {
			tempData.mClose = csvFileLine.split(",").at(7).toDouble();
		}
		else {
			tempData.mClose = 0;
		}
		if (csvFileLine.split(",").size() > 7) {
			tempData.mVolume = csvFileLine.split(",").at(8).toDouble();
		}
		else {
			tempData.mVolume = 0;
		}
		if (csvFileLine.split(",").size() > 8) {
			tempData.mAClose = csvFileLine.split(",").at(9).toDouble();
		}
		else {
			tempData.mAClose = 0;
		}
		if (csvFileLine.split(",").size() > 9) {
			tempData.mRate = csvFileLine.split(",").at(10).toDouble();
		}
		else {
			tempData.mRate = 0;
		}
		mIndexData.push_back(tempData);
	}

	QString temp1, temp2, temp3, temp4, temp5, temp6, temp7;
	for (int i = 0; i < (int)mIndexData.size(); i++) {
		AddIndexData(ui->tblIndexData, mIndexData[i].mID, mIndexData[i].mMarket, mIndexData[i].mName, mIndexData[i].mKind, temp1.setNum(mIndexData[i].mOpen), temp2.setNum(mIndexData[i].mHigh), temp3.setNum(mIndexData[i].mLow), temp4.setNum(mIndexData[i].mClose), temp5.setNum(mIndexData[i].mVolume), temp6.setNum(mIndexData[i].mAClose), temp7.setNum(mIndexData[i].mRate));
	}
}

/**
 *	TargetCSVファイルを読み込んでデータ配列にデータを格納する関数
 */
void WndMain::ReadTargetCSV(QString filePath, int fragmentLength)
{
	// ファイル読み込み
	QFile readCsvFile(filePath);
	if (!readCsvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

	// ファイル行読み込み用変数の宣言および配列の初期化
	QTextStream csvFileReader(&readCsvFile);
	QString csvFileLine;
	PliceData tempData;
	mTargetPliceData.clear();

	// ファイル行を一行ずつ読み込みデータ配列にデータを格納する
	while (!csvFileReader.atEnd()) {
		csvFileLine				= csvFileReader.readLine();
		tempData.mDate			= QDate::fromString((csvFileLine.split(",").at(0)), "yyyy/M/d");
		tempData.mDateJulian	= QDate::fromString((csvFileLine.split(",").at(0)), "yyyy/M/d").toJulianDay();
		tempData.mOpen			= csvFileLine.split(",").at(1).toDouble();
		tempData.mHigh			= csvFileLine.split(",").at(2).toDouble();
		tempData.mLow			= csvFileLine.split(",").at(3).toDouble();
		tempData.mClose			= csvFileLine.split(",").at(4).toDouble();
		tempData.mVolume		= csvFileLine.split(",").at(5).toDouble();
		tempData.mAClose		= csvFileLine.split(",").at(6).toDouble();
		mTargetPliceData.push_back(tempData);
	}

	QString temp1, temp2, temp3, temp4, temp5, temp6;
	for (int i = (int)mTargetPliceData.size() - fragmentLength; i < (int)mTargetPliceData.size(); i++) {
		AddPliceData(ui->tblTarget, mTargetPliceData[i].mDate.toString("yyyy/M/d"), temp1.setNum(mTargetPliceData[i].mOpen), temp2.setNum(mTargetPliceData[i].mHigh), temp3.setNum(mTargetPliceData[i].mLow), temp4.setNum(mTargetPliceData[i].mClose), temp5.setNum(mTargetPliceData[i].mVolume), temp6.setNum(mTargetPliceData[i].mAClose));
	}
}

/**
 *	TemplateCSVファイルを読み込んでデータ配列にデータを格納する関数
 */
void WndMain::ReadTemplateCSV(QString filePath, int pltLength, QDate date)
{
	// 変数宣言
	int startDate;

	// ファイル読み込み
	QFile readCsvFile(filePath);
	if (!readCsvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

	// ファイル行読み込み用変数の宣言および配列の初期化
	QTextStream csvFileReader(&readCsvFile);
	QString csvFileLine;
	PliceData tempData;
	mTemplatePliceData.clear();

	// ファイル行を一行ずつ読み込みデータ配列にデータを格納する
	while (!csvFileReader.atEnd()) {
		csvFileLine				= csvFileReader.readLine();
		tempData.mDate			= QDate::fromString((csvFileLine.split(",").at(0)), "yyyy/M/d");
		tempData.mDateJulian	= QDate::fromString((csvFileLine.split(",").at(0)), "yyyy/M/d").toJulianDay();
		tempData.mOpen			= csvFileLine.split(",").at(1).toDouble();
		tempData.mHigh			= csvFileLine.split(",").at(2).toDouble();
		tempData.mLow			= csvFileLine.split(",").at(3).toDouble();
		tempData.mClose			= csvFileLine.split(",").at(4).toDouble();
		tempData.mVolume		= csvFileLine.split(",").at(5).toDouble();
		tempData.mAClose		= csvFileLine.split(",").at(6).toDouble();
		mTemplatePliceData.push_back(tempData);
	}

	// 日付が一致するデータを特定
	for (int i = 1; i < (int)mTemplatePliceData.size(); i++) {
		if (date == mTemplatePliceData[i].mDate) {
			startDate = i;
		}
	}

	QString temp1, temp2, temp3, temp4, temp5, temp6;
	for (int i = startDate; i < startDate + pltLength; i++) {
		AddPliceData(ui->tblTemplate, mTemplatePliceData[i].mDate.toString("yyyy/M/d"), temp1.setNum(mTemplatePliceData[i].mOpen), temp2.setNum(mTemplatePliceData[i].mHigh), temp3.setNum(mTemplatePliceData[i].mLow), temp4.setNum(mTemplatePliceData[i].mClose), temp5.setNum(mTemplatePliceData[i].mVolume), temp6.setNum(mTemplatePliceData[i].mClose));
	}
}

/**
 *	Scoreファイルを読み込んでデータ配列にデータを格納する関数
 */
void WndMain::ReadScore(QString filePath)
{
	// ファイル読み込み
	QFile readScoreFile(filePath);
	if (!readScoreFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

	// ファイル行読み込み用変数の宣言および配列の初期化
	QTextStream scoreFileReader(&readScoreFile);
	QString scoreFileLine;
	ScoreData tempData;
	mScoreData.clear();

	// ファイル行を一行ずつ読み込みデータ配列にデータを格納する
	while (!scoreFileReader.atEnd()) {
		scoreFileLine			= scoreFileReader.readLine();
		tempData.mTargetID		= scoreFileLine.split(",").at(0).toInt();
		tempData.mTemplateID	= scoreFileLine.split(",").at(1).toInt();
		tempData.mDate			= QDate::fromString((scoreFileLine.split(",").at(2)), "yyyy/M/d");
		tempData.mScore			= scoreFileLine.split(",").at(3).split("=").at(1).toDouble();
		mScoreData.push_back(tempData);
	}

	QString temp1, temp2, temp3;
	for (int i = 0; i < (int)mScoreData.size(); i++) {
		AddScoreData(ui->tblScore, temp1.setNum(mScoreData[i].mTargetID), temp2.setNum(mScoreData[i].mTemplateID), mScoreData[i].mDate.toString("yyyy/MM/dd"), temp3.setNum(mScoreData[i].mScore));
	}
}

/**
 *	Prediction後のテーブル反映関数
 */
void WndMain::PredictionTableMapping()
{
	QString temp1, temp2, temp3, temp4;
	for (int i = (int)gpPredict->mBestCasePliceData.size() - 1 - ui->spnPredictionLength->value(); i < (int)gpPredict->mBestCasePliceData.size(); i++) {
		AddPredictionData(ui->tblPrediction, temp1.setNum(i + 1), temp2.setNum(gpPredict->mBestCasePliceData[i].mClose), temp3.setNum(gpPredict->mWorstCasePliceData[i].mClose), temp4.setNum(gpPredict->mCenterCasePliceData[i].mClose));
	}
}

/**
 *	Targetデータグラフ描画関数
 */
void WndMain::DrawTargetCSVPlot(int fragmentLength, int start)
{
	// グラフ描画用データ配列宣言と初期化
	vector <double> dataJulian;
	vector <double> dataClose;
	dataJulian.clear();
	dataClose.clear();

	// グラフ描画用にPliceData配列の各データを読み込む（一番上の行はラベルだから読み込まない）
	for (int i = 1; i < (int)mTargetPliceData.size(); i++) {
		dataJulian.push_back(mTargetPliceData[i].mDateJulian);			// ユリウス暦の読み込み
		dataClose.push_back(mTargetPliceData[i].mClose);				// 終値の読み込み
	}

	// グラフ描画
	mTargetPlotCurve->setSamples(&dataJulian[dataJulian.size()-fragmentLength-start], &dataClose[dataJulian.size()-fragmentLength-start], fragmentLength);
	mTargetPlotCurve->attach(ui->qwtpltTarget);

	// グラフエリアの設定
	ui->qwtpltTarget->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltTarget->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltTarget->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(upTime()));
	ui->qwtpltTarget->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltTarget->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
	ui->qwtpltTarget->replot();
}

/**
 *	Templateデータグラフ描画関数
 */
void WndMain::DrawTemplateCSVPlot(int pltLength, QDate date)
{
	// グラフ描画用データ配列宣言と初期化
	vector <double> dataJulian;
	vector <double> dataClose;
	dataJulian.clear();
	dataClose.clear();

	// 変数宣言
	int startDate;

	// グラフ描画用にPliceData配列の各データを読み込む（一番上の行はラベルだから読み込まない）
	for (int i = 1; i < (int)mTemplatePliceData.size(); i++) {
		dataJulian.push_back(mTemplatePliceData[i].mDateJulian);		// ユリウス暦の読み込み
		dataClose.push_back(mTemplatePliceData[i].mClose);				// 終値の読み込み
	}

	// 日付が一致するデータを特定
	for (int i = 1; i < (int)mTemplatePliceData.size(); i++) {
		if (date == mTemplatePliceData[i].mDate) {
			startDate = i;
		}
	}

	// グラフ描画
	mTemplatePlotCurve->setSamples(&dataJulian[startDate-1], &dataClose[startDate-1], pltLength);
	mTemplatePlotCurve->attach(ui->qwtpltTemplate);

	// グラフエリアの設定
	ui->qwtpltTemplate->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltTemplate->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltTemplate->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(upTime()));
	ui->qwtpltTemplate->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltTemplate->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
	ui->qwtpltTemplate->replot();
}

/**
 *	BestCaseデータグラフ描画関数
 */
void WndMain::DrawBestCasePlot()
{
	// グラフ描画用データ配列宣言と初期化
	vector <double> dataJulian;
	vector <double> dataClose;
	dataJulian.clear();
	dataClose.clear();

	// グラフ描画用にPliceData配列の各データを読み込む（一番上の行はラベルだから読み込まない）
	for (int i = 0; i < (int)gpPredict->mBestCasePliceData.size(); i++) {
		dataJulian.push_back(gpPredict->mBestCasePliceData[i].mDateJulian);		// ユリウス暦の読み込み
		dataClose.push_back(gpPredict->mBestCasePliceData[i].mClose);			// 終値の読み込み
	}

	// グラフ描画
	mBestCasePlotCurve->setSamples(&dataJulian[0], &dataClose[0], gpPredict->mBestCasePliceData.size());
	mBestCasePlotCurve->attach(ui->qwtpltBestCase);

	// グラフエリアの設定
	ui->qwtpltBestCase->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltBestCase->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltBestCase->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(upTime()));
	ui->qwtpltBestCase->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltBestCase->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
	ui->qwtpltBestCase->replot();
}

/**
 *	WorstCaseデータグラフ描画関数
 */
void WndMain::DrawWorstCasePlot()
{
	// グラフ描画用データ配列宣言と初期化
	vector <double> dataJulian;
	vector <double> dataClose;
	dataJulian.clear();
	dataClose.clear();

	// グラフ描画用にPliceData配列の各データを読み込む（一番上の行はラベルだから読み込まない）
	for (int i = 0; i < (int)gpPredict->mWorstCasePliceData.size(); i++) {
		dataJulian.push_back(gpPredict->mWorstCasePliceData[i].mDateJulian);	// ユリウス暦の読み込み
		dataClose.push_back(gpPredict->mWorstCasePliceData[i].mClose);			// 終値の読み込み
	}

	// グラフ描画
	mWorstCasePlotCurve->setSamples(&dataJulian[0], &dataClose[0], gpPredict->mWorstCasePliceData.size());
	mWorstCasePlotCurve->attach(ui->qwtpltWorstCase);

	// グラフエリアの設定
	ui->qwtpltWorstCase->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltWorstCase->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltWorstCase->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(upTime()));
	ui->qwtpltWorstCase->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltWorstCase->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
	ui->qwtpltWorstCase->replot();
}

/**
 *	CenterCaseデータグラフ描画関数
 */
void WndMain::DrawCenterCasePlot()
{
	// グラフ描画用データ配列宣言と初期化
	vector <double> dataJulian;
	vector <double> dataClose;
	dataJulian.clear();
	dataClose.clear();

	// グラフ描画用にPliceData配列の各データを読み込む（一番上の行はラベルだから読み込まない）
	for (int i = 0; i < (int)gpPredict->mCenterCasePliceData.size(); i++) {
		dataJulian.push_back(gpPredict->mCenterCasePliceData[i].mDateJulian);	// ユリウス暦の読み込み
		dataClose.push_back(gpPredict->mCenterCasePliceData[i].mClose);			// 終値の読み込み
	}

	// グラフ描画
	mCenterCasePlotCurve->setSamples(&dataJulian[0], &dataClose[0], gpPredict->mCenterCasePliceData.size());
	mCenterCasePlotCurve->attach(ui->qwtpltCenterCase);

	// グラフエリアの設定
	ui->qwtpltCenterCase->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltCenterCase->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltCenterCase->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(upTime()));
	ui->qwtpltCenterCase->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltCenterCase->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
	ui->qwtpltCenterCase->replot();
}

/**
 *	[Open]ボタンのclicked()シグナルを受け取るスロット
 */
void WndMain::on_mnuOpen_triggered()
{
	// ファイルダイアログ
	QFileInfo fileInfo(QFileDialog::getOpenFileName(this,
													tr("Select *.csv"),
													"C:\\",
													tr("LaserUtil data(*.csv)"), 0, 0));
	msOpenFile = fileInfo;
}

/**
 *	[FA]->[Template]->[Restruct]メニューのtriggered()シグナルを受け取るスロット
 */
void WndMain::on_mnuTemplateRestruct_triggered()
{
	gpTemplate->Restruct(mRootPath);
}

/**
 *	[FA]->[Target]->[Restruct]メニューのtriggered()シグナルを受け取るスロット
 */
void WndMain::on_mnuTargetRestruct_triggered()
{
	gpTarget->Restruct(mRootPath);
}

/**
 *	[FA]->[Predict]->[CalculateScore]メニューのtriggered()シグナルを受け取るスロット
 */
void WndMain::on_mnuCalculateScore_triggered()
{
	for (int i = 0; i < ui->spnTestTerm->value(); i++) {
		gpPredict->CalculateScore(mRootPath, ui->spnFragmentLength->value(), ui->spnScoreFiltering->value(), ui->spnMinS->value(), i);
		gpPredict->Prediction(mRootPath, 998407, ui->spnFragmentLength->value(), ui->spnPredictionLength->value(), i);
	}
}

/**
 *	[ScoreCalculation]ボタンのclicked()シグナルを受け取るスロット
 */
void WndMain::on_btnCalculation_clicked()
{
	for (int i = 0; i < ui->spnTestTerm->value(); i++) {
		gpPredict->CalculateScore(mRootPath, ui->spnFragmentLength->value(), ui->spnScoreFiltering->value(), ui->spnMinS->value(), i);
		for (int j = 1000; j < 1000000; j++) {
			if (((j > 10000) && !((j == 998405) || (j == 998407)))) {
				continue;
			}
			gpPredict->Prediction(mRootPath, j, ui->spnFragmentLength->value(), ui->spnPredictionLength->value(), i);
		}
		gpPredict->Screening(mRootPath, i);
	}
}

/**
 *	[DrawPlot]ボタンのclicked()シグナルを受け取るスロット
 */
void WndMain::on_btnDrawPlot_clicked()
{
	// 変数宣言
	QString targetID;
	QString templateID;
	QDate templateDate;
	QString targetFilePath;
	QString templateFilePath;

	// テーブルリストをリセットする
	ui->tblTarget->setRowCount(0);
	ui->tblTemplate->setRowCount(0);
	ui->tblTarget->clear();
	ui->tblTemplate->clear();

	// 選択行からTargetファイル名取得
	targetID = ui->tblScore->item(ui->tblScore->currentRow(), 0)->text();
	targetFilePath = mRootPath + "TargetDB\\" + targetID + ".csv";

	// 選択行からTemplateファイル名取得
	templateID = ui->tblScore->item(ui->tblScore->currentRow(), 1)->text();
	templateFilePath = mRootPath + "TemplateDB\\" + templateID + ".csv";

	// 選択行からTemplateのDate取得
	templateDate = QDate::fromString((ui->tblScore->item(ui->tblScore->currentRow(), 2)->text()), "yyyy/M/d");

	// CSV読み込み
	ReadTargetCSV(targetFilePath, ui->spnFragmentLength->value());
	ReadTemplateCSV(templateFilePath, ui->spnTemplateLength->value(), templateDate);

	// グラフ描画(読み込み中のScoreFileのディレクトリ名からstartを取得)
	DrawTargetCSVPlot(ui->spnFragmentLength->value(), mScoreDirName.split("_").at(0).toInt());
	DrawTemplateCSVPlot(ui->spnTemplateLength->value(), templateDate);

	// UI表示
	ui->ledtTargetFile->setText(targetFilePath);
	ui->ledtTemplateFile->setText(templateFilePath);
}

/**
 *	[Prediction]ボタンのclicked()シグナルを受け取るスロット
 */
void WndMain::on_btnPrediction_clicked()
{
	// 変数宣言
	QString targetID;

	// 選択行からTargetID取得
	targetID = ui->tblScore->item(ui->tblScore->currentRow(), 0)->text();

	// 実行(読み込み中のScoreFileのディレクトリ名からstartを取得)
	gpPredict->Prediction(mRootPath, targetID.toInt(), ui->spnFragmentLength->value(), ui->spnPredictionLength->value(), mScoreDirName.split("_").at(0).toInt());

	// グラフ描画
	DrawBestCasePlot();
	DrawWorstCasePlot();
	DrawCenterCasePlot();

	// UI表示
	PredictionTableMapping();
}

/**
 *	銘柄リスト上でダブルクリックするとチャートダイアログ表示
 */
void WndMain::on_tblIndexData_itemDoubleClicked(QTableWidgetItem *item)
{
	int ret = 0;
	DlgChart *dlg = new DlgChart(this);

	// 選択銘柄の時系列データを読み込み、チャート描画する
	dlg->ReadPliceCSV("Yahoo/" + ui->tblIndexData->item(item->row(), 0)->text() + ".csv");
	dlg->DrawPliceCSVPlot();

	// ダイアログ表示
	ret = dlg->exec();

	// ダイアログ上での処理
	if (ret == QDialog::Accepted) {
	}

	delete dlg;
	dlg = NULL;
}

void WndMain::on_btnDB_clicked()
{
	// パラメータ読み込み
	mpSMAThread->ReadParam(ui->spnMA->value());
	mpEMAThread->ReadParam(ui->spnMA->value());
	mpRMAThread->ReadParam(ui->spnMA->value());
	mpWMAThread->ReadParam(ui->spnMA->value());
	mpChangeThread->ReadParam();
	mpTermHighThread->ReadParam(ui->spnTerm->value());
	mpTermLowThread->ReadParam(ui->spnTerm->value());
	mpIchimokuThread->ReadParam();
	mpBollingerBandThread->ReadParam(20);
	mpFibonacciThread->ReadParam(100);

	// 実行
	mpSMAThread->start();
	mpEMAThread->start();
	mpRMAThread->start();
	mpWMAThread->start();
	mpChangeThread->start();
	mpTermHighThread->start();
	mpTermLowThread->start();
	mpIchimokuThread->start();
	mpBollingerBandThread->start();
	mpFibonacciThread->start();
}

void WndMain::on_btnBackTest_clicked()
{
	mpBackTestThread->ReadParam(5, 1000000, 5, 25);
	mpBackTestThread->start();
}

void WndMain::on_btnSimulation_clicked()
{
	mpSimulationThread->ReadParam(5, 1000000, ui->dedtBackTestStart->date(), ui->dedtBackTestEnd->date());
	mpSimulationThread->start();
}
