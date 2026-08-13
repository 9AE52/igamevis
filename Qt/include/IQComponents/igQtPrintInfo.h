#pragma once
/*输出操作提示信息与模型数据信息.*/
#include <QtWidgets/qtextbrowser.h>
#include <QtCore/qtextcodec.h>
#include <vector>
#include <IQCore/igQtExportModule.h>


class IG_QT_MODULE_EXPORT igQtPrintInfo :
	public QTextBrowser{
	Q_OBJECT
public:
	igQtPrintInfo (QWidget *parent = Q_NULLPTR);
	virtual ~igQtPrintInfo (void);
public:
	void infoClear ();
	void clearPreviousLine();
	void inputSurfaceMeshInfo(QString filename, int vcnt, int fcnt, QString& str);
	void inputVolumeMeshInfo(QString filename, int vcnt, int fcnt, QString &str);
	void outputMeshInfo(QString &filename, int vcnt, int fcnt);
	void printString(QString);

	void inputFileInfo (QString &pathinfo,QString &filetype);//读取的文件信息
	void outputFileInfo (QString &pathinfo);//保存文件信息
	void curveModelInfo (std::vector<std::string> objtype,std::vector<int> ControlNum,std::vector<int> degree);
	void surfaceModelInfo (std::vector<std::string> objtype,std::vector<int> uControlNum,std::vector<int> vControlNum,std::vector<int> udegree,std::vector<int> vdegree);//打印模型信息
private:
	QTextCodec *codec = QTextCodec::codecForName ("GBK");//文本转换对象
};