//#include "igQtPrintInfo.h"

#include <IQComponents/igQtPrintInfo.h>

igQtPrintInfo::igQtPrintInfo (QWidget *parent): QTextBrowser (parent){
}
igQtPrintInfo::~igQtPrintInfo(void) {

}
void igQtPrintInfo::infoClear (){
	this->clear ();
}
void igQtPrintInfo::printString(QString str) {
	this->insertPlainText(str);
	this->append("");
}

void igQtPrintInfo::clearPreviousLine()
{
	auto cursor = this->textCursor();
	cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor, 1);
	cursor.removeSelectedText();
}
void igQtPrintInfo::inputSurfaceMeshInfo(QString filename, int vcnt, int fcnt, QString &str) {
	this->insertPlainText("read " + filename + " :  vcnt = " + QString::number(vcnt) + ",  fcnt= " + QString::number(fcnt) + ";  " + str);
	this->append("");
}
void igQtPrintInfo::inputVolumeMeshInfo(QString filename, int vcnt, int fcnt, QString& str) {
	this->insertPlainText("read " + filename + " :  vcnt = " + QString::number(vcnt) + ",  ccnt= " + QString::number(fcnt) + ";  " + str);
	this->append("");
}
void igQtPrintInfo::outputMeshInfo(QString &filename, int vcnt = 0, int fcnt = 0) {
	this->insertPlainText("write " + filename + " :  vcnt = " + QString::number(vcnt) + ",  facecnt or cellcnt = " + QString::number(fcnt));
	this->append("");
}
void igQtPrintInfo::inputFileInfo (QString &pathinfo, QString &filetype){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————文件信息———————————————————— "));
	this->append ("");
	this->insertPlainText (codec->toUnicode(" 已读取文件，文件格式： ")+filetype);
	this->append ("");
	this->insertPlainText (codec->toUnicode (" 文件路径： ")+ codec->toUnicode(pathinfo.toStdString().c_str()));
}
void igQtPrintInfo::outputFileInfo (QString &pathinfo){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————文件信息———————————————————— "));
	this->append ("");
	this->insertPlainText (codec->toUnicode (" 已保存文件 "));
	this->append ("");
	this->insertPlainText (codec->toUnicode (" 文件路径： ") + codec->toUnicode(pathinfo.toStdString ().c_str ()));
}
void igQtPrintInfo::curveModelInfo (std::vector<std::string> objtype, std::vector<int> ControlNum, std::vector<int> degree){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————几何曲线信息———————————————————— "));
	for(int i = 0;i < objtype.size ();i++){
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 第 ")+ codec->toUnicode (QString::number (i+1).toStdString ().c_str ())+ codec->toUnicode(" 条曲线 "));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 几何类型： "));
		this->insertPlainText (codec->toUnicode (objtype[i].c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 控制点个数： "));
		this->insertPlainText (codec->toUnicode (QString::number (ControlNum[i]).toStdString ().c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 次数： "));
		this->insertPlainText (codec->toUnicode (QString::number (degree[i]).toStdString ().c_str ()));
		this->append ("");
	}
}
void igQtPrintInfo::surfaceModelInfo (std::vector<std::string> objtype, std::vector<int> uControlNum, std::vector<int> vControlNum, std::vector<int> udegree, std::vector<int> vdegree){
	this->clear ();
	this->insertPlainText (codec->toUnicode (" ————————————————————几何曲面信息———————————————————— "));
	for(int i = 0;i < objtype.size ();i++){
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 第 ") + codec->toUnicode (QString::number (i + 1).toStdString ().c_str ()) + codec->toUnicode (" 张曲面 "));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" 几何类型： "));
		this->insertPlainText (codec->toUnicode(objtype[i].c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" U方向控制点个数： "));
		this->insertPlainText (codec->toUnicode (QString::number(uControlNum[i]).toStdString().c_str()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" V方向控制点个数： "));
		this->insertPlainText (codec->toUnicode (QString::number (vControlNum[i]).toStdString ().c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" U方向次数： "));
		this->insertPlainText (codec->toUnicode (QString::number (udegree[i]).toStdString ().c_str ()));
		this->append ("");
		this->insertPlainText (codec->toUnicode (" V方向次数： "));
		this->insertPlainText (codec->toUnicode (QString::number (vdegree[i]).toStdString ().c_str ()));
		this->append ("");
	}
}
