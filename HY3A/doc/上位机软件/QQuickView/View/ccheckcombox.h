#ifndef CCHECKCOMBOX_H
#define CCHECKCOMBOX_H

#include <QComboBox>
#include <QMouseEvent>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidgetItem>

class CCheckCombox : public QWidget
{
    Q_OBJECT
public:
    explicit CCheckCombox(QWidget *parent = nullptr);

    //添加下拉框内容
    void appendItem(const QString &text, bool bChecked);
    void clearItem();
    void appendAllAndCheck(QStringList list, bool bChecked, bool comboVisible = true);
    //QComboBox的虚函数用来隐藏列表框,当单击是复选框时不让隐藏，用来改变状态
    void hidePopup();
    QStringList selectedItems();


private slots:
    void stateChanged(int state);
    void textChanged(const QString &text);

private:
    QListWidget *pListWidget;
    QLineEdit *pLineEdit;
    QComboBox *comboBox;
    QString strSelectedText;
    bool bSelected;

};

#endif // CCHECKCOMBOX_H
