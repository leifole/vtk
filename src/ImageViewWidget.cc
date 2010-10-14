/* 
 * File:   ImageViewWidget.cc
 * Author: blueck
 * 
 * Created on 17. Juni 2010, 14:14
 */

#include "ImageViewWidget.h"
#include <stdexcept>
#include <QtCore/QtPlugin>

using namespace base::samples::frame;
Q_EXPORT_PLUGIN2(ImageViewWidget, ImageViewWidget)

ImageViewWidget::ImageViewWidget(int width, int height, QImage::Format format):
image(width, height, format),
scale_factor(1),
contextMenu(this)
{
 //   setMinimumSize(QSize(width, height));
    act_image = & image;
    //create actions
    save_image_act = new QAction(tr("&Save Image"),this);
    save_image_act->setStatusTip(tr("Save the image to disk"));
    connect(save_image_act,SIGNAL(triggered()),this,SLOT(saveImage()));
    contextMenu.addAction(save_image_act);
}


ImageViewWidget::~ImageViewWidget()
{
    disconnect(save_image_act, 0, 0, 0);
    delete save_image_act;
//   items.clear();
}

void ImageViewWidget::contextMenuEvent ( QContextMenuEvent * event )
{
   contextMenu.exec(event->globalPos());
}

void ImageViewWidget::saveImage()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save File"),
                            save_path,
                            tr("Images (*.png)"));
    if(path.length() > 0)	
    {
      save_path = path;
      act_image->save(save_path,"PNG",80);
    }
}
bool ImageViewWidget::saveImage2(QString path)
{
    act_image->save(path,"PNG",80);
}

bool ImageViewWidget::saveImage3(const QString &mode, int pixel_size,  int width,  int height,const char* pbuffer, QString path)
{
    QImage image;
    configQImage(image,width,height,pixel_size,mode);
    copyToQImage(image, mode, pixel_size,width,height,pbuffer);
    image.save(path,"PNG",80);
}

QObject* ImageViewWidget::addText(int xPos, int yPos, int groupNr, const QString &text)
{
    TextItem* textItem = new TextItem(xPos, yPos, groupNr, text);
    items.push_back(textItem);
    return textItem;
}

QObject* ImageViewWidget::addLine(int xPos, int yPos, int groupNr, const QColor &color, int endX, int endY)
{
    LineItem* item = new LineItem(xPos, yPos, groupNr, color, endX, endY);
    items.push_back(item);
    return item;
}

QObject* ImageViewWidget::addEllipse(int xPos, int yPos, int groupNr, const QColor &color, int width, int height)
{
    EllipseItem* item = new EllipseItem(xPos, yPos, groupNr, color, width, height);
    items.push_back(item);
    return item;
}

QObject* ImageViewWidget::addRectangle(int xPos, int yPos, int groupNr, const QColor &color, int width, int height)
{
    RectangleItem* item = new RectangleItem(xPos, yPos, groupNr, color, width, height);
    items.push_back(item);
    return item;
}
QObject* ImageViewWidget::addPolyline(int groupNr, const QColor &color, const QList<QPoint> &points)
{
    PolylineItem* item = new PolylineItem(color, groupNr, points);
    items.push_back(item);
    return item;
}

QObject* ImageViewWidget::addPolygon(int groupNr, const QColor &color, const QList<QPoint> &points)
{
    PolygonItem* item = new PolygonItem(color, groupNr, points);
    items.push_back(item);
    return item;
}

QObject* ImageViewWidget::addItem(QObject* object)
{
    DrawItem* drawItem = dynamic_cast<DrawItem*>(object);
    items.push_back(drawItem);
    return object;
}

 QObject* ImageViewWidget::removeItem(QObject* object,bool delete_object)
{
    DrawItem *draw_item = dynamic_cast<DrawItem*>(object);
    items.removeAll(draw_item);
    if(delete_object)
    {
      delete object;
      object = NULL;
    }
    return object;
}

void ImageViewWidget::removeAllItems(bool delete_objects)
{
   if(delete_objects)
   {
      QList<DrawItem*>::iterator iter = items.begin();
      for(;iter != items.end();++iter)
          delete(*iter);
   }
   items.clear();
}

void ImageViewWidget::setGroupStatus(int groupNr, bool enable)
{
    if(!enable)
    {
        disabledGroups.push_back(groupNr);
    }
    else
    {
        disabledGroups.removeAll(groupNr);
    }
    repaint();
}

void ImageViewWidget::clearGroups()
{
    disabledGroups.clear();
}


void ImageViewWidget::changeFormat(int width, int height, QImage::Format format)
{
    image = QImage(width, height, format);
 //   setMinimumSize(QSize(width*scale_factor, height*scale_factor));
}



void ImageViewWidget::changeFormat2(QString mode, int pixel_size, int width, int height)
{
  configQImage(image,width,height,pixel_size,mode);
 // setMinimumSize(QSize(width*scale_factor, height*scale_factor));

}



