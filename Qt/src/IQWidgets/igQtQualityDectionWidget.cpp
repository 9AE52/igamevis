#include "IQWidgets/igQtQualityDetectionWidget.h"
/**
 * @class   igQtQualityDetectionWidget
 * @brief   igQtQualityDetectionWidget's brief
 */
//#include <HexMeshQuality.h>
//#include <TriMeshQuality.h>
//#include <TetMeshQuality.h>
//#include <QuadMeshQuality.h>
//#include <iGameManager.h>
//#include <iGameScalarsToColors.h>
//#include <iGameDataArray.h>
//#include <iGameModelColorManager.h>
igQtQualityDetectionWidget::igQtQualityDetectionWidget(QWidget* parent) : QWidget(parent), ui(new Ui::QualityDetection) {
	//ui->setupUi(this);
	//connect(ui->triMeshComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(showTriMeshQualityView()));
	//connect(ui->tetMeshComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(showTetMeshQualityView()));
	//connect(ui->quadMeshComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(showQuadMeshQualityView()));
	//connect(ui->hexMeshComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(showHexMeshQualityView()));
	//manager = iGame::iGameManager::Instance();
	//tmpPoints = iGame::iGameFloatArray::New();
	//tmpColors = iGame::iGameFloatArray::New();
	//tmpPoints->SetNumberOfComponents(3);
	//tmpColors->SetNumberOfComponents(3);
	//metricData = iGame::iGameFloatArray::New();
}
igQtQualityDetectionWidget::~igQtQualityDetectionWidget()
{
	//if (this->ColorData != nullptr) {
	//	delete this->ColorData;
	//}
	//delete tmpPoints;
	//delete tmpColors;
	//delete metricData;
}
void igQtQualityDetectionWidget::clearData()
{
	//delete tmpPoints;
	//delete tmpColors;
	//delete metricData;
	//tmpPoints = iGame::iGameFloatArray::New();
	//tmpColors = iGame::iGameFloatArray::New();
	//tmpPoints->SetNumberOfComponents(3);
	//tmpColors->SetNumberOfComponents(3);
	//metricData = iGame::iGameFloatArray::New();

}
void igQtQualityDetectionWidget::updateMetricDataToDraw()
{
	//float sqrt_2 = std::sqrt(2);
	//float sqrt_3 = std::sqrt(3);
	//float sqrt_6 = std::sqrt(6);
	//int n = metricData->GetNumberOfTuples();
	//switch (this->meshMetric)
	//{
	//case TRI_MinAngle:
	//case TET_InternalAngle:
	//	for (int i = 0; i < n; i++) {
	//		auto value = metricData->GetValue(i);
	//		metricData->SetValue(i, std::abs(60 - value));
	//	}
	//	break;
	//case TRI_Jacobian:
	//case QUAD_Jacobian:
	//case HEX_Jacobian:
	//	for (int i = 0; i < n; i++) {
	//		auto value = metricData->GetValue(i);
	//		metricData->SetValue(i, std::abs(1.0 - value));
	//	}
	//	break;
	//case HEX_Strech:
	//case HEX_Diagonal:
	//	for (int i = 0; i < n; i++) {
	//		auto value = metricData->GetValue(i);
	//		metricData->SetValue(i, 1.0 - value);
	//	}
	//	break;
	//case QUAD_MinAngle:
	//	for (int i = 0; i < n; i++) {
	//		auto value = metricData->GetValue(i);
	//		metricData->SetValue(i, std::abs(90 - value));
	//	}
	//	break;
	//case TET_Jacobian:
	//	for (int i = 0; i < n; i++) {
	//		auto value = metricData->GetValue(i);
	//		metricData->SetValue(i, std::abs(sqrt_2 * 0.5 - value));
	//	}
	//	break;
	//case TET_CollapseRatio:
	//	for (int i = 0; i < n; i++) {
	//		auto value = metricData->GetValue(i);
	//		metricData->SetValue(i, std::abs(sqrt_6 / 3 - value));
	//	}
	//	break;
	//case TET_VolAspectRatio:
	//	for (int i = 0; i < n; i++) {
	//		auto value = metricData->GetValue(i);
	//		metricData->SetValue(i, std::abs(sqrt_3 * 0.5 - value));
	//	}
	//	break;
	//default:
	//	break;
	//}
}

