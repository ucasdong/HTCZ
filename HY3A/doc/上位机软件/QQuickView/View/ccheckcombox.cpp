#include "ccheckcombox.h"
#include <QCheckBox>
#include <QDebug>
#include <QHBoxLayout >

CCheckCombox::CCheckCombox(QWidget *parent):
    QWidget(parent)
{
    pListWidget = new QListWidget(this);
    pLineEdit = new QLineEdit(this);
    comboBox = new QComboBox(this);
    comboBox->setModel(pListWidget->model());
    comboBox->setView(pListWidget);
    //comboBox->setLineEdit(pLineEdit);
    pLineEdit->setReadOnly(true);

    connect(pLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    comboBox->setMaximumWidth(5);
    QHBoxLayout  *mainLayout = new QHBoxLayout;
    setWindowFlags(Qt::FramelessWindowHint);

    mainLayout->addWidget(pLineEdit);
    mainLayout->addWidget(comboBox);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setStretch(0,9);
    mainLayout->setStretch(1,1);
    this->setLayout(mainLayout);
    // this->setFixedSize(200,100);
}

void CCheckCombox::appendItem(const QString &text, bool bChecked)
{
    QListWidgetItem *pItem = new QListWidgetItem(pListWidget);
    pListWidget->addItem(pItem);
    pItem->setData(Qt::UserRole, text);
    QCheckBox *pCheckBox = new QCheckBox(this);
    pCheckBox->setText(text);
    pCheckBox->setChecked(bChecked);
    pListWidget->addItem(pItem);
    pListWidget->setItemWidget(pItem, pCheckBox);

    connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
}

void CCheckCombox::clearItem()
{
    pListWidget->clear();
}

void CCheckCombox::appendAllAndCheck(QStringList list, bool bChecked, bool comboVisible)
{
    clearItem();
    comboBox->setVisible(comboVisible);
    for(int i = 0; i < list.size(); i++)
    {
        QString text = list.at(i);
        QListWidgetItem *pItem = new QListWidgetItem(pListWidget);
        pListWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, text);
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(text);
        pListWidget->addItem(pItem);
        pListWidget->setItemWidget(pItem, pCheckBox);

        connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
        pCheckBox->setChecked(bChecked);
    }
}

QStringList CCheckCombox::selectedItems()
{
    QStringList items;
    int nCount = pListWidget->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = pListWidget->item(i);
        QWidget *pWidget = pListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            items.append(strText);
        }
    }
    return  items;
}

void CCheckCombox::stateChanged(int state)
{
    bSelected = true;
    QString strSelectedData("");
    strSelectedText.clear();
    QCheckBox *pSenderCheckBox = nullptr;
    int nCount = pListWidget->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = pListWidget->item(i);
        QWidget *pWidget = pListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(";");
        }
        //所点击的复选框
        if (pSenderCheckBox == pCheckBox)
        {
            int nData = pItem->data(Qt::UserRole).toInt();
            qDebug() << QString("I am sender...id : %1").arg(nData);
        }
    }
    if (strSelectedData.endsWith(";"))
        strSelectedData.remove(strSelectedData.count() - 1, 1);
    if (!strSelectedData.isEmpty())
    {
        //ui.comboBox->setEditText(strSelectedData);
        strSelectedText = strSelectedData;
        pLineEdit->setText(strSelectedData);
        pLineEdit->setToolTip(strSelectedData);
    }
    else
    {
        pLineEdit->clear();
    }
    bSelected = false;
}

void CCheckCombox::textChanged(const QString &text)
{
    if (!bSelected)
        pLineEdit->setText(strSelectedText);
}