QImage::Format ImageViewWidget::getFormat(frame_mode_t mode,int pixel_size)
{
  switch (Frame::getChannelCount(mode))
  {
  case 1:
      switch (pixel_size)
      {
	case 1:
	  return QImage::Format_Indexed8;
	default:
	  throw "Unknown format. Can not convert Frame "
		"to QImage.";
      }
      break;
  case 3:
	switch (pixel_size)
	{
	case 3:
	    return QImage::Format_RGB888;
	default:
	    throw "Unknown format. Can not convert Frame "
	    "to QImage.";
	}
	break;
  default:
      throw "Unknown format. Can not convert Frame "
	    "to QImage.";
  }
  return QImage::Format_Invalid;
}

void ImageViewWidget::copyToQImage(QImage &image,const QString &mode, int pixel_size,  int width,  int height,const char* pbuffer)
{
  std::string temp = mode.toStdString();
  base::samples::frame::frame_mode_t _mode = Frame::toFrameMode(temp);
  if(_mode == MODE_UNDEFINED)
    throw std::runtime_error(" ImageViewWidget::addRawImage: Wrong mode");

  //convert bayer pattern to rgb24 (rgb888)
  if(_mode == MODE_BAYER_RGGB || _mode == MODE_BAYER_GRBG || _mode == MODE_BAYER_BGGR || _mode == MODE_BAYER_GBRG)
      FrameHelper::convertBayerToRGB24((const uint8_t*)pbuffer,(uint8_t*) image.bits(), width, height ,_mode);
  else
  {
      QImage::Format format = getFormat(_mode,pixel_size);
      memcpy( image.bits(),pbuffer,width*height*pixel_size);
  }
}

void ImageViewWidget::addRawImage(const QString &mode, int pixel_size,  int width,  int height,const char* pbuffer)
{
   copyToQImage(image, mode, pixel_size,width,height,pbuffer);
 //   act_image = prepareForDrawing(image,temp_image,scale_factor,true);
  //  glImage = QGLWidget::convertToGLFormat(image);
 //   repaint();


    updateGL();
}

void ImageViewWidget::addImage(const QImage &image)
{
    memcpy(this->image.bits(), image.bits(),  image.numBytes());
    repaint();
}

void ImageViewWidget::addDrawItemsToWidget(QImage &shownImage)
{
    if(!items.empty())
    {
        QPainter painter(&shownImage);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
	QList<DrawItem*>::iterator iter = items.begin();
        for(;iter != items.end();++iter)
        {
	  if(!disabledGroups.contains((*iter)->getGroupNr()))
                (*iter)->draw(&painter);
        }
    }
}

void ImageViewWidget::configQImage(QImage &image, int width, int height,int pixel_size, QString mode)
{
  std::string temp = mode.toStdString();
  base::samples::frame::frame_mode_t _mode = Frame::toFrameMode(temp);
  switch(_mode)
  {
    case MODE_BAYER_RGGB:
    case MODE_BAYER_GRBG:
    case MODE_BAYER_BGGR:
    case MODE_BAYER_GBRG:
      image = QImage(width, height, QImage::Format_RGB888);
      break;

    default:
    {
      QImage::Format format = getFormat(_mode,pixel_size);
      image = QImage(width, height,format);
      //change colors to grayscale
      if(format == QImage::Format_Indexed8)
      {
        for(int i = 0;i<256;++i)
          image.setColor(i,qRgb(i,i,i));
      }
    }
  }
}

QImage* ImageViewWidget::prepareForDrawing(QImage &image,QImage &temp,float scale_factor,bool overlay)
{
   QImage* pimage = &image;
   if (scale_factor != 1)
   {
     temp = pimage->scaledToHeight(image.height() * 0.5);
     pimage = &temp;
   }

   if(pimage->format() == QImage::Format_Indexed8)
   {
     //convert image to RGB
     temp = pimage->convertToFormat (QImage::Format_RGB888);
     pimage = &temp;
   }
   if(overlay)
     addDrawItemsToWidget(*pimage);
   return pimage;
}

void ImageViewWidget::paintEvent(QPaintEvent* event)
{
   /* QImage shownImage(image);
    addDrawItemsToWidget(shownImage);
    QPainter qpainter(this);
    qpainter.drawImage(0, 0, shownImage);
    */
 /*   QPainter qpainter(this);
    act_image = prepareForDrawing(image,temp_image,scale_factor,true);
    qpainter.drawImage(0, 0, *act_image);*/
}

void ImageViewWidget::paintGL()
{

  glDrawPixels(image.width(), image.height(), GL_RGB, GL_UNSIGNED_BYTE, image.bits());
}

void ImageViewWidget::resizeGL(int w, int h)
{
    glPixelZoom(((float)width())/ image.width(),-((float)height())/ image.height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    glRasterPos2i(0,height());
}


