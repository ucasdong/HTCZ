#include "HealthyForm.h"
#include "ui_HealthyForm.h"
#include <QSignalMapper>
HealthyForm::HealthyForm(QWidget *parent) :
	QDialog(parent),
	m_pStaticCheckDlg(new StaticChecktDialog(parent)),
	m_pWordEngine(new WordEngine),

	ui(new Ui::HealthyForm)
{
	ui->setupUi(this);
	QList<QSplitter *> splitterList = ui->stackedWidget->findChildren<QSplitter *>();
	for (int i = 0; i < splitterList.count(); i++)
	{
		splitterList.at(i)->setStretchFactor(0, 1);
		splitterList.at(i)->setStretchFactor(1, 9);
	}
	QSignalMapper *signalMap = new QSignalMapper(this);
	m_tButtonList.push_back(ui->toolbtn_0);
	m_tButtonList.push_back(ui->toolbtn_1);
	m_tButtonList.push_back(ui->toolbtn_2);
	m_tButtonList.push_back(ui->toolbtn_3);
	m_tButtonList.push_back(ui->toolbtn_5);
	m_tButtonList.push_back(ui->toolbtn_6);
	m_checkButtonId = 0;
	for (int i = 0; i < m_tButtonList.size(); i++)
	{
		signalMap->setMapping(m_tButtonList[i], i);
		connect(m_tButtonList[i], SIGNAL(clicked()), signalMap, SLOT(map()));
	}
	connect(signalMap, SIGNAL(mapped(int)), this, SLOT(onToolButtonClicked(int)));
	initTable();
	m_pStaticCheckDlg->hide();
	ui->badPointTableWidget->hideColumn(0);
}

HealthyForm::~HealthyForm()
{
	delete ui;
}

void HealthyForm::onToolButtonClicked(int id)
{
	if (m_checkButtonId == id)
	{
		m_tButtonList[m_checkButtonId]->setChecked(true);
		return;
	}
	m_tButtonList[m_checkButtonId]->setChecked(false);
	m_checkButtonId = id;
	ui->stackedWidget->setCurrentIndex(id);
}

void HealthyForm::on_btn_chooseDir_clicked()
{
	QString sDir = QFileDialog::getExistingDirectory(
		this,
		"Open",
		".");
	ui->label_file->setText(sDir);
	QDir fromDir(sDir);
	fileInfoList = fromDir.entryInfoList(QStringList() << "*.raw", QDir::Files);

	//while(!dataInput.device()->atEnd() && n < 3)
	//{

	//    QByteArray lineBa =dataInput.device()->read(12384).toHex();
	//    if(bFirst)
	//    {
	//        int nCCD = lineBa.mid(14,2).toInt(nullptr, 16); //与技术要求不一致需要确认
	//        sCCD = QStringLiteral("0%1星相机CCD%2%3").arg((nCCD & 0x0f)/4 + 1).arg((nCCD & 0x0f)%4 + 1).arg(nCCD>191?QStringLiteral("多光谱"):QStringLiteral("全色"));
	//        bFirst = false;
	//    }

	//    if(lineBa.mid(34,2).toInt(nullptr, 16) == 0x1d)
	//    {
	//        sGain = QString("%1%2%3%4%5").arg(lineBa.mid(54,2).toInt(nullptr, 16) & 0xf).arg(lineBa.mid(58,2).toInt(nullptr, 16) & 0xf)
	//                .arg(lineBa.mid(62,2).toInt(nullptr, 16) & 0xf).arg(lineBa.mid(66,2).toInt(nullptr, 16) & 0xf).arg(lineBa.mid(70,2).toInt(nullptr, 16) & 0xf);
	//        sTDI = QString("%1%2%3%4%5").arg(lineBa.mid(74,2).toInt(nullptr, 16) & 0xf).arg(lineBa.mid(78,2).toInt(nullptr, 16) & 0xf)
	//                .arg(lineBa.mid(82,2).toInt(nullptr, 16) & 0xf).arg(lineBa.mid(86,2).toInt(nullptr, 16) & 0xf).arg(lineBa.mid(90,2).toInt(nullptr, 16) & 0xf);
	//        sAlgorithm = lineBa.mid(92,8).toInt(nullptr, 16) == 0x00020007?"qg":lineBa.mid(92,8).toInt(nullptr, 16) == 0x000f0000?"qk":"mc";
	//        sLvBo = QString::number(lineBa.mid(100,4).toInt(nullptr, 16), 16);
	//        n++;
	//    }

	//    if(lineBa.mid(34,2).toInt(nullptr, 16) == 0x1e)
	//    {
	//        sQianWei = QString::number(lineBa.mid(52,40).toInt(nullptr, 16), 16);
	//        n++;
	//    }
	//    if(lineBa.mid(34,2).toInt(nullptr, 16) == 0x1f)
	//    {
	//        sTime = QString::number(lineBa.mid(58,2).toInt(nullptr, 16), 16);
	//        n++;
	//    }

	//}
	//ui->label_fileInfo->setText(QStringLiteral(" 片号:%1,增益:%2,级数:%3,积分时间:%4,算法:%5,滤波档位%6,箝位系数%7").arg(sCCD).arg(sGain)
	//                            .arg(sTDI).arg(sTime).arg(sAlgorithm).arg(sLvBo).arg(sQianWei));

}

