#include "vectorfield3D.h"

#include <vtkLookupTable.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkPolyDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkAxesActor.h>
//#include <vtkCallbackCommand.h>
//#include <vtkCommand.h>

Vectorfield3D::Vectorfield3D(QWidget *parent)
    : QVTKWidget(parent), 
    vectorfield(vtkSmartPointer<vtkPolyData>::New()),
    points(vtkSmartPointer<vtkPoints>::New()),
    vectors(vtkSmartPointer<vtkFloatArray>::New()),
    glyphMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
    lut(vtkSmartPointer<vtkLookupTable>::New()),
    ren(vtkSmartPointer<vtkRenderer>::New()),
    scalarBar(vtkSmartPointer<vtkScalarBarActor>::New()),
    marker(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
    cam(vtkSmartPointer<vtkCamera>::New()),
    mag_max(0.0), mag_min(1.0)
{

  // Color lookup table hue range and resolution
  lut->SetNumberOfColors(1024);
  lut->SetHueRange(0.667, 0.0);
  lut->SetVectorModeToMagnitude();
  lut->Build();
   
  // data structure to hold vectorfield of type PolyData
  vectorfield->SetPoints(points);
  vectors->SetNumberOfComponents(3);
  vectorfield->GetPointData()->SetVectors(vectors);

  // Arrow source (outputs PolyData)
  vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
  arrow->SetTipResolution(6);
  arrow->SetTipRadius(0.05);
  arrow->SetTipLength(0.35);
  arrow->SetShaftResolution(6);
  arrow->SetShaftRadius(0.015);

  // the actual glyph to vizualize a vector
  vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
  glyph->SetInput(vectorfield);
  glyph->SetSource(arrow->GetOutput()); 
  glyph->SetVectorModeToUseVector();
  glyph->SetColorModeToColorByVector();
  glyph->SetScaleModeToDataScalingOff();
  glyph->OrientOn();
  glyph->SetScaleFactor(0.2);

  // Set up camera view
  cam->SetPosition(10.0, 0, 0);
  cam->SetFocalPoint(0.0, 0.0, 0.0);
  //cam->SetViewUp(0.0, 0.0, 0.0);
  //cam->SetViewAngle(18.604);
  
  // glyph mapper
  glyphMapper->SetInputConnection( glyph->GetOutputPort() );
  glyphMapper->SetLookupTable(lut);

  // Glyph actor 
  vtkSmartPointer<vtkActor> glyphActor = vtkSmartPointer<vtkActor>::New();
  glyphActor->SetMapper( glyphMapper );

  // Scalar scale bar actor
  scalarBar->SetLookupTable(lut);
  scalarBar->SetTitle("Flux Magnitude");
  scalarBar->SetNumberOfLabels(2);
scalarBar->SetOrientationToHorizontal();
  //  scalarBar->SetOrientationToVertical();
  scalarBar->SetWidth(0.6);
  scalarBar->SetHeight(0.05);
  scalarBar->SetPosition(0.2, 0.1);

  // Renderer
  ren->AddActor(glyphActor);
  ren->AddActor(scalarBar);
  //ren->SetBackground( 0.8, 0.8, 0.8 );
  ren->SetActiveCamera(cam);

  // render window
  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer( ren );
  renWin->SetSize(1024, 768);
  SetRenderWindow(renWin);

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  marker->SetOutlineColor(0.93, 0.57, 0.13);
  marker->SetOrientationMarker(axes);
  marker->SetInteractor(renWin->GetInteractor());
  marker->SetViewport(0.0, 0.0, 0.25, 0.25);
  marker->SetEnabled(1);
  marker->InteractiveOff();
  
  //vtkSmartPointer<vtkCallbackCommand> renderCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  //renderCallback->SetCallback(myCallback);
  //ren->AddObserver(vtkCommand::StartEvent,renderCallback);
}

Vectorfield3D::~Vectorfield3D()
{
  //vectorfield->Delete();
  //points->Delete();
  //vectors->Delete();
  //glyphMapper->Delete();
  //lut->Delete();
  //ren->Delete();
  //scalarBar->Delete();
}

void Vectorfield3D::addVector(double px, double py, double pz,
                              double x, double y, double z){
 double v[3];
 v[0] = x;
 v[1] = y;
 v[2] = z;
 double magnitude = sqrt(x*x + y*y + z*z);
 if (magnitude < mag_min) mag_min = magnitude;
 if (magnitude > mag_max) mag_max = magnitude;
 int id = points->GetNumberOfPoints();
 points->InsertPoint(id,px, py, pz);
 points->Modified();
 vectors->InsertTuple(id, v);
 vectors->Modified();
 glyphMapper->SetScalarRange(mag_min, mag_max);
 update();
}   

void Vectorfield3D::adaptVector(double px, double py, double pz,
                              double x, double y, double z){
 double v[3];
 v[0] = x;
 v[1] = y;
 v[2] = z;
 double magnitude = sqrt(x*x + y*y + z*z);
 if (magnitude < mag_min) mag_min = magnitude;
 if (magnitude > mag_max) mag_max = magnitude;
 int id = points->GetNumberOfPoints();
 points->SetPoint(id-1,px, py, pz);
 points->Modified();
 vectors->InsertTuple(id-1, v);
 vectors->Modified();
 glyphMapper->SetScalarRange(mag_min, mag_max);
 update();
}   


void Vectorfield3D::addVector(base::Position pos, base::Vector3d flux){
  Vectorfield3D::addVector(pos(0), pos(1), pos(2), flux(0), flux(1), flux(2));
}

void Vectorfield3D::showScalarBar(bool b){
 b ? ren->AddActor(scalarBar) : ren->RemoveActor(scalarBar);
}

void Vectorfield3D::showOrientationMarker(bool b){
  marker->SetEnabled(b ? 1 : 0);
}

void Vectorfield3D::setTitle(QString title){
  setWindowTitle(title);
}

void Vectorfield3D::setScalarBarTitle(QString title){
  scalarBar->SetTitle(title.toStdString().c_str());
  update();
}

void Vectorfield3D::setInitialMagRange(double min, double max){
  mag_min = min;
  mag_max = max;
  update();
}

void Vectorfield3D::setBackground(double r, double g, double b){
  ren->SetBackground(r,g,b);
}

void Vectorfield3D::setCameraPosition(double x, double y, double z){
  cam->SetPosition(x,y,z);
}

void Vectorfield3D::setCameraFocalPoint(double x, double y, double z){
  cam->SetFocalPoint(x,y,z);
}

void Vectorfield3D::setCameraViewUp(double x, double y, double z){
  cam->SetViewUp(x,y,z);
}

//void Vectorfield3D::myCallback(vtkObject* caller,unsigned long eid, void* clientdata, void* calldata){
//  double pos[3];
//  double focal[3];
//  double up[3];
//
//  cam->GetPosition(pos);
//  cam->GetFocalPoint(focal);
//  cam->GetViewUp(up);
//
//  printf("Pos: %f, %f, %f\n",pos[0], pos[1], pos[2]);
//}
