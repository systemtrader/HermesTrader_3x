#include "Template.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <direct.h>

/**
 *	コンストラクタ
 */
Template::Template()
{
}

/**
 *	TemplateRateファイルの作成
 */
void Template::Restruct(QString rootPath)
{
	// 変数宣言
	QString csvFileName;			// CSVファイル名
	QString rateFileName;			// rateファイル名
	QString readLine;				// textリーダー

	// rateディレクトリ生成
	_mkdir(rootPath.toLocal8Bit() + "TemplateRate\\");

	for (int i = 1000; i < 1000000; i++) {
		if (((i > 10000) && !((i == 998405) || (i == 998407)))) {
			continue;
		}

		// 読み込み用ファイルオープン
		csvFileName = rootPath + "\\TemplateDB\\" + mTemp1.setNum(i) + ".csv";
		QFile csvFile(csvFileName);
		if (csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			// 宣言&初期化
			QTextStream readCsv(&csvFile);
			readLine = "";
			mDate.clear();
			mClose.clear();

			// csvファイルの読み取り
			while (!readCsv.atEnd()) {
				readLine = readCsv.readLine();
				// 各カラムのデータ取得
				mDate.push_back(QDate::fromString((readLine.split(",").at(0)), "M/d/yyyy"));
				mClose.push_back(readLine.split(",").at(4).toDouble());
			}

			// 書き込み用ファイルオープン
			rateFileName = rootPath + "TemplateRate\\" + mTemp1.setNum(i) + ".rate";
			QFile RateFile(rateFileName);
			if (!RateFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				return;
			}

			// 書き込み処理
			for (int i = 2; i < (int)mClose.size(); i++) {
				QTextStream out(&RateFile);
				// 騰落率計算結果の書き込み
				out << (mDate[i].toString("yyyy/MM/dd")) << "," << ((mClose[i] - mClose[i-1])/mClose[i-1]) << "\n";
			}

			RateFile.close();
		}

		csvFile.close();
	}
}