void HealthyForm::initTable()
{
	QString sStyle = "QTableWidget::item                          "
		"{                                           "
		"    border:1px solid #F2F8FF;}              "
		"QTableWidget::item::selected                "// 每个单元格被选中状态
		"{"
		" background:#137595;                        "      // 每个单元格被选中时 背景颜色
		"}"
		"QTableWidget                                "
		"{                                           "
		"    background:#FFFFFF;                     "
		"    border:none;                            "
		"    font-size:14px;                         "
		"    font-family:\"Microsoft YaHei\";        "
		"    color:white;                            "
		"    background-color: #1A2B3C;              "
		"    selection-background-color:lightblue;"
		"    alternate-background-color:#F2F8FF;}    "
		"    QHeaderView::section                    "                 // 标题头 每个单独的标题区域
		"    {                                       "
		"    font-size:14px;                         "                 // 每个标题的字体大小
		"    font-family:\"Microsoft YaHei\";        "                 // 每个标题的字体类型
		"    color:#FFFFFF;                          "                 // 每个标题的字体颜色
		"    background:#304458;                     "                 // 每个标题区域的背景色
		"    border:2px;                             "                 // 每个标题区域的边框
		"    text-align:left;                        "                 // 每个标题的对齐方式（貌似不能用）。建议使用tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter)
		"    min-height:49px;                        "                 // 标题区域的高度
		"    max-height:49px;                        "                 //
		"    margin-left:2px;                        "                 // 每个标题区域的margin
		"    padding-left:0px;                       "                 // 每个标题区域的padding
		"}                                           ";
	ui->badPointTableWidget->setStyleSheet(sStyle);
	ui->maxDNTableWidget->setStyleSheet(sStyle);
}

void HealthyForm::initControl(QString restr, QStringList dataList)
{
	{
		QRegularExpression re(restr + "_comboBox*");
		QList<QComboBox *> lampComBoxList = ui->stackedWidget->findChildren<QComboBox *>(re);
		for (int i = 0; i < lampComBoxList.count(); i++)
		{
			lampComBoxList.at(i)->clear();
			lampComBoxList.at(i)->addItems(dataList);
		}

		QRegularExpression reLine("lineEdit_" + restr + "*");
		QList<QLineEdit *> lineEditList = ui->stackedWidget->findChildren<QLineEdit *>(reLine);
		for (int i = 0; i < lineEditList.count(); i++)
		{
			lineEditList.at(i)->setText(dataList.join(";"));
		}
	}
}

