//#include "GraphicCollectAndProcess.h"
#include <QtWidgets/QApplication>
//#include <QDesktopWidget>
#include <QTextCodec>
#include <QMessageBox>
//#include <DbgHelp.h>
//#include <windows.h>
#include "MainInputClass.h"

/*
typedef struct 
{
	int screen_no;
	QRect rect;
}SCREEN;

#pragma comment(lib, "dbghelp.lib")

void HTCZMessageHandler(QtMsgType type, const QMessageLogContext &logContext, const QString &str)
{
	QString timeString = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
	QString txt;
	switch (type)
	{
	case QtDebugMsg:
		txt = QString("[%1] Debug: %2").arg(timeString).arg(str);
		break;
	case QtWarningMsg:
		txt = QString("[%1] Warning: %2").arg(timeString).arg(str);
		break;
	case QtCriticalMsg:
		txt = QString("[%1] Critical: %2").arg(timeString).arg(str);
		break;
	case QtFatalMsg:
		txt = QString("[%1] Fatal: %2").arg(timeString).arg(str);
		QMessageBox::warning(NULL, ("Error"), txt);
		break;
	default:
		break;
	}

	QString logFile = QString("%1/log/error.log").arg(QCoreApplication::applicationDirPath());
	QFile outFile(logFile);
	if (outFile.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream ts(&outFile);
		ts << txt << endl;
	}
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	QString dumpDir = QString("%1/dump/").arg(QCoreApplication::applicationDirPath());
	::CreateDirectoryA(dumpDir.toStdString().c_str(), NULL);
	std::string strDumpFile = dumpDir.toStdString() + QDateTime::currentDateTime().toLocalTime().toString("yyyyMMddhhmm").toStdString() + ".dmp";
	HANDLE hDumpFile = CreateFileA(strDumpFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDumpFile != INVALID_HANDLE_VALUE) {

		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	}

	EXCEPTION_RECORD* record = pException->ExceptionRecord;
	QString errCode(QString::number(record->ExceptionCode, 16)), errAdr(QString::number((uint)record->ExceptionAddress, 16)), errMod;
	QMessageBox::warning(NULL, QStringLiteral("异常"), QStringLiteral("错误内容如下\r\n") +
		QString("Error Code:%1,nError Address:%2").arg(errCode).arg(errAdr),
		QMessageBox::Ok);
	return EXCEPTION_EXECUTE_HANDLER;

}
*/

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	// 注册日志MsgHandler
	//qInstallMessageHandler(HTCZMessageHandler);
	//SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);	//	注冊异常捕获函数
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

	MainInputClass mainInput;
	mainInput.startWindow();
	return a.exec();
}