std::string igQtQualityDetectionWidget::loadMetricInfo(std::string str)
{
	//float minv = 1e9, maxv = -1e9;
	//float val = 0.0;
	//int n = metricData->GetNumberOfValues();
	//for (int i = 0; i < n; i++) {
	//	val = metricData->GetValue(i);
	//	minv = std::min(minv, val);
	//	maxv = std::max(maxv, val);
	//	//std::cout << metricData->GetValue(i) << std::endl;
	//}
	//updateMetricDataToDraw();
	//return  str + "\nMax Value : " + std::to_string(maxv) + "\nMin Value : " + std::to_string(minv);
    return "";
}
void igQtQualityDetectionWidget::initColorData()
{
	//auto ColorMap = iGame::iGameModelColorManager::Instance()->GetColorMapper();
	//if (reScale) {
	//	ColorMap->InitRange(metricData);
	//}
	//else {
	//	ColorMap->SetRange(0, 1.0);
	//}
	////std::cout << ColorMap->GetRange()[0] << ' ' << ColorMap->GetRange()[1] << '\n';
	//this->ColorData = ColorMap->MapScalars(metricData);
}
//void igQtQualityDetectionWidget::LoadMeshInfoWithCellColor(iGame::iGameTriMesh* mesh, iGame::iGameFloatArray* ColorData) {
//	int fcnt = mesh->GetNumberOfFaces();
//	tmpPoints->Reserve(fcnt * 3);
//	tmpColors->Reserve(fcnt * 3);
//	float rgb[16];
//	for (int i = 0; i < fcnt; i++) {
//		auto f = mesh->GetFace(i);
//		ColorData->GetTuple(i, rgb);
//		for (int j = 0; j < 3; j++) {
//			auto p = mesh->GetPoint(f.v(j));
//			tmpPoints->AddTuple3(p[0], p[1], p[2]);
//			tmpColors->AddTuple3(rgb[0], rgb[1], rgb[2]);
//		}
//	}
//	manager->SetColor(tmpColors->GetRawPointer(), tmpColors->GetNumberOfTuples());
//	manager->SetPoint(tmpPoints->GetRawPointer(), tmpPoints->GetNumberOfTuples());
//	mesh->MappingWithCellColor();
//}
//
//void igQtQualityDetectionWidget::LoadMeshInfoWithCellColor(iGame::iGameQuadMesh* mesh, iGame::iGameFloatArray* ColorData)
//{
//	int fcnt = mesh->GetNumberOfFaces();
//	tmpPoints->Reserve(fcnt * 4);
//	tmpColors->Reserve(fcnt * 4);
//	float rgb[16];
//	for (int i = 0; i < fcnt; i++) {
//		auto f = mesh->GetFace(i);
//		ColorData->GetTuple(i, rgb);
//		for (int j = 0; j < 4; j++) {
//			auto p = mesh->GetPoint(f.v(j));
//			tmpPoints->AddTuple3(p[0], p[1], p[2]);
//			tmpColors->AddTuple3(rgb[0], rgb[1], rgb[2]);
//		}
//	}
//	manager->SetColor(tmpColors->GetRawPointer(), tmpColors->GetNumberOfTuples());
//	manager->SetPoint(tmpPoints->GetRawPointer(), tmpPoints->GetNumberOfTuples());
//	mesh->MappingWithCellColor();
//}
//void igQtQualityDetectionWidget::LoadMeshInfoWithCellColor(iGame::iGameTetMesh* mesh, iGame::iGameFloatArray* ColorData)
//{
//	int ccnt = mesh->GetNumberOfVolumes();
//	tmpPoints->Reserve(ccnt * 12);
//	tmpColors->Reserve(ccnt * 12);
//	float rgb[16];
//	int vhs[4][3] = { {0,1,3},{1,2,3},{2,0,3},{0,2,1}
//	};
//	for (int i = 0; i < ccnt; i++) {
//		auto c = mesh->GetVolume(i);
//		ColorData->GetTuple(i, rgb);
//		for (int j = 0; j < 4; j++) {
//			for (int k = 0; k < 3; k++) {
//				auto vh = c.v(vhs[j][k]);
//				auto p = mesh->GetPoint(vh);
//				tmpPoints->AddTuple3(p[0], p[1], p[2]);
//				tmpColors->AddTuple3(rgb[0], rgb[1], rgb[2]);
//			}
//		}
//	}
//	manager->SetColor(tmpColors->GetRawPointer(), tmpColors->GetNumberOfTuples());
//	manager->SetPoint(tmpPoints->GetRawPointer(), tmpPoints->GetNumberOfTuples());
//	mesh->MappingWithCellColor();
//
//}
//void igQtQualityDetectionWidget::LoadMeshInfoWithCellColor(iGame::iGameHexMesh* mesh, iGame::iGameFloatArray* ColorData)
//{
//	int ccnt = mesh->GetNumberOfVolumes();
//	tmpPoints->Reserve(ccnt * 24);
//	tmpColors->Reserve(ccnt * 24);
//	float rgb[16];
//	for (int i = 0; i < ccnt; i++) {
//		auto c = mesh->GetVolume(i);
//		ColorData->GetTuple(i, rgb);
//		for (int j = 0; j < 6; j++) {
//			auto f = mesh->GetFace(c.f(j));
//			for (int k = 0; k < 4; k++) {
//				auto p = mesh->GetPoint(f.v(k));
//				tmpPoints->AddTuple3(p[0], p[1], p[2]);
//				tmpColors->AddTuple3(rgb[0], rgb[1], rgb[2]);
//			}
//		}
//	}
//	manager->SetColor(tmpColors->GetRawPointer(), tmpColors->GetNumberOfTuples());
//	manager->SetPoint(tmpPoints->GetRawPointer(), tmpPoints->GetNumberOfTuples());
//	mesh->MappingWithCellColor();
//
//}


