#include "DlgChart.h"
#include "ui_DlgChart.h"

/**
 *	コンストラクタ
 */
DlgChart::DlgChart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgChart)
{
    ui->setupUi(this);

	// BestCaseプロット
	mPlicePlotCurve = new QwtPlotCurve("Plot");
	mPlicePlotCurve->setPen((QPen(Qt::yellow, 1)));

	// UI初期化
	ui->tblChartData->horizontalHeader()->setVisible(true);
}

/**
 *	デストラクタ
 */
DlgChart::~DlgChart()
{
	delete mPlicePlotCurve;
    delete ui;
}

/**
 *	changeEvent()
 *
 *	@param[in] e 受け取ったイベント
 */
void DlgChart::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
 *	時系列CSVファイルを読み込んでデータ配列にデータを格納する関数
 */
void DlgChart::ReadPliceCSV(QString filePath)
{
	// 変数宣言
	//int startDate;

	// ファイル読み込み
	QFile readCsvFile(filePath);
	if (!readCsvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

	// ファイル行読み込み用変数の宣言および配列の初期化
	QTextStream csvFileReader(&readCsvFile);
	QString csvFileLine;
	PliceData tempData;
	mPliceData.clear();

	// ファイル行を一行ずつ読み込みデータ配列にデータを格納する
	while (!csvFileReader.atEnd()) {
		csvFileLine				= csvFileReader.readLine();
		if (csvFileLine != "") {
			tempData.mDate			= QDate::fromString((csvFileLine.split(",").at(0)), "yyyy/M/d");
			tempData.mDateJulian	= QDate::fromString((csvFileLine.split(",").at(0)), "yyyy/M/d").toJulianDay();
			tempData.mOpen			= csvFileLine.split(",").at(1).toDouble();
			tempData.mHigh			= csvFileLine.split(",").at(2).toDouble();
			tempData.mLow			= csvFileLine.split(",").at(3).toDouble();
			tempData.mClose			= csvFileLine.split(",").at(4).toDouble();
			tempData.mVolume		= csvFileLine.split(",").at(5).toDouble();
			tempData.mAClose		= csvFileLine.split(",").at(6).toDouble();
			mPliceData.push_back(tempData);
		}
	}

	QString temp1, temp2, temp3, temp4, temp5, temp6;
	for (int i = 0; i < (int)mPliceData.size(); i++) {
		gpWndMain->AddPliceData(ui->tblChartData, mPliceData[i].mDate.toString("yyyy/M/d"), temp1.setNum(mPliceData[i].mOpen), temp2.setNum(mPliceData[i].mHigh), temp3.setNum(mPliceData[i].mLow), temp4.setNum(mPliceData[i].mClose), temp5.setNum(mPliceData[i].mVolume), temp6.setNum(mPliceData[i].mAClose));
	}
}

/**
 *	時系列データグラフ描画関数
 */
void DlgChart::DrawPliceCSVPlot()
{
	if (mPliceData.size() == 0) {
		return;
	}

	// グラフ描画用データ配列宣言と初期化
	vector <double> dataJulian;
	vector <double> dataClose;
	dataJulian.clear();
	dataClose.clear();

	// 変数宣言
	//int startDate;

	// グラフ描画用にPliceData配列の各データを読み込む（一番上の行はラベルだから読み込まない）
	for (int i = 0; i < (int)mPliceData.size(); i++) {
		dataJulian.push_back(mPliceData[i].mDateJulian);			// ユリウス暦の読み込み
		dataClose.push_back(mPliceData[i].mClose);					// 終値の読み込み
	}

	// グラフ描画
	mPlicePlotCurve->setSamples(&dataJulian[0], &dataClose[0], mPliceData.size());
	mPlicePlotCurve->attach(ui->qwtpltChart);

	// グラフエリアの設定
	ui->qwtpltChart->setAxisTitle(QwtPlot::xBottom, "Date");
	ui->qwtpltChart->setAxisTitle(QwtPlot::yLeft, "Close");
	ui->qwtpltChart->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(gpWndMain->upTime()));
	ui->qwtpltChart->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	ui->qwtpltChart->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
	ui->qwtpltChart->replot();
}
