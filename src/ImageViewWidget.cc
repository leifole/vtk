/* 
 * File:   ImageViewWidget.cc
 * Author: blueck
 * 
 * Created on 17. Juni 2010, 14:14
 */

#include "ImageViewWidget.h"
#include <stdexcept>

using namespace base::samples::frame;

ImageViewWidget::ImageViewWidget(int width, int height, QImage::Format format)
{
    image = new QImage(width, height, format);
    this->format = format;
    this->width = width;
    this->height = height;
    this->setMinimumSize(QSize(width, height));
}

ImageViewWidget::~ImageViewWidget()
{
    items.clear();
}

void ImageViewWidget::addText(int xPos, int yPos, int groupNr, QColor* color, char* text)
{
    TextItem* textItem = new TextItem(xPos, yPos, groupNr, color, text);
    items.push_back(textItem);
   // return textItem;
}

DrawItem* ImageViewWidget::addLine(int xPos, int yPos, int groupNr, QColor* color, int endX, int endY)
{
    LineItem* item = new LineItem(xPos, yPos, groupNr, color, endX, endY);
    items.push_back(item);
    return item;
}

DrawItem* ImageViewWidget::addEllipse(int xPos, int yPos, int groupNr, QColor* color, int width, int height)
{
    EllipseItem* item = new EllipseItem(xPos, yPos, groupNr, color, width, height);
    items.push_back(item);
    return item;
}

DrawItem* ImageViewWidget::addRectangle(int xPos, int yPos, int groupNr, QColor* color, int width, int height)
{
    RectangleItem* item = new RectangleItem(xPos, yPos, groupNr, color, width, height);
    items.push_back(item);
    return item;
}

DrawItem* ImageViewWidget::addPolyline(int groupNr, QColor* color, QPoint* points, int numberOfPoints)
{
    PolylineItem* item = new PolylineItem(color, groupNr, points, numberOfPoints);
    items.push_back(item);
    return item;
}

DrawItem* ImageViewWidget::addPolygon(int groupNr, QColor* color, QPoint* points, int numberOfPoints)
{
    PolygonItem* item = new PolygonItem(color, groupNr, points, numberOfPoints);
    items.push_back(item);
    return item;
}

void ImageViewWidget::addItem(DrawItem* drawItem)
{
    items.push_back(drawItem);
}

void ImageViewWidget::removeItem(DrawItem* drawItem)
{
    items.removeAll(drawItem);
}

void ImageViewWidget::removeAll(DrawType drawType)
{
    QList<DrawItem*> removeItems;
    for(int i=0;i<items.size();i++)
    {
        if(items[i]->getType() == drawType)
        {
            removeItems.push_back(items[i]);
        }
    }
    for(int i=0;i<removeItems.size();i++)
    {
        items.removeAll(removeItems[i]);
    }
}

void ImageViewWidget::removeAllItems()
{
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
    delete image;
    image = new QImage(width, height, format);
}


void ImageViewWidget::addImage(uchar* data, int numBytes)
{
    memcpy(this->image->bits(), data, numBytes);
    repaint();
}

void ImageViewWidget::changeFormat2(QString mode, int pixel_size, int width, int height)
{
  std::string temp = mode.toStdString();
  base::samples::frame::frame_mode_t _mode = Frame::toFrameMode(temp);
  if(_mode == MODE_BAYER_RGGB || _mode == MODE_BAYER_GRBG || _mode == MODE_BAYER_BGGR || _mode == MODE_BAYER_GBRG)
      changeFormat(width,height,QImage::Format_RGB888);
  else
    changeFormat(width,height,getFormat(_mode,pixel_size));
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

void ImageViewWidget::addRawImage(QString mode, int pixel_size,  int width,  int height,const char* pbuffer)
{
    std::string temp = mode.toStdString();
    base::samples::frame::frame_mode_t _mode = Frame::toFrameMode(temp);
    if(_mode == MODE_UNDEFINED)
      throw std::runtime_error(" ImageViewWidget::addRawImage: Wrong mode");
    
    //convert bayer pattern to rgb24 (rgb888) 
    if(_mode == MODE_BAYER_RGGB || _mode == MODE_BAYER_GRBG || _mode == MODE_BAYER_BGGR || _mode == MODE_BAYER_GBRG)
        FrameHelper::convertBayerToRGB24((const uint8_t*)pbuffer,(uint8_t*) this->image->bits(), width, height ,_mode);
    else
    {
	QImage::Format format = getFormat(_mode,pixel_size);
	memcpy( this->image->bits(),pbuffer,width*height*pixel_size);
    }
    repaint();
}

void ImageViewWidget::addImage(QImage* image)
{
    addImage(image->bits(), image->numBytes());
}

void ImageViewWidget::addImageAndScale(uchar* data, int numBytes, int origWidth, int origHeight)
{
    // dont scale if not needed
    if(origHeight == height && origWidth == width)
    {
       addImage(data, numBytes);
    }
    else
    {
      QImage tempImage(data, origWidth, origHeight, format);
      QImage scaled = tempImage.scaled(width, height);
      addImage(&scaled);
    }
}


void ImageViewWidget::addDrawItemsToWidget(QImage* shownImage)
{
    if(items.size() > 0)
    {
        QPainter painter(shownImage);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
        for(int i=0;i<items.size();i++)
        {
            DrawItem* item = items[i];
            if(!disabledGroups.contains(item->getGroupNr()))
            {
                item->draw(&painter);
            }
        }
    }
}

void ImageViewWidget::paintEvent(QPaintEvent* event)
{
    // always paint on a copy of the actual QImage as otherwise
    // you cant go back to the correct image without lines, texts
    // and the like
    QImage shownImage(*image);
    addDrawItemsToWidget(&shownImage);
    QPainter qpainter(this);
    qpainter.drawImage(0, 0, shownImage);
//    delete image;
}