void HealthyForm::on_btn_badPoint_clicked()
{
	ui->badPointTableWidget->setRowCount(0);

	if (fileInfoList.isEmpty())
		emit on_btn_chooseDir_clicked();

	for (int indexf = 0; indexf < fileInfoList.length(); indexf++)
	{

		QFile f(fileInfoList.at(indexf).absoluteFilePath());
		if (!f.open(QIODevice::ReadOnly))
		{
			qDebug() << "Open failed.";
			return;
		}
		QDataStream dataInput(&f);
		QString lineStr;
		dataInput.device()->seek(0);
		bool bFirst = true;
		int nLastIndex = 0;
		int nLine = 1;
		vector<int> points;
		qint64 dn[12288] = { 0 };
		memset(dn, 0, sizeof(dn));
		float dnAvg[12288] = { 0 };

		while (!dataInput.device()->atEnd())
		{
			memset(dn, 0, sizeof(dn));
			//long long dnCount = 0;
			for (int j = 0; j < 23; j++)
			{
				QByteArray lineBa = dataInput.device()->read(49312).toHex();
				for (int i = 0; i < 12288; i++)
				{
					dn[i] = dn[i] + lineBa.mid(4 * (i + 40), 4).toInt();
				}
			}
			for (int i = 0; i < 12288; i++)
			{
				dnAvg[i] = dn[i] / 23;
			}
		}

		for (int i = 1; i < 12288 - 1; i++)
		{
			float fPre = dnAvg[i - 1];
			float fCur = dnAvg[i];
			float fNext = dnAvg[i + 1];
			float fXi = ui->DNspinBox->value() / 100.0;
			if (2 * fCur > ((fPre + fNext)*(1 + fXi)) || 2 * fCur < ((fPre + fNext)*(1 - fXi)))
			{
				int nCurrentRowCount = ui->badPointTableWidget->rowCount();
				ui->badPointTableWidget->setRowCount(nCurrentRowCount + 1);//总行数增加1

				QString curSp = QString::fromStdString("B");
				QTableWidgetItem* item = new QTableWidgetItem;
				item->setText(fileInfoList.at(indexf).baseName());
				item->setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
				item->setFlags(Qt::NoItemFlags);
				ui->badPointTableWidget->setItem(nCurrentRowCount, 1, item);

				QTableWidgetItem* itemCCD = new QTableWidgetItem;
				itemCCD->setText(fileInfoList.at(indexf).baseName());
				ui->badPointTableWidget->setItem(nCurrentRowCount, 0, itemCCD);

				QTableWidgetItem* itemNum = new QTableWidgetItem;
				itemNum->setText(QString::number(i));
				ui->badPointTableWidget->setItem(nCurrentRowCount, 2, itemNum);

				QTableWidgetItem* itemValue = new QTableWidgetItem;
				itemValue->setText(QString::number((((2 * fCur) / (fPre + fNext)) - 1) * 100));
				ui->badPointTableWidget->setItem(nCurrentRowCount, 3, itemValue);
			}
		}
	}
}

void HealthyForm::on_btn_jumpCal_clicked()
{
	if (fileInfoList.isEmpty())
		emit on_btn_chooseDir_clicked();

	for (int indexf = 0; indexf < fileInfoList.length(); indexf++)
	{
		QFile f(fileInfoList.at(indexf).absoluteFilePath());
		if (!f.open(QIODevice::ReadOnly))
		{
			qDebug() << "Open failed.";
			return;
		}
		ui->jumpTableWidget->setRowCount(0);
		QDataStream dataInput(&f);
		QString lineStr;
		dataInput.device()->seek(0);
		bool bFirst = true;
		int nLastIndex = 0;
		int nLine = 1;
		while (!dataInput.device()->atEnd())
		{
			QByteArray lineBa = dataInput.device()->read(49312).toHex();

			if (bFirst)
			{
				bFirst = false;
			}
			else
			{
				int curIndex = lineBa.mid(6, 6).toInt(nullptr, 16);
				if ((nLastIndex == 0xffffff && curIndex != 0x00) || (nLastIndex != 0xffffff && curIndex != nLastIndex + 1))
				{

					int nCurrentRowCount = ui->jumpTableWidget->rowCount();
					ui->jumpTableWidget->setRowCount(nCurrentRowCount + 1);//总行数增加1

					QTableWidgetItem* itemCCD = new QTableWidgetItem;
					itemCCD->setText(fileInfoList.at(indexf).absoluteFilePath());
					ui->jumpTableWidget->setItem(nCurrentRowCount, 0, itemCCD);

					QTableWidgetItem* itemCur = new QTableWidgetItem;
					itemCur->setText(QString::number(curIndex));
					ui->jumpTableWidget->setItem(nCurrentRowCount, 1, itemCur);

					QTableWidgetItem* itemPre = new QTableWidgetItem;
					itemPre->setText(QString::number(nLastIndex));
					ui->jumpTableWidget->setItem(nCurrentRowCount, 2, itemPre);
				}
			}
			nLastIndex = lineBa.mid(34, 2).toInt(nullptr, 16);
			nLine++;
		}
	}
}