void igQtQualityDetectionWidget::showTriMeshQualityView()
{
	//std::string acceptableRange;
	//std::string metricName;
	//int metricIndex = ui->triMeshComboBox->currentIndex();
	//this->reScale = true;
	//switch (metricIndex) {
	//case 0: {
	//	this->meshMetric = OFF;
	//	break;
	//}
	//case 1: {
	//	this->meshMetric = TRI_Area;
	//	acceptableRange = "acceptable range: [0, MAX]";
	//	metricName = "Area";
	//	break;
	//}
	//case 2: {
	//	this->meshMetric = TRI_MaxAngle;
	//	acceptableRange = "acceptable range: [60, 90]\nOptimal Value: 60";
	//	metricName = "MaxAngle";
	//	break;
	//}
	//case 3: {
	//	this->meshMetric = TRI_MinAngle;
	//	acceptableRange = "Acceptable Range: [30, 60]\nOptimal Value: 60";
	//	metricName = "MinAngle";
	//	break;
	//}
	//case 4: {
	//	this->meshMetric = TRI_Jacobian;
	//	acceptableRange = "Acceptable Range: [0, MAX]\nOptimal Value: 1";
	//	metricName = "Jacobian";
	//	break;
	//}
	//case 5: {
	//	this->meshMetric = TRI_AspectRatio;
	//	acceptableRange = "Acceptable Range: [1, 1.3]\nOptimal Value: 1";
	//	metricName = "AspectRatio";
	//	break;
	//}
	//case 6: {
	//	this->meshMetric = TRI_EdgeRatio;
	//	acceptableRange = "Acceptable Range: [1, 1.3]\nOptimal Value: 1";
	//	metricName = "EdgeRatio";
	//	break;
	//}
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}
	//this->loadTriMeshMetricData();
	//ui->labelTriMetric->clear();
	//if (this->meshMetric == OFF) {
	//	manager->SetColor(nullptr, 0);
	//	manager->GetCurrentModel()->points = manager->GetCurrentModel()->DataSet->GetPoints()->GetRawPointer();
	//	manager->GetCurrentModel()->pointNum = manager->GetCurrentModel()->DataSet->GetPoints()->GetNumberOfPoints();
	//	manager->GetCurrentModel()->Mesh->Mapping();
	//	manager->GetCurrentModel()->RenderData = manager->GetCurrentModel()->Mesh->GetMappingData();
	//	Q_EMIT updateCurrentModelColor();
	//	return;
	//}
	//ui->labelTriMetric->setText(QString::fromStdString(loadMetricInfo(acceptableRange)));
	//initColorData();
	//LoadMeshInfoWithCellColor(iGame::iGameTriMesh::SafeDownCast(manager->GetCurrentModel()->Mesh), ColorData);
	////LoadMeshInfoWithCellColor(manager->GetCurrentModel()->QuadMesh, ColorData);
	//Q_EMIT updateCurrentModelColor();


}
void igQtQualityDetectionWidget::showQuadMeshQualityView()
{
	//std::string acceptableRange;
	//std::string metricName;
	//int metricIndex = ui->quadMeshComboBox->currentIndex();
	//this->reScale = true;
	//switch (metricIndex) {
	//case 0: {
	//	this->meshMetric = OFF;
	//	break;
	//}
	//case 1: {
	//	this->meshMetric = QUAD_Area;
	//	acceptableRange = "acceptable range: [0, MAX]";
	//	metricName = "Area";
	//	break;
	//}
	//case 2: {
	//	this->meshMetric = QUAD_MaxAngle;
	//	acceptableRange = "acceptable range: [90,135]\nOptimal Value: 90";
	//	metricName = "MaxAngle";
	//	break;
	//}
	//case 3: {
	//	this->meshMetric = QUAD_MinAngle;
	//	acceptableRange = "Acceptable Range: [45, 90]\nOptimal Value: 90";
	//	metricName = "MinAngle";
	//	break;
	//}
	//case 4: {
	//	this->meshMetric = QUAD_Jacobian;
	//	acceptableRange = "Acceptable Range: [0, MAX]\nOptimal Value: 1";
	//	metricName = "Jacobian";
	//	break;
	//}
	//case 5: {
	//	this->meshMetric = QUAD_AspectRatio;
	//	acceptableRange = "Acceptable Range: [1, 1.3]\nOptimal Value: 1";
	//	metricName = "AspectRatio";
	//	break;
	//}
	//case 6: {
	//	this->meshMetric = QUAD_EdgeRatio;
	//	acceptableRange = "Acceptable Range: [1, 1.3]\nOptimal Value: 1";
	//	metricName = "EdgeRatio";
	//	break;
	//}
	//case 7: {
	//	this->meshMetric = QUAD_Warpage;
	//	acceptableRange = "Acceptable Range: [0, 0.7]\nOptimal Value: 0";
	//	metricName = "Warpage";
	//	break;
	//}
	//case 8: {
	//	this->meshMetric = QUAD_Taper;
	//	acceptableRange = "Acceptable Range: [0, 0.7]\nOptimal Value: 0";
	//	metricName = "Taper";
	//	break;
	//}
	//case 9: {
	//	this->meshMetric = QUAD_Skew;
	//	acceptableRange = "Acceptable Range: [0.5, 1]\nOptimal Value: 1";
	//	metricName = "Skew";
	//	break;
	//}
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}
	//this->loadQuadMeshMetricData();
	//ui->labelQuadMetric->clear();
	//if (this->meshMetric == OFF) {
	//	manager->SetColor(nullptr, 0);
	//	manager->GetCurrentModel()->points = manager->GetCurrentModel()->DataSet->GetPoints()->GetRawPointer();
	//	manager->GetCurrentModel()->pointNum = manager->GetCurrentModel()->DataSet->GetPoints()->GetNumberOfPoints();
	//	manager->GetCurrentModel()->Mesh->Mapping();
	//	manager->GetCurrentModel()->RenderData = manager->GetCurrentModel()->Mesh->GetMappingData();
	//	Q_EMIT updateCurrentModelColor();
	//	return;
	//}
	//ui->labelQuadMetric->setText(QString::fromStdString(loadMetricInfo(acceptableRange)));
	//initColorData();
	//LoadMeshInfoWithCellColor(iGame::iGameQuadMesh::SafeDownCast(manager->GetCurrentModel()->Mesh), ColorData);
	////LoadMeshInfoWithCellColor(manager->GetCurrentModel()->QuadMesh, ColorData);
	//Q_EMIT updateCurrentModelColor();

}
void igQtQualityDetectionWidget::showTetMeshQualityView()
{
	//std::string acceptableRange;
	//std::string metricName;
	//int metricIndex = ui->tetMeshComboBox->currentIndex();
	//this->reScale = true;
	//switch (metricIndex) {
	//case 0: {
	//	this->meshMetric = OFF;
	//	break;
	//}
	//case 1: {
	//	this->meshMetric = TET_Volume;
	//	acceptableRange = "acceptable range: [0, MAX]";
	//	metricName = "Volume";
	//	break;
	//}
	//case 2: {
	//	this->meshMetric = TET_AspectRatio;
	//	acceptableRange = "acceptable range: [1,3]\nOptimal Value: 1";
	//	metricName = "AspectRatio";
	//	break;
	//}
	//case 3: {
	//	this->meshMetric = TET_Jacobian;
	//	acceptableRange = "Acceptable Range: [0, MAX]\nOptimal Value: sqrt(2)/2";
	//	metricName = "Jacobian";
	//	break;
	//}
	//case 4: {
	//	this->meshMetric = TET_CollapseRatio;
	//	acceptableRange = "Acceptable Range: [0.1, MAX]\nOptimal Value: sqrt(6)/3";
	//	metricName = "CollapseRatio";
	//	break;
	//}
	//case 5: {
	//	this->meshMetric = TET_Skew;
	//	acceptableRange = "Acceptable Range: [0, 90]\nOptimal Value: 90";
	//	metricName = "Skew";
	//	this->reScale = false;
	//	break;
	//}
	//case 6: {
	//	this->meshMetric = TET_InternalAngle;
	//	acceptableRange = "Acceptable Range: [0, 60]\nOptimal Value: 60";
	//	metricName = "InternalAngle";
	//	break;
	//}
	//case 7: {
	//	this->meshMetric = TET_VolAspectRatio;
	//	acceptableRange = "Acceptable Range: [0, MAX]\nOptimal Value: sqrt(3)/2";
	//	metricName = "Volume AspectRatio";
	//	break;
	//}
	//case 8: {
	//	this->meshMetric = TET_EquiangleSkew;
	//	acceptableRange = "Acceptable Range: [0, MAX]\nOptimal Value: 0";
	//	metricName = "EquiangleSkew";
	//	break;
	//}
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}
	//this->loadtTetMeshMetricData();
	//ui->labelTetMetric->clear();
	//if (this->meshMetric == OFF) {
	//	manager->SetColor(nullptr, 0);
	//	manager->GetCurrentModel()->points = manager->GetCurrentModel()->DataSet->GetPoints()->GetRawPointer();
	//	manager->GetCurrentModel()->pointNum = manager->GetCurrentModel()->DataSet->GetPoints()->GetNumberOfPoints();
	//	manager->GetCurrentModel()->Mesh->Mapping();
	//	manager->GetCurrentModel()->RenderData = manager->GetCurrentModel()->Mesh->GetMappingData();

	//	Q_EMIT updateCurrentModelColor();
	//	return;
	//}

	//ui->labelTetMetric->setText(QString::fromStdString(loadMetricInfo(acceptableRange)));
	//initColorData();
	//LoadMeshInfoWithCellColor(iGame::iGameTetMesh::SafeDownCast(manager->GetCurrentModel()->Mesh), ColorData);
	//Q_EMIT updateCurrentModelColor();

}
void igQtQualityDetectionWidget::showHexMeshQualityView()
{
	//std::string acceptableRange;
	//std::string metricName;
	//int metricIndex = ui->hexMeshComboBox->currentIndex();
	//this->reScale = true;
	//switch (metricIndex)
	//{
	//case 0: {
	//	this->meshMetric = OFF;
	//	break;
	//}
	//case 1: {
	//	this->meshMetric = HEX_Volume;
	//	acceptableRange = "acceptable range: [0, MAX]";
	//	metricName = "Volume";
	//	break;
	//}
	//case 2: {
	//	this->meshMetric = HEX_Taper;
	//	acceptableRange = "acceptable range: [0, 0.5]\nOptimal Value: 0";
	//	metricName = "Taper";
	//	break;
	//}
	//case 3: {
	//	this->meshMetric = HEX_Jacobian;
	//	acceptableRange = "Acceptable Range: [0, MAX]\nOptimal Value: 1";
	//	metricName = "Jacobian";
	//	break;
	//}
	//case 4: {
	//	this->meshMetric = HEX_EdgeRatio;
	//	acceptableRange = "Acceptable Range: [1, MAX]\nOptimal Value: 1";
	//	metricName = "EdgeRatio";
	//	break;
	//}
	//case 5: {
	//	this->meshMetric = HEX_MaxEdgeRatio;
	//	acceptableRange = "Acceptable Range: [1, 1.3]\nOptimal Value: 1";
	//	metricName = "MaxEdgeRatio";
	//	break;
	//}
	//case 6: {
	//	this->meshMetric = HEX_Skew;
	//	acceptableRange = "Acceptable Range: [0, 0.5]\nOptimal Value: 0";
	//	metricName = "Skew";
	//	this->reScale = false;
	//	break;
	//}
	//case 7: {
	//	this->meshMetric = HEX_Strech;
	//	acceptableRange = "Acceptable Range: [0.25, 1]\nOptimal Value: 1";
	//	metricName = "Strech";
	//	this->reScale = false;
	//	break;
	//}
	//case 8: {
	//	this->meshMetric = HEX_Diagonal;
	//	acceptableRange = "Acceptable Range: [0.65, 1]\nOptimal Value: 1";
	//	metricName = "Diagonal";
	//	this->reScale = false;
	//	break;
	//}
	//case 9: {
	//	this->meshMetric = HEX_RelativeSizeSquared;
	//	acceptableRange = "Acceptable Range: [0, 1]";
	//	metricName = "RelativeSizeSquared";
	//	this->reScale = false;
	//	break;
	//}
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}
	//this->loadHexMeshMetricData();
	//ui->labelHexMetric->clear();

	//if (this->meshMetric == OFF) {
	//	manager->SetColor(nullptr, 0);
	//	manager->GetCurrentModel()->points = manager->GetCurrentModel()->DataSet->GetPoints()->GetRawPointer();
	//	manager->GetCurrentModel()->pointNum = manager->GetCurrentModel()->DataSet->GetPoints()->GetNumberOfPoints();
	//	manager->GetCurrentModel()->Mesh->Mapping();
	//	manager->GetCurrentModel()->RenderData = manager->GetCurrentModel()->Mesh->GetMappingData();
	//	Q_EMIT updateCurrentModelColor();
	//	return;
	//}
	//ui->labelHexMetric->setText(QString::fromStdString(loadMetricInfo(acceptableRange)));
	//initColorData();
	//LoadMeshInfoWithCellColor(iGame::iGameHexMesh::SafeDownCast(manager->GetCurrentModel()->Mesh), ColorData);
	//Q_EMIT updateCurrentModelColor();

}
void igQtQualityDetectionWidget::loadTriMeshMetricData()
{
	//if (manager == nullptr || manager->GetCurrentModel() == nullptr || manager->GetCurrentModel()->Mesh == nullptr) {
	//	this->meshMetric = OFF;
	//	return;
	//}
	//auto mesh = iGame::iGameTriMesh::SafeDownCast(manager->GetCurrentModel()->Mesh);
	//int fcnt = mesh->GetNumberOfFaces();
	//metricData->Reserve(fcnt);
	//iGame::TriMeshQuality meshMonitor(mesh);
	//switch (this->meshMetric) {
	//case TRI_Area:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceArea(i));
	//	}
	//	break;
	//case TRI_MaxAngle:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceMaxAngle(i));
	//	}
	//	break;
	//case TRI_MinAngle:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceMinAngle(i));
	//	}
	//	break;
	//case TRI_Jacobian:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceJacobian(i));
	//	}
	//	break;
	//case TRI_AspectRatio:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceAspectRatio(i));
	//	}
	//	break;
	//case TRI_EdgeRatio:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceEdgeRatio(i));
	//	}
	//	break;
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}

	//return;
}
void igQtQualityDetectionWidget::loadtTetMeshMetricData()
{

	//if (manager == nullptr || manager->GetCurrentModel() == nullptr || manager->GetCurrentModel()->Mesh == nullptr) {
	//	this->meshMetric = OFF;
	//	return;
	//}
	//auto mesh = iGame::iGameTetMesh::SafeDownCast(manager->GetCurrentModel()->Mesh);
	//int ccnt = mesh->GetNumberOfVolumes();
	//metricData->Reserve(ccnt);
	//iGame::TetMeshQuality meshMonitor(mesh);
	//switch (this->meshMetric) {
	//case TET_Volume:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellVolume(i));
	//	}
	//	break;
	//case TET_AspectRatio:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellAspectRatio(i));
	//	}
	//	break;
	//case TET_Jacobian:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellJacobian(i));
	//	}
	//	break;
	//case TET_CollapseRatio:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellCollapseRatio(i));
	//	}
	//	break;
	//case TET_Skew:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellVolSkew(i));
	//	}
	//	break;
	//case TET_InternalAngle:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellMinInternalAngle(i));
	//	}
	//	break;
	//case TET_VolAspectRatio:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellVolAspectRatio(i));
	//	}
	//	break;
	//case TET_EquiangleSkew:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellEquiangleSkewness(i));
	//	}
	//	break;
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}
}
void igQtQualityDetectionWidget::loadQuadMeshMetricData()
{

	//if (manager == nullptr || manager->GetCurrentModel() == nullptr || manager->GetCurrentModel()->Mesh == nullptr) {
	//	this->meshMetric = OFF;
	//	return;
	//}
	//auto mesh = iGame::iGameQuadMesh::SafeDownCast(manager->GetCurrentModel()->Mesh);
	//int fcnt = mesh->GetNumberOfFaces();
	//metricData->Reserve(fcnt);
	//iGame::QuadMeshQuality meshMonitor(mesh);
	//switch (this->meshMetric) {
	//case QUAD_Area:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceArea(i));
	//	}
	//	break;
	//case QUAD_MaxAngle:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceMaxAngle(i));
	//	}
	//	break;
	//case QUAD_MinAngle:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceMinAngle(i));
	//	}
	//	break;
	//case QUAD_Jacobian:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceJacobian(i));
	//	}
	//	break;
	//case QUAD_AspectRatio:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceAspectRatio(i));
	//	}
	//	break;
	//case QUAD_EdgeRatio:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceEdgeRatio(i));
	//	}
	//	break;
	//case QUAD_Warpage:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceWarpage(i));
	//	}
	//	break;
	//case QUAD_Taper:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceTaper(i));
	//	}
	//	break;
	//case QUAD_Skew:
	//	for (int i = 0; i < fcnt; i++) {
	//		metricData->AddValue(meshMonitor.GetFaceSkew(i));
	//	}
	//	break;
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}
}
void igQtQualityDetectionWidget::loadHexMeshMetricData()
{

	//if (manager == nullptr || manager->GetCurrentModel() == nullptr || manager->GetCurrentModel()->Mesh == nullptr) {
	//	this->meshMetric = OFF;
	//	return;
	//}
	//auto mesh = iGame::iGameHexMesh::SafeDownCast(manager->GetCurrentModel()->Mesh);
	//int ccnt = mesh->GetNumberOfVolumes();
	//metricData->Reserve(ccnt);
	//iGame::HexMeshQuality meshMonitor(mesh);
	//switch (this->meshMetric) {
	//case HEX_Volume:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellVolume(i));
	//	}
	//	break;
	//case HEX_Taper:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellTaper(i));
	//	}
	//	break;
	//case HEX_Jacobian:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellJacobian(i));
	//	}
	//	break;
	//case HEX_EdgeRatio:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellEdgeRatio(i));
	//	}
	//	break;
	//case HEX_MaxEdgeRatio:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellMaximinEdgeRatio(i));
	//	}
	//	break;
	//case HEX_Skew:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellSkew(i));
	//	}
	//	break;
	//case HEX_Strech:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellStrech(i));
	//	}
	//	break;
	//case HEX_Diagonal:
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellDiagonal(i));
	//	}
	//	break;
	//case HEX_RelativeSizeSquared:
	//{
	//	float avgV = meshMonitor.GetMeshVolume() / (1.0 * ccnt);
	//	for (int i = 0; i < ccnt; i++) {
	//		metricData->AddValue(meshMonitor.GetCellRelativeSizeSquared(i, avgV));
	//	}
	//}
	//break;
	//default:
	//	this->meshMetric = OFF;
	//	break;
	//}
}