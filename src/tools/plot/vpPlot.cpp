/****************************************************************************
 *
 * $Id: vpPlot.cpp 3821 2012-06-27 13:50:37Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2012 by INRIA. All rights reserved.
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional 
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 * 
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Description:
 * Plot curves.
 *
 * Authors:
 * Nicolas Melchior
 *
 *****************************************************************************/



#include <visp/vpConfig.h>

#if defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined(VISP_HAVE_OPENCV) 
#include <visp/vpPlot.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpMath.h>
#include <visp/vpMeterPixelConversion.h>
#include <visp/vpPixelMeterConversion.h>
#include <visp/vpPose.h>
#include <fstream>
#include <list>
#include <vector>

/*!
  Default constructor.

  Needs then a call to init().

*/
vpPlot::vpPlot()
{
  graphList = NULL;
  display = NULL;
    
  margei = 30;
  margej = 40;
}
/*!
  This constructor creates a new window where the curves
  will be drawn. The number of graphics in the window must be set.

  \warning You can modify the default window size, but this is not advised.

  \warning Under Unix, when X11 is used to display the curves, the
  default font is set to "-adobe-times-medium-i-normal--10-100-75-75-p-52-iso8859-*". 
  Note that you can chose an other one using "xfontsel".

  \param graphNbr : The number of graph in the window.
  \param height : Height of the window.
  \param width : Width of the window.
  \param x,y : The window is set at position x,y (column index, row index).
  \param title : Window title. 
*/
vpPlot::vpPlot(const unsigned int graphNbr, 
	       const unsigned int height, const unsigned int width, 
	       const int x, const int y, const char *title)
{
  graphList = NULL;
  display = NULL;
      
  margei = 30;
  margej = 40;
  
  init(graphNbr, height, width, x, y, title);
}

/*!
  Creates a new window where the curves
  will be drawn. The number of graphics in the window must be set.

  \warning You can modify the default window size, but this is not advised.

  \param graphNbr : The number of graph in the window.
  \param height : Height of the window.
  \param width : Width of the window.
  \param x,y : The window is set at position x,y (column index, row index).
  \param title : Window title. 
*/
void vpPlot::init(const unsigned int graphNbr, 
		  const unsigned int height, const unsigned int width, 
		  const int x, const int y, const char *title)
{
  I.init(height,width,255);
    
#if defined VISP_HAVE_X11
  display = new vpDisplayX;
#elif defined VISP_HAVE_GDI
  display = new vpDisplayGDI;
#elif defined VISP_HAVE_OPENCV
  display = new vpDisplayOpenCV;
#endif

  display->init(I, x, y, title);
  
  vpDisplay::display(I);
    
  factori = height/700.0f;
  factorj = width/700.0f;
    
  initNbGraph(graphNbr);
}

/*!
  Basic destructor
*/
vpPlot::~vpPlot()
{
  if (graphList != NULL)
  {
    delete[] graphList;
    graphList = NULL;
  }
  if (display != NULL)
  {
    delete display;
    display = NULL;
  }
}