void HealthyForm::on_btn_stableCal_clicked()
{
	ui->stableChartWidget->removeAllLines();
	if (fileInfoList.isEmpty())
		emit on_btn_chooseDir_clicked();

	for (int indexf = 0; indexf < fileInfoList.length(); indexf++)
	{
		QFile f(fileInfoList.at(indexf).absoluteFilePath());
		if (!f.open(QIODevice::ReadOnly))
		{
			qDebug() << "Open failed.";
			return;
		}
		QDataStream dataInput(&f);
		QString lineStr;
		dataInput.device()->seek(0);
		vector<double> points;
		while (!dataInput.device()->atEnd())
		{
			long long dnCount = 0;
			for (int j = 0; j < 23; j++)
			{
				QByteArray lineBa = dataInput.device()->read(49312).toHex();
				for (int i = 0; i < 12288; i++)
				{
					dnCount = dnCount + lineBa.mid(4 * (i + 40), 4).toInt();
				}
			}
			points.push_back(dnCount / (12288 * 23));
		}
		ui->stableChartWidget->drawALine(fileInfoList.at(indexf).baseName(), points);
	}
}



void HealthyForm::on_btn_LightLine_clicked()
{
	ui->LightLineChartWidget->removeAllLines();
	ui->stableChartWidget->removeAllLines();
	if (fileInfoList.isEmpty())
		emit on_btn_chooseDir_clicked();

	for (int indexf = 0; indexf < fileInfoList.length(); indexf++)
	{
		QFile f(fileInfoList.at(indexf).absoluteFilePath());
		if (!f.open(QIODevice::ReadOnly))
		{
			qDebug() << "Open failed.";
			return;
		}
		QDataStream dataInput(&f);
		QString lineStr;
		dataInput.device()->seek(0);
		vector<double> points;
		bool bFirst = true;
		double dLastAvg = 0;
		while (!dataInput.device()->atEnd())
		{
			long long dnCount = 0;
			for (int j = 0; j < 23; j++)
			{
				QByteArray lineBa = dataInput.device()->read(49312).toHex();

				for (int i = 0; i < 12288; i++)
				{
					dnCount = dnCount + lineBa.mid(4 * (i + 40), 4).toInt();
				}
			}
			if (bFirst)
			{
				bFirst = false;
				points.push_back(0);
			}
			else
			{
				points.push_back(qAbs(dLastAvg - dnCount / (12288 * 23)));
			}
			dLastAvg = dnCount / (12288 * 23);
		}
		ui->LightLineChartWidget->drawALine(fileInfoList.at(indexf).baseName(), points);
		ui->LightLineChartWidget->setYRange(0, dLastAvg);
	}
}

void HealthyForm::on_btn_erroLine_clicked()
{
	/*ui->erroLineChartWidget->removeAllLines();
	vector<InputParam> inputList;
	InputParam CCDParam;
	qDebug() << fileName;
	CCDParam.CCD_ID = fileName.right(8).mid(0, 1).toInt();
	if (fileName.right(7).mid(0, 1) == "P")
	{
		CCDParam.SpectrumList.push_back("P");
	}
	else
	{
		CCDParam.SpectrumList.push_back("B1");
		CCDParam.SpectrumList.push_back("B2");
		CCDParam.SpectrumList.push_back("B3");
		CCDParam.SpectrumList.push_back("B4");
	}
	vector<OutParam> AvgList;
	vector<OutParam> NoisyList;
	vector<OutParam> SNRList;

	QString filePath = fileName;
	filePath = filePath.replace('/', "\\");
	CCDParam.strAbsolutePath = (const char*)filePath.toLocal8Bit();
	inputList.push_back(CCDParam);
	m_Calculator._Cal_SNR_P_LOG20(inputList, AvgList, NoisyList, SNRList);

	double dAvg = 0;

	for (int i = 0; i < AvgList.size(); i++)
	{
		vector<double> points;
		int nLength = AvgList.at(i).list.size();
		points.push_back(0);
		if (AvgList.at(i).list.at(0) > dAvg)
			dAvg = AvgList.at(i).list.at(0);
		for (int j = 1; j < nLength; j++)
		{
			points.push_back(qAbs(AvgList.at(i).list.at(j) - AvgList.at(i).list.at(j - 1)));
		}
		QString curSp = QString::fromStdString(AvgList.at(i).strSpectrum);

		ui->erroLineChartWidget->drawALine(curSp, points);
	}
	ui->erroLineChartWidget->setYRange(0, dAvg);*/
}

