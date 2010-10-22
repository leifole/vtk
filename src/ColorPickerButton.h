#ifndef COLORPICKERBUTTON_H
#define COLORPICKERBUTTON_H

#include <QtGui/QPushButton>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QColorDialog>

#include <iostream>

/**
 * Widget which consists of a button and a color displayed on top
 * of the button. Clicking the button will open a colorDialog to cahnge the color of the button.<br>
 * The widget extends QPushbutton for the usual button functionality.<br>
 * The button emits a colorSelected signal when the color was changed via the color chooser
 * dialog.
 *
 * <h1>History</h1>
 * 2010-10-22 Initial revision
 *
 * <h1>Known Issues</h1>
 *
 * <h1>To-Do</h1>
 *
 * @author Bjoern Lueck <blueck@dfki.de>
 * @version 0.1
 */
class ColorPickerButton : public QPushButton
{
  
  Q_OBJECT
  
  public:
      /**
       * Constructor initializing the button and setting teh initial color
       * @param displayColor the initial color to be used, defaults to QColor(0, 0, 0)
       * @param parent the parent widget, may be omitted
       */
    ColorPickerButton(QColor displayColor = QColor(0, 0, 0), QWidget* parent = 0);

    /**
     * @seeQPushButton#paintEvent()
     */
    void paintEvent(QPaintEvent* event);

    /**
     * Sets the color to be displayed on the button
     * @param newColor the color to be shown
     */
    void setDisplayColor(QColor newColor) { displayColor = newColor; };

    /**
     * Returns the color which is currently selected
     * @return the color currently selected
     */
    QColor getDisplayColor() { return displayColor; };
    
  public slots:
    /**
     * Opens the color dialog to select the color
     */
    void openColorDialog();

    /**
     * Notifies the button of a color change
     * @param color the new color to be selected
     */
    void newColorSelected(const QColor& color);
    
  signals:
    /**
     * Signals that the color was changed
     * via the color dialog
     * @param color the new color that was selected
     */
    void colorSelected(const QColor& color);
    
  protected:
    /** The current selected color*/
    QColor displayColor;
    /** Dialog to show, when the button was pressed*/
    QColorDialog colorDialog;
  
};

#endif // COLORPICKERBUTTON_H
