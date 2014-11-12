#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QtGui>
#include <QDate>

using namespace std;

class Template
{
public:
								Template();
	void						Restruct(QString rootPath);															// TemplateRateファイルの作成

private:
	vector <QDate>				mDate;
	vector <double>				mClose;
	QString						mTemp1;
};

extern Template*				gpTemplate;																			// Templateのグローバルポインタ

#endif // TEMPLATE_H