/*!
  Function called by the constructor to initialize the window and the basic parameters.

  \exception vpException::dimensionError if the parameter exceed the possible
  number of graph (4).

  \param graphNbr : The number of graph in the window.
*/
void
vpPlot::initNbGraph (unsigned int nbGraph)
{
  if(nbGraph > 4){
    throw vpException(vpException::dimensionError, "Cannot create more than 4 graphs");
  }
  graphList = new vpPlotGraph[nbGraph];
  graphNbr = nbGraph;
  
  switch (nbGraph)
  {
    case 1 : 
      graphList[0].initSize(vpImagePoint(0,0), (unsigned int)(700*factorj),(unsigned int)(700*factori),margei,margej);
      break;
    case 2 :
      graphList[0].initSize(vpImagePoint(0,0),(unsigned int)(700*factorj),(unsigned int)(350*factori),margei,margej);
      graphList[1].initSize(vpImagePoint((unsigned int)(350*factori),0),(unsigned int)(700*factorj),(unsigned int)(350*factori),margei,margej);
      break;
    case 3 :
      graphList[0].initSize(vpImagePoint(0,0),(unsigned int)(350*factorj),(unsigned int)(350*factori),margei,margej);
      graphList[1].initSize(vpImagePoint(0,(unsigned int)(350*factorj)),(unsigned int)(350*factorj),(unsigned int)(350*factori),margei,margej);
      graphList[2].initSize(vpImagePoint((unsigned int)(350*factori),0),(unsigned int)(700*factorj),(unsigned int)(350*factori),margei,margej);
      break;
    case 4 :
      graphList[0].initSize(vpImagePoint(0,0),(unsigned int)(350*factorj),(unsigned int)(350*factori),margei,margej);
      graphList[1].initSize(vpImagePoint(0,(unsigned int)(350*factorj)),(unsigned int)(350*factorj),(unsigned int)(350*factori),margei,margej);
      graphList[2].initSize(vpImagePoint((unsigned int)(350*factori),0),(unsigned int)(350*factorj),(unsigned int)(350*factori),margei,margej);
      graphList[3].initSize(vpImagePoint((unsigned int)(350*factori),(unsigned int)(350*factorj)),(unsigned int)(350*factorj),(unsigned int)(350*factori),margei,margej);
      break;
  }
  
  for (unsigned int i = 0; i < graphNbr; i++)
  {
    strcpy(graphList[i].title, "");
    strcpy(graphList[i].unitx, "");
    strcpy(graphList[i].unity, "");
    strcpy(graphList[i].unitz, "");
    graphList[i].textdispayed=false;
  }
}

/*!
  Function which enables to initialize the number of curves which belongs to a graphic.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param curveNbr : The number of curves belonging to the graphic
*/
void
vpPlot::initGraph (unsigned int graphNum, unsigned int curveNbr)
{
  (graphList+graphNum)->initGraph(curveNbr);
}


// void
// vpPlot::initRange (const int graphNum, 
// 		   double xmin, double xmax, double /*xdelt*/, 
// 		   double ymin, double ymax, double /*ydelt*/, 
// 		   const bool gx, const bool gy)
// {
//   (graphList+graphNum)->initScale(I,xmin,xmax,10,ymin,ymax,10,gx,gy);
// }

/*!
  This method enables to set the initial range of the selected graphic.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param xmin : The initial minimum value along the x axis given in the user coordinates.
  \param xmax : The initial maximum value along the x axis given in the user coordinates.
  \param ymin : The initial minimum value along the y axis given in the user coordinates.
  \param ymax : The initial maximum value along the y axis given in the user coordinates.
*/
void
vpPlot::initRange (const unsigned int graphNum, 
		   double xmin, double xmax, 
		   double ymin, double ymax)
{
  (graphList+graphNum)->initScale(I,xmin,xmax,10,ymin,ymax,10,true,true);
}

/*!
  This method enables to set the initial range of the selected graphic.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param xmin : The initial minimum value along the x axis given in the user coordinates.
  \param xmax : The initial maximum value along the x axis given in the user coordinates.
  \param ymin : The initial minimum value along the y axis given in the user coordinates.
  \param ymax : The initial maximum value along the y axis given in the user coordinates.
  \param zmin : The initial minimum value along the z axis given in the user coordinates.
  \param zmax : The initial maximum value along the z axis given in the user coordinates.
*/
void
vpPlot::initRange (const unsigned int graphNum, 
		   double xmin, double xmax, double ymin, 
		   double ymax, double zmin, double zmax)
{
  (graphList+graphNum)->initScale(I,xmin,xmax,10,ymin,ymax,10,zmin,zmax,10,true,true);
}

/*!
  This function enables you to choose the color used to draw a given curve.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param curveNum : The index of the curve in the list of the curves belonging to the graphic.
  \param color : The color you want to use
*/
void
vpPlot::setColor (const unsigned int graphNum, const unsigned int curveNum, vpColor color)
{
  (graphList+graphNum)->setCurveColor(curveNum, color);
}

