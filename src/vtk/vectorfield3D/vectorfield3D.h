/**
  * @file
  * @author Leif Christensen <leif.christensen@dfki.de>
  * @version 0.1
  * 
  * @section DESCRIPTION
  * 
  * Vectorfield3D Widget to display vector (flux density) fields in Vizkit based on VTK.
  */
#ifndef VECTORFIELD3D_H
#define VECTORFIELD3D_H

#include <QtGui>
#include <QtCore>
#include <QString>
#include <QVTKWidget.h>
#include <QtDesigner/QDesignerExportWidget>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkOrientationMarkerWidget.h>
#include <base/Pose.hpp>
#include <base/Eigen.hpp>
#include <vtkObject.h>
#include <vtkCamera.h>

/// Widget to display vector (flux density) fields in Vizkit based on VTK.
  
class QDESIGNER_WIDGET_EXPORT Vectorfield3D : public QVTKWidget
{
    Q_OBJECT
    Q_CLASSINFO("Author", "Leif Christensen")

public:
    Vectorfield3D(QWidget *parent = 0);
    virtual ~Vectorfield3D();

public slots:

  /** Method to set the title of the scalar range legend bar.
    * @param title The scalar range legend bar caption */
  void setScalarBarTitle(QString title);

  /** Set the title of the rendering window.
    * @param title Title of the rendering window */
  void setTitle(QString title);
  
  /** Add a vector to the vectorfield visualization
    * @param pos Origin of the vector
    * @param flux Field strenght and orientation in 3D */ 
  void addVector(base::Position pos, base::Vector3d flux);

  /** Add a vector to the vectorfield visualization.
   * @param px,py,pz The origin of the vector
   * @param x,y,z Field strength and orientation */  
  void addVector(double px, double py, double pz,
                 double x, double y, double z);

  /** adapt last inserted vector */
  void adaptVector(double px, double py, double pz,
                 double x, double y, double z);
  /** Toggle visibility of scalar range legend bar */
  void showScalarBar(bool b);

  /** toggle visibility of orientatin marker */
  void showOrientationMarker(bool b);

  void setInitialMagRange(double min, double max);
  
  void setBackground(double r, double g, double b);

  void setCameraPosition(double x, double y, double z);
  void setCameraFocalPoint(double x, double y, double z);
  void setCameraViewUp(double x, double y, double z);
  
  //void myCallback(vtkObject*,unsigned long eid, void* clientdata, void* calldata);

private:
  vtkSmartPointer<vtkPolyData> vectorfield;
  vtkSmartPointer<vtkPoints> points;
  vtkSmartPointer<vtkFloatArray> vectors;
  vtkSmartPointer<vtkPolyDataMapper> glyphMapper;
  vtkSmartPointer<vtkLookupTable> lut;
  vtkSmartPointer<vtkRenderer> ren;
  vtkSmartPointer<vtkScalarBarActor> scalarBar;
  vtkSmartPointer<vtkOrientationMarkerWidget> marker;
  vtkSmartPointer<vtkCamera> cam;
  double mag_max, mag_min;
};

#endif /* VECTORFIELD3D_H */
