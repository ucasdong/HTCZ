#ifndef ASSISTFORM_H
#define ASSISTFORM_H

#include <QWidget>
#include "AssistPicDataModel.h"
#include "CommonHeader.h"
#include "PayloadParser.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class assistForm;
}

class assistForm : public QWidget
{
    Q_OBJECT

public:
    explicit assistForm(QWidget *parent = nullptr);
    ~assistForm();
    void setData(BYTE *data, int length);
	void setName(QString name);


private:
	Ui::assistForm *ui;
    AssistPicDataModel * m_pAllTableModel;
    PayloadParser * m_pParser;
    QJsonArray m_define;
};

#endif // ASSISTFORM_H