/*!
  display the grid for all graphics.
*/
void
vpPlot::displayGrid()
{
  for (unsigned int i = 0; i < graphNbr; i++)
    graphList[i].displayGrid(I);
}

/*!
  This function enables you to add a new point in the curve. This point is drawn with the parameters of the curve.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param curveNum : The index of the curve in the list of the curves belonging to the graphic.
  \param x : The coordinate of the new point along the x axis and given in the user unit system.
  \param y : The coordinate of the new point along the y axis and given in the user unit system.
*/
void
vpPlot::plot (const unsigned int graphNum, const unsigned int curveNum, const double x, const double y)
{
  (graphList+graphNum)->plot(I,curveNum,x,y);
}

/*!
  This function enables you to add new points in all curves of a plot. These points are drawn with the parameters of the curves.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param x : The coordinate of the new points along the x axis and given in the user unit system.
  \param v_y : y coordinates vector. The coordinates of the new points along the y axis and given in the user unit system.
*/
void vpPlot::plot(const unsigned int graphNum, 
      const double x, const vpColVector &v_y)
{
	if((graphList+graphNum)->curveNbr == v_y.getRows())
	{
		for(unsigned int i = 0;i < v_y.getRows();++i)
			this->plot(graphNum, i, x, v_y[i]);
	}
	else
		vpTRACE("error in plot vector : not the right dimension");
}

/*!
  This function enables you to add a new point in the curve. This point is drawn with the parameters of the curve.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param curveNum : The index of the curve in the list of the curves belonging to the graphic.
  \param x : The coordinate of the new point along the x axis and given in the user unit system.
  \param y : The coordinate of the new point along the y axis and given in the user unit system.
  \param z : The coordinate of the new point along the z axis and given in the user unit system.
*/
void
vpPlot::plot (const unsigned int graphNum, const unsigned int curveNum, const double x, const double y, const double z)
{
  (graphList+graphNum)->plot(I,curveNum,x,y,z);
}

/*!
  This function enables you to add new points in all curves of a plot. These points are drawn with the parameters of the curves.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param x : The coordinate of the new points along the x axis and given in the user unit system.
  \param v_y : y coordinates vector. The coordinates of the new points along the y axis and given in the user unit system.
  \param v_z : z coordinates vector. The coordinates of the new points along the z axis and given in the user unit system.
*/
void vpPlot::plot(const unsigned int graphNum, const double x, const vpColVector &v_y, const vpColVector &v_z)
{
	if((graphList+graphNum)->curveNbr == v_y.getRows() && (graphList+graphNum)->curveNbr == v_z.getRows())
	{
		for(unsigned int i = 0;i < v_y.getRows();++i)
			this->plot(graphNum, i, x, v_y[i], v_z[i]);
	}
	else
		vpTRACE("error in plot vector : not the right dimension");
}

/*!
  This method unable to move the point of view if you have a 3D graphic.
*/
void
vpPlot::navigate()
{
  vpImagePoint trash;
  vpMouseButton::vpMouseButtonType b = vpMouseButton::button1;
  
  bool blocked = false;
  unsigned int iblocked = 0;
  vpImagePoint iP;
  
  while (b != vpMouseButton::button3)
  {
    if (!blocked)
    {
      vpDisplay::getPointerPosition(I,iP);
      for (unsigned int i = 0; i < graphNbr ; i++)
      {
	if (iP.inRectangle((graphList+i)->graphZone))
	{
	  iblocked = i;
	  break;
	}
      }
      if ((graphList+iblocked)->move(I))
      {
	(graphList+iblocked)->replot3D(I);
      }
      
      blocked = (graphList+iblocked)->blocked;
    }
    else
    {
      if ((graphList+iblocked)->move(I))
      {
	(graphList+iblocked)->replot3D(I);
      }
      blocked = (graphList+iblocked)->blocked;
    }
  }
}

