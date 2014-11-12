#include "DlgAppStart.h"
#include "ui_DlgAppStart.h"
#include <algorithm>

/**
 *	コンストラクタ
 */
DlgAppStart::DlgAppStart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAppStart)
{
    ui->setupUi(this);

	connect(&mNetManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
}

/**
 *	デストラクタ
 */
DlgAppStart::~DlgAppStart()
{
	delete ui;
}

/**
 *	closeEvent()
 *
 *	@param[in] e 受け取ったイベント
 */
void DlgAppStart::closeEvent(QCloseEvent *)
{
	// Config.iniへの書き込み
	QSettings iniWriter("Config.ini", QSettings::IniFormat);

	// DBセクション
	iniWriter.beginGroup("DB");
	iniWriter.setValue("GetLastDataDate", ui->dedtLastDBUpdateDate->date().toJulianDay());									// 最終DB更新日
	iniWriter.endGroup();
}

/**
 *	データのセット
 *
 *	@param[in] lastDBUpdateDate						最終更新日
 *	@param[in] latestDBUpdateDate					最新更新可能日
 */
void DlgAppStart::SetData(QDate lastDBUpdateDate, QDate latestDBUpdateDate)
{
	// UIへのデータ反映
	ui->dedtLastDBUpdateDate->setDate(lastDBUpdateDate);				// 最終更新日
	ui->dedtLatestDBUpdateDate->setDate(latestDBUpdateDate);			// 最新更新可能日
}

/**
 *	DBの更新
 *
 *	@param[in] startDate							データ取得開始日
 *	@param[in] endDate								データ取得終了日
 */
void DlgAppStart::UpdateDB(int id, QDate startDate, QDate endDate)
{
	// URL生成
	mURL =	"http://info.finance.yahoo.co.jp/history/";				// Yahooファイナンスの時系列ページ
	mURL +=	"?code=" + mTemp1.setNum(id);							// 証券コード
	mURL +=	"&sy=" + mTemp1.setNum(startDate.year());				// 開始年
	mURL +=	"&sm=" + mTemp1.setNum(startDate.month());				// 開始月
	mURL +=	"&sd=" + mTemp1.setNum(startDate.day());				// 開始日
	mURL +=	"&ey=" + mTemp1.setNum(endDate.year());					// 終了年
	mURL +=	"&em=" + mTemp1.setNum(endDate.month());				// 終了月
	mURL +=	"&ed=" + mTemp1.setNum(endDate.day());					// 終了日
	mURL += "&p=" + mTemp1.setNum(mPage);							// ページナンバー

	// アクセス
	mNetManager.get(QNetworkRequest(QUrl(mURL)));
}

/**
 *	DBの整形
 */
void DlgAppStart::FormatDB()
{
	// 変数宣言
	QString readFileName;			// 読み込みファイル名
	QFile readFile;					// 読み込みファイル
	QString readFileLine;			// 読み込みファイル行
	QString writeFileName;			// 書き込みファイル名
	QFile writeFile;				// 書き込みファイル
	QString temp_1;					// 一時保存用文字列
	PliceData tempData;				// 時系列一時データ
	vector <PliceData> pliceData;	// 時系列データ配列

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

		if (!readFile.exists()) {
			continue;
		}

		if (readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			QTextStream readFileReader(&readFile);
			readFileLine = "";

			// 時系列データの読み取り
			while (!readFileReader.atEnd()) {
				readFileLine = readFileReader.readLine();

				// TOPIXと日経平均のときはデータ数は5個または7個
				if ((i == 998405) || (i == 998407)) {
					if (readFileLine.split(",").size() == 5) {
						// データ取得
						tempData.mDate		= QDate::fromString((readFileLine.split(",").at(0)), "yyyy/M/d");
						tempData.mOpen		= readFileLine.split(",").at(1).toDouble();
						tempData.mHigh		= readFileLine.split(",").at(2).toDouble();
						tempData.mLow		= readFileLine.split(",").at(3).toDouble();
						tempData.mClose		= readFileLine.split(",").at(4).toDouble();
						tempData.mVolume	= 0;
						tempData.mAClose	= readFileLine.split(",").at(4).toDouble();
						pliceData.push_back(tempData);
					}
					else if (readFileLine.split(",").size() == 7) {
						// データ取得
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
				else {
					// それ以外はデータ数は7個。分割等がなく正常データのみ抽出
					if (readFileLine.split(",").size() == 7) {
						// データ取得
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
			}
		}

		readFile.close();

		// 日付順にソート
		sort(pliceData.begin(), pliceData.end(), compareDate<PliceData>());

		// 同一データの削除
		for (int j = 0; j < (int)pliceData.size(); j++) {
			if (j == 0) {
				continue;
			}
			// 現行行と直前行の日付が同じ場合は現行行を削除
			if (pliceData[j].mDate == pliceData[j-1].mDate) {
				for (int k = j; k < (int)pliceData.size() - 1; k++) {
					pliceData[k] = pliceData[k+1];
				}
				pliceData.pop_back();
				j--;
			}
		}

		// 書き込み用ファイルオープン
		writeFileName = readFileName;
		writeFile.setFileName(writeFileName);
		if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		// 書き込み処理
		for (int j = 0; j < (int)pliceData.size(); j++) {
			QTextStream out(&writeFile);

			// 整形後データの書き込み
			out << (pliceData[j].mDate.toString("yyyy/M/d")) << ","
				<< (pliceData[j].mOpen) << ","
				<< (pliceData[j].mHigh) << ","
				<< (pliceData[j].mLow) << ","
				<< (pliceData[j].mClose) << ","
				<< (pliceData[j].mVolume) << ","
				<< (pliceData[j].mAClose) << "\n";
		}

		writeFile.close();
	}
}

/**
 *	HTTPデータ受信完了時のイベント
 *
 *	@param[in] reply								reply
 */
void DlgAppStart::replyFinished(QNetworkReply *reply)
{
	if (reply->error() != QNetworkReply::NoError) {
		// エラーメッセージ
		QMessageBox::warning(this, tr("Error"),
							 mTemp1.setNum(mID),
							 QMessageBox::Ok
							 );

		// 保存用ファイルを閉じる
		mFile.close();

		// リトライ
		mPage = 1;
		UpdateDB(mID, ui->dedtLastDBUpdateDate->date(), ui->dedtLatestDBUpdateDate->date());

		return;
	}
	else {
		// 取得データ
		QString str = QString::fromUtf8(reply->readAll().data());

		// 取得データを整える
		QTextCodec* tc = QTextCodec::codecForLocale();
		str.replace("</table>", "\n");									// table行の終わりがデータの終わりなので、改行して以降のデータを読まなくする
		str.replace(",", "");											// カンマをとる
		str.replace(QRegExp("\\s+"), "");								// スペースをとる
		str.replace("class=\"stocksPriceRed\"", "");					// 削除
		str.replace("class=\"stocksPriceGreen\"", "");					// 削除
		str.replace("</td><td>", ",");									// 各データ値の区切り文字をカンマにする
		str.replace(tc->toUnicode("年"), "/");							// 年を/にする
		str.replace(tc->toUnicode("月"), "/");							// 月を/にする
		str.replace(tc->toUnicode("日"), "");							// 日を省く
		QStringList row = str.split("<tr><td>");

		// データ出力
		if (row.size() > 1) {
			if (mPage == 1) {
				// ファイルが既に存在している場合はAppendモードで開く
				if (mFile.exists()) {
					// ファイルを開く
					mFile.open(QIODevice::Append);
				}
				// ファイルがない場合は新規作成する
				else {
					// ファイルを開く
					mFile.open(QIODevice::WriteOnly);
				}
			}

			QTextStream out(&mFile);

			for (int i = 1; i < row.size(); i++) {
				out << row.at(i).split("</td></tr>").at(0) << "\n";
			}

			// 次のページを読み込む
			mPage++;
			UpdateDB(mID, ui->dedtLastDBUpdateDate->date(), ui->dedtLatestDBUpdateDate->date());
		}
		else {
			// 保存用ファイルを閉じる
			mFile.close();

			// 次のIDを読み込む
			mID++;
			mPage = 1;
			if (mID == 10000) {
				mID = 998405;
				// 次の出力ファイル名
				mFile.setFileName("Yahoo\\" + mTemp1.setNum(mID) + ".csv");

				// 次のIDを読み込む
				UpdateDB(mID, ui->dedtLastDBUpdateDate->date(), ui->dedtLatestDBUpdateDate->date());
			}
			else if (mID == 998406) {
				mID = 998407;
				// 次の出力ファイル名
				mFile.setFileName("Yahoo\\" + mTemp1.setNum(mID) + ".csv");

				// 次のIDを読み込む
				UpdateDB(mID, ui->dedtLastDBUpdateDate->date(), ui->dedtLatestDBUpdateDate->date());
			}
			else if (mID == 998408) {
				// 全データ取得完了したらEndDate-5(取り忘れ防止)をStartDateに代入する
				ui->dedtLastDBUpdateDate->setDate(QDate::fromJulianDay(ui->dedtLatestDBUpdateDate->date().toJulianDay() - 5));
				ui->btnUpdateDB->setEnabled(true);
				// 全データ整形する
				FormatDB();
			}
			else if (mID < 10000) {
				// 次の出力ファイル名
				mFile.setFileName("Yahoo\\" + mTemp1.setNum(mID) + ".csv");

				// 次のIDを読み込む
				UpdateDB(mID, ui->dedtLastDBUpdateDate->date(), ui->dedtLatestDBUpdateDate->date());
			}
		}
	}
}

/**
 *	[UpdateDB]ボタンのclicked()シグナルを受け取るスロット
 */
void DlgAppStart::on_btnUpdateDB_clicked()
{
	// データ取得開始
	mID = 1300;
	mPage = 1;
	ui->btnUpdateDB->setEnabled(false);
	UpdateDB(mID, ui->dedtLastDBUpdateDate->date(), ui->dedtLatestDBUpdateDate->date());
}

/**
 *	[Close]ボタンのclicked()シグナルを受け取るスロット
 */
void DlgAppStart::on_btnUpdateClose_clicked()
{
	this->close();
}