void HealthyForm::on_btn_staticReport_clicked()
{
	QString filepath = QFileDialog::getSaveFileName(this, tr("Save as..."),
		QString(), tr("word files (*.doc);;"));
	if (filepath != "")
	{
		m_pStaticCheckDlg->setVisible(true);
		this->hide();
		m_pStaticCheckDlg->show();
		QCoreApplication::processEvents();
		m_pWordEngine->Open(QCoreApplication::applicationDirPath() + "\\staticCheckReport.dot", false);

		m_pWordEngine->replaceText("filename", ui->label_file->text());
		m_pWordEngine->replaceText("fileInfo", ui->label_fileInfo->text());
		on_btn_badPoint_clicked();
		m_pWordEngine->replaceText("badpoint", QString::number(ui->badPointTableWidget->rowCount()));
		if (ui->badPointTableWidget->rowCount() > 0)
		{
			QAxObject * obj = m_pWordEngine->insertTable("badpointInfo", ui->badPointTableWidget->rowCount() + 1, ui->badPointTableWidget->columnCount() - 1);
			for (int j = 1; j < ui->badPointTableWidget->columnCount(); j++)
				m_pWordEngine->setTableCellString(obj, 0, j, ui->badPointTableWidget->horizontalHeaderItem(j)->text());

			for (int i = 0; i < ui->badPointTableWidget->rowCount(); i++)
			{
				for (int j = 1; j < ui->badPointTableWidget->columnCount(); j++)
					m_pWordEngine->setTableCellString(obj, i + 1, j - 1, ui->badPointTableWidget->item(i, j)->text());
			}

		}
		m_pWordEngine->replaceText("jump", QString::number(0));

		on_btn_jumpCal_clicked();
		m_pWordEngine->replaceText("lianxu", QString::number(ui->jumpTableWidget->rowCount()));
		if (ui->jumpTableWidget->rowCount() > 0)
		{
			QAxObject * obj = m_pWordEngine->insertTable("lianxuInfo", ui->jumpTableWidget->rowCount() + 1, ui->jumpTableWidget->columnCount());
			for (int j = 0; j < ui->jumpTableWidget->columnCount(); j++)
				m_pWordEngine->setTableCellString(obj, 0, j, ui->jumpTableWidget->horizontalHeaderItem(j)->text());

			for (int i = 0; i < ui->jumpTableWidget->rowCount(); i++)
			{
				for (int j = 0; j < ui->jumpTableWidget->columnCount(); j++)
					m_pWordEngine->setTableCellString(obj, i + 1, j, ui->jumpTableWidget->item(i, j)->text());
			}

		}

		on_btn_stableCal_clicked();

		QTimer::singleShot(4000, this, [=]() {
			QString lableName = "static";
			QPixmap pixmap = ui->stableChartWidget->chart()->grab();
			QImage image2 = pixmap.toImage();
			image2.save(lableName + ".png");
			m_pWordEngine->replacePic(lableName, QCoreApplication::applicationDirPath() + "\\" + lableName + ".png");
			m_pWordEngine->save(filepath);
			m_pWordEngine->close();
			m_pStaticCheckDlg->hide();
			});


		//        on_btn_stableCal_clicked();
		//        eng.replaceText("lianxu", QString::number(ui->s->rowCount()));
		//        if(ui->jumpTableWidget->rowCount()>0)
		//        {
		//            QAxObject * obj = eng.insertTable("lianxuInfo", ui->jumpTableWidget->rowCount()+1,ui->jumpTableWidget->columnCount());
		//            for(int j = 0; j <  ui->jumpTableWidget->columnCount(); j++)
		//                eng.setTableCellString(obj, 0, j, ui->jumpTableWidget->horizontalHeaderItem(j)->text());

		//            for(int i = 0; i < ui->jumpTableWidget->rowCount(); i++)
		//            {
		//                for(int j = 0; j <  ui->jumpTableWidget->columnCount(); j++)
		//                    eng.setTableCellString(obj, i+1, j, ui->jumpTableWidget->item(i, j)->text());
		//            }

		//        }


	}
}