/*!
  This method displays the coordinates of the point pointed by the mouse pointer.
  
  \param block : If true, this method waits a click.
*/
void
vpPlot::getPixelValue(const bool block)
{
  vpImagePoint iP;
  
  if (block)
    vpDisplay::getClick(I,iP);
  else
    vpDisplay::getPointerPosition(I,iP);
  
  for (unsigned int i = 0; i < graphNbr; i++)
  {
    if ((graphList+i)->getPixelValue(I,iP)) break;
  }
}

/*!
  Sets the title of a graphic.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param title : The graphic title.
*/
void
vpPlot::setTitle (const unsigned int graphNum, const char *title)
{
  (graphList+graphNum)->setTitle(title);
}

/*!
  Sets the unit system of the x axis.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param unitx : The name of the unit of the x axis.
*/
void
vpPlot::setUnitX (const unsigned int graphNum, const char *unitx)
{
  (graphList+graphNum)->setUnitX(unitx);
}

/*!
  Sets the unit system of the y axis.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param unity : The name of the unit of the y axis.
*/
void
vpPlot::setUnitY (const unsigned int graphNum, const char *unity)
{
  (graphList+graphNum)->setUnitY(unity);
}

/*!
  Sets the unit system of the z axis.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param unitz : The name of the unit of the z axis.
*/
void
vpPlot::setUnitZ (const unsigned int graphNum, const char *unitz)
{
  (graphList+graphNum)->setUnitZ(unitz);
}

/*!
  Sets the legend of a curve.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param curveNum : The index of the curve in the list of the curves belonging to the graphic.
  \param legend : The legend of the curve.
*/
void
vpPlot::setLegend (const unsigned int graphNum, const unsigned int curveNum, const char *legend)
{
  (graphList+graphNum)->setLegend(curveNum, legend);
}

/*!
  This method enables to erase the list of points stored for the graphic number  \f$ graphNum \f$.
  
  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
*/
void 
vpPlot::resetPointList (const unsigned int graphNum)
{
  for (unsigned int i = 0; i < (graphList+graphNum)->curveNbr; i++)
    (graphList+graphNum)->resetPointList(i);
}

/*!
This function enables you to choose the thickness used to draw a given curve.
 
  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param curveNum : The index of the curve in the list of the curves belonging to the graphic.
  \param thickness : The thickness you want to use
*/
void
vpPlot::setThickness (const unsigned int graphNum, const unsigned int curveNum, const unsigned int thickness)
{
  (graphList+graphNum)->setCurveThickness(curveNum, thickness);
}

/*!
  This method enables to erase the list of points stored for the curve number \f$ curveNum \f$ contained in the  graphic number  \f$ graphNum \f$.
  
  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param curveNum : The index of the curve in the list of the curves belonging to the graphic.
*/
void 
vpPlot::resetPointList (const unsigned int graphNum, const unsigned int curveNum)
{
  (graphList+graphNum)->resetPointList(curveNum);
}

