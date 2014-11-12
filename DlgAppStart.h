#ifndef DLGAPPSTART_H
#define DLGAPPSTART_H

#include <QDialog>
#include <QtGui>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "WndMain.h"

using namespace std;

namespace Ui {
    class DlgAppStart;
}

class DlgAppStart : public QDialog
{
	Q_OBJECT
public:
	explicit					DlgAppStart(QWidget *parent = 0);													// コンストラクタ
								~DlgAppStart();																		// デストラクタ
	void						SetData(QDate lastDBUpdateDate, QDate latestDBUpdateDate);							// データのセット

protected:
	void						closeEvent(QCloseEvent *);															// ダイアログを閉じるときの処理
	void						UpdateDB(int id, QDate startDate, QDate endDate);									// DBの更新
	void						FormatDB();																			// DBの整形

private slots:
	void						replyFinished(QNetworkReply *);														// HTTPデータ受信完了時のイベント
	void						on_btnUpdateDB_clicked();															// [UpdateDB]ボタンのclicked()シグナルを受け取るスロット
	void						on_btnUpdateClose_clicked();														// [Close]ボタンのclicked()シグナルを受け取るスロット

private:
	Ui::DlgAppStart				*ui;
	QNetworkAccessManager		mNetManager;
	QFile						mFile;
	QString						mURL;
	int							mID;
	int							mPage;
	QString						mTemp1;
	QString						mTemp2;
};

// PliceDataの日付比較関数(昇順)
template <class PliceData> class compareDate:
		public binary_function<PliceData, PliceData, bool>
{
public:
	bool operator()(PliceData a, PliceData b)
	{
		return a.mDate.toJulianDay() < b.mDate.toJulianDay();
	}
};

#endif // DLGAPPSTART_H
