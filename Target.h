#ifndef TARGET_H
#define TARGET_H

#include <QtGui>
#include <QDate>

using namespace std;

class Target
{
public:
								Target();
	void						Restruct(QString rootPath);															// TargetRateファイルの作成

private:
	vector <QDate>				mDate;
	vector <double>				mClose;
	QString						mTemp1;
};

extern Target*					gpTarget;																			// Targetのグローバルポインタ

#endif // TARGET_H
