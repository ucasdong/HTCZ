#include "assistform.h"
#include "ui_assistform.h"
#include <QFile>
assistForm::assistForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::assistForm)
{
    ui->setupUi(this);
	ui->splitter->setStretchFactor(0, 1);
	ui->splitter->setStretchFactor(1, 9);

    m_pAllTableModel = new AssistPicDataModel();
    //m_pMultiTableModel = new AssistPicDataModel;
    ui->tableView->setModel(m_pAllTableModel);
    ui->tableView->resizeColumnsToContents();
    m_pParser = new PayloadParser;
    QFile loadFile(QCoreApplication::applicationDirPath()+"\\assistDefine.json");

    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "could't open projects json";
        return;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();
    QString str = QString::fromUtf8(allData.data()).trimmed();
    str = str.replace('\n', "");
    str = str.replace('\t', "");

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(str.toStdString().data(), &json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "json error!" << json_error.errorString();
        return;
    }
    m_define = jsonDoc.array();
    m_pParser->setConfig(m_define);

}

assistForm::~assistForm()
{
    delete ui;
}

void assistForm::setData(BYTE *data, int length)
{
    //for(int i = length - 6192  * 32 * 4; i <= length - 6192  * 32 * 2; i = i + 6192 * 2)
    //{
    //    unsigned short * pData = (unsigned short *)&data[i];
    //    //qDebug()<<pData[8];
    //    if(swapI16(pData[8]) == 0x0000)
    //    {
    //        QVector<QByteArray> infos = m_pParser->parse(QByteArray((char*)&data[i], 6192  * 32 * 2));
    //        for(int i = 0; i < m_define.size(); i++)
    //        {
    //            QJsonObject jsonObj = m_define[i].toObject();
    //            if(jsonObj["display_mode"].toString() == "lcd")
    //            {
				//	auto t = jsonObj["id"].toString();
    //                QLCDNumber *lcd = ui->layoutWidget->findChild<QLCDNumber *>(jsonObj["id"].toString());
    //                lcd->display(infos.at(i).toInt());
    //            }

    //            if(jsonObj["display_mode"].toString() == "lineEdit")
    //            {
    //                QLineEdit *lineEdit = ui->layoutWidget->findChild<QLineEdit *>(jsonObj["id"].toString());
    //                lineEdit->setText(QString::fromLatin1(infos.at(i).toStdString().data()));
    //            }
    //            if(jsonObj["display_mode"].toString() == "textEdit")
    //            {
    //                QTextEdit *textEdit = ui->layoutWidget->findChild<QTextEdit *>(jsonObj["id"].toString());
    //                textEdit->setText(QString::fromLatin1(infos.at(i).toStdString().data()));
    //            }
    //        }
    //        m_pAllTableModel->setRawData((unsigned char *)pData);
    //        return;
    //    }

    //}
}

void assistForm::setName(QString name)
{
	ui->nameLabel->setText(name);
}