/*!
  This function enables to save in a text file all the plotted points of a graphic.

  The first line of the text file is the graphic title. Then the points coordinates are given. If the graphic has to curves: 
  - the first column corresponds to the x axis of the first curve
  - the second column corresponds to the y axis of the first curve
  - the third column corresponds to the z axis of the first curve
  - the fourth column corresponds to the x axis of the second curve
  - the fifth column corresponds to the y axis of the second curve
  - the sixth column corresponds to the z axis of the second curve

  The column are delimited thanks to tabultaions.

  \param graphNum : The index of the graph in the window. As the number of graphic in a window is less or equal to 4, this parameter is between 0 and 3.
  \param dataFile : Name of the text file.
*/
void vpPlot::saveData(const unsigned int graphNum, const char* dataFile)
{
  std::ofstream fichier;
  fichier.open(dataFile);

  unsigned int ind;
  double *p = new double[3];
  bool end=false;

  std::vector< std::list<double>::const_iterator > vec_iter_pointListx((graphList+graphNum)->curveNbr);
  std::vector< std::list<double>::const_iterator > vec_iter_pointListy((graphList+graphNum)->curveNbr);
  std::vector< std::list<double>::const_iterator > vec_iter_pointListz((graphList+graphNum)->curveNbr);

  fichier << (graphList+graphNum)->title << std::endl;

  for(ind=0;ind<(graphList+graphNum)->curveNbr;ind++)
  {
    vec_iter_pointListx[ind] = (graphList+graphNum)->curveList[ind].pointListx.begin();
    vec_iter_pointListy[ind] = (graphList+graphNum)->curveList[ind].pointListy.begin();
    vec_iter_pointListz[ind] = (graphList+graphNum)->curveList[ind].pointListz.begin();
//    (graphList+graphNum)->curveList[ind].pointListx.front();
//    (graphList+graphNum)->curveList[ind].pointListy.front();
//    (graphList+graphNum)->curveList[ind].pointListz.front();
  }

  while (end == false)
  {
    end = true;
    for(ind=0;ind<(graphList+graphNum)->curveNbr;ind++)
    {
//      if (!(graphList+graphNum)->curveList[ind].pointListx.outside()
//          && !(graphList+graphNum)->curveList[ind].pointListy.outside()
//          && !(graphList+graphNum)->curveList[ind].pointListz.outside())
      if((vec_iter_pointListx[ind] != (graphList+graphNum)->curveList[ind].pointListx.end())
        && (vec_iter_pointListy[ind] != (graphList+graphNum)->curveList[ind].pointListy.end())
        && (vec_iter_pointListz[ind] != (graphList+graphNum)->curveList[ind].pointListz.end()))
      {
        p[0] = *vec_iter_pointListx[ind];
        p[1] = *vec_iter_pointListy[ind];
        p[2] = *vec_iter_pointListz[ind];
//        p[0] = (graphList+graphNum)->curveList[ind].pointListx.value();
//        p[1] = (graphList+graphNum)->curveList[ind].pointListy.value();
//        p[2] = (graphList+graphNum)->curveList[ind].pointListz.value();

        fichier << p[0] << "\t" << p[1] << "\t" << p[2] << "\t";
        ++vec_iter_pointListx[ind];
        ++vec_iter_pointListy[ind];
        ++vec_iter_pointListz[ind];
//        (graphList+graphNum)->curveList[ind].pointListx.next();
//        (graphList+graphNum)->curveList[ind].pointListy.next();
//        (graphList+graphNum)->curveList[ind].pointListz.next();
//        if(!(graphList+graphNum)->curveList[ind].pointListx.nextOutside()
//           && !(graphList+graphNum)->curveList[ind].pointListy.nextOutside()
//           && !(graphList+graphNum)->curveList[ind].pointListz.nextOutside())
        if((vec_iter_pointListx[ind] != (graphList+graphNum)->curveList[ind].pointListx.end())
          && (vec_iter_pointListy[ind] != (graphList+graphNum)->curveList[ind].pointListy.end())
          && (vec_iter_pointListz[ind] != (graphList+graphNum)->curveList[ind].pointListz.end()))
          end = false;
      }
      else
      {
//        p[0] = (graphList+graphNum)->curveList[ind].pointListx.value();
//        p[1] = (graphList+graphNum)->curveList[ind].pointListy.value();
//        p[2] = (graphList+graphNum)->curveList[ind].pointListz.value();
        p[0] = (graphList+graphNum)->curveList[ind].pointListx.back();
        p[1] = (graphList+graphNum)->curveList[ind].pointListy.back();
        p[2] = (graphList+graphNum)->curveList[ind].pointListz.back();
        fichier << p[0] << "\t" << p[1] << "\t" << p[2] << "\t";
      }
    }
    fichier << std::endl;
  }

  delete[] p;
  fichier.close();
}

#endif
