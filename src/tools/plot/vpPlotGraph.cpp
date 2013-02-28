/****************************************************************************
 *
 * $Id: vpPlotGraph.cpp 3682 2012-04-11 14:28:29Z fnovotny $
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
 * Define a graph for the vpPlot class.
 *
 * Authors:
 * Nicolas Melchior
 *
 *****************************************************************************/
#define FLUSH_ON_PLOT

#include <visp/vpConfig.h>
#ifndef DOXYGEN_SHOULD_SKIP_THIS




#include <visp/vpPlotGraph.h>
#include <visp/vpMath.h>
#include <visp/vpMeterPixelConversion.h>
#include <visp/vpPixelMeterConversion.h>
#include <visp/vpPose.h>

#include <visp/vpDisplayOpenCV.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGDI.h>
#include <cmath>    // std::fabs
#include <visp/vpMath.h>
#include <limits>   // numeric_limits
#if defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined(VISP_HAVE_OPENCV) 

vpPlotGraph::vpPlotGraph()
{
  curveList = NULL;
  
  gridColor.setColor(200,200,200);
  
  nbDivisionx = 10;
  nbDivisiony = 10;
  nbDivisionz = 10;
  
  xmax = 10;
  xmin = 0;
  ymax = 10;
  ymin = -10;
  zmax = 10;
  zmin = -10;
  xdelt = 1;
  ydelt = 1;
  zdelt = 1;
  gridx = true;
  gridy = true;
  scaleInitialized = false;
  firstPoint = true;
  
  dispUnit = false;
  dispTitle = false;
  dispLegend = false;

  blockedr = false;
  blockedz = false;
  blocked = false;
  
  epsi = 5;
  epsj = 6;
  
  old_iPr = vpImagePoint(-1,-1);
  old_iPz = vpImagePoint(-1,-1);
}

vpPlotGraph::~vpPlotGraph()
{
  if (curveList != NULL)
  {
    delete[] curveList;
    curveList = NULL;
  }
}

void
vpPlotGraph::initGraph (unsigned int nbCurve)
{
  curveList = new vpPlotCurve[nbCurve];
  curveNbr = nbCurve;
  
  vpColor colors[6] = {vpColor::blue,vpColor::green,vpColor::red,vpColor::cyan,vpColor::orange,vpColor::purple};
  
  for (unsigned int i = 0; i < curveNbr; i++)
  {
    (curveList+i)->color = colors[i%6]; 
    (curveList+i)->curveStyle = line;
    (curveList+i)->pointListx.clear();
    (curveList+i)->pointListy.clear();
    strcpy((curveList+i)->legend,"");
  }
}

void
vpPlotGraph::initSize (vpImagePoint topLeft, unsigned int width, unsigned int height, unsigned int margei, unsigned int margej)
{
  this->topLeft = topLeft;
  this->width = width;
  this->height = height;
  graphZone.setTopLeft(topLeft);
  graphZone.setWidth(width);
  graphZone.setHeight(height);
  
  this->dTopLeft = vpImagePoint(topLeft.get_i()+margei,topLeft.get_j()+margej);
  this->dWidth = width-margej-10;
  this->dHeight = height-2*margei;
  dGraphZone.setTopLeft(dTopLeft);
  dGraphZone.setWidth(dWidth+1);
  dGraphZone.setHeight(dHeight+1);
  
  this->dTopLeft3D = vpImagePoint(topLeft.get_i()+margei,topLeft.get_j()+10);
  dGraphZone3D.setTopLeft(dTopLeft3D);
  dGraphZone3D.setWidth(dWidth+1);
  dGraphZone3D.setHeight(dHeight+1);
  
  if (this->dWidth > this->dHeight)
  {
    w_ysize = 1.0;
    w_xsize = this->dWidth/this->dHeight;
    w_zsize = w_xsize;
    
    w_yval = w_ysize/2.0;
    w_xval = w_xsize/2.0;
    w_zval = w_zsize/2.0;
  }
  else if (this->dWidth == this->dHeight)
  {
    w_ysize = 1.0;
    w_xsize = 1.0;
    w_zsize = 1.0;
    
    w_yval = 0.5;
    w_xval = 0.5;
    w_zval = 0.5;
  }
  else if (this->dWidth < this->dHeight)
  {
    w_xsize = 1.0;
    w_ysize = this->dHeight/this->dWidth;
    w_zsize = w_ysize;
    
    w_yval = w_ysize/2.0;
    w_xval = w_xsize/2.0;
    w_zval = w_zsize/2.0;
  }
  
  cam.initPersProjWithoutDistortion(1000,1000,this->dWidth/2.0,this->dHeight/2.0);
  
  findPose();
  
  cMf.buildFrom(0,0,cMo[2][3],0,0,0);
}


void
vpPlotGraph::findPose()
{
  vpPoint point[4];
  point[0].setWorldCoordinates(-w_xval,-w_yval,-w_zval);
  point[1].setWorldCoordinates(w_xval,-w_yval,-w_zval);
  point[2].setWorldCoordinates(w_xval,w_yval,-w_zval);
  point[3].setWorldCoordinates(-w_xval,w_yval,-w_zval);
  
  vpImagePoint iP[4];
  iP[0].set_ij(0,0);
  iP[1].set_ij(0,dWidth-1);
  iP[2].set_ij(dHeight-1,dWidth-1);
  iP[3].set_ij(dHeight-1,0);

  double x=0, y=0;
  vpPose pose;
  pose.clearPoint();

  for (unsigned int i=0 ; i < 4 ; i++)
  {
    vpPixelMeterConversion::convertPoint(cam, iP[i], x, y);
    point[i].set_x(x);
    point[i].set_y(y);
    pose.addPoint(point[i]);
  }
  
  pose.computePose(vpPose::LAGRANGE, cMo) ;
  pose.computePose(vpPose::VIRTUAL_VS, cMo);
}

void
vpPlotGraph::computeGraphParameters()
{
  zoomx = dWidth/(xmax-xmin);
  zoomy = dHeight/(ymax-ymin);
  xorg = dTopLeft.get_j() - (xmin*zoomx);
  yorg = dTopLeft.get_i() + (ymax*zoomy);
}

void 
vpPlotGraph::setCurveColor(const unsigned int curveNum, const vpColor color)
{
  (curveList+curveNum)->color = color;
}

void
vpPlotGraph::setTitle (const char *title)
{
  strcpy(this->title, title);
  dispTitle = true;
}

void
vpPlotGraph::setUnitX (const char *unitx)
{
  strcpy(this->unitx, unitx);
  dispUnit = true;
}

void
vpPlotGraph::setUnitY (const char *unity)
{
  strcpy(this->unity, unity);
  dispUnit = true;
}

void
vpPlotGraph::setUnitZ (const char *unitz)
{
  strcpy(this->unitz, unitz);
  dispUnit = true;
}

void
vpPlotGraph::setLegend (const unsigned int curveNum, const char *legend)
{
  strcpy((curveList+curveNum)->legend,legend);
  dispLegend = true;
}

void 
vpPlotGraph::setCurveThickness(const unsigned int curveNum, const unsigned int thickness)
{
  (curveList+curveNum)->thickness = thickness;
}

int
laFonctionSansNom (const double delta)
{
  double d = delta;
  int power = 0;
  if (d < 1)
  {
    while (d < 1)
    {
      d = d * 10;
      power++;
    }
    power--;
    return power;
  }
  
  if (d >= 10)
  {
    while (d > 10)
    {
      d = d / 10;
      power--;
    }
    power--;
    return power;
  }
  
  return 0;
}

void
vpPlotGraph::displayGrid (vpImage<unsigned char> &I)
{
  computeGraphParameters();
  
  xdelt = (xmax-xmin)/nbDivisionx;
  ydelt = (ymax-ymin)/nbDivisiony;
  
  double t;
  char valeur[20];
  int power;
  
  power = laFonctionSansNom(xdelt);
  for(t=xmin;t<=xmax;t=t+xdelt)
  {
    double x = xorg+(zoomx*t);
    if(gridy)
      vpDisplay::displayDotLine(I,vpImagePoint(dTopLeft.get_i(),x), vpImagePoint(dTopLeft.get_i()+dHeight,x), gridColor);
    else
      vpDisplay::displayDotLine(I,vpImagePoint(yorg,x), vpImagePoint(yorg-3,x), vpColor::black);    
    
    if (t+xdelt <= xmax+1e-10)
    {
      double ttemp;
      if (power!=0)
        ttemp = t*pow(10.0,power); 
      else ttemp = t;
      sprintf(valeur, "%.2f", ttemp);
#if defined VISP_HAVE_X11
      vpDisplay::displayCharString(I,vpImagePoint(yorg + 3*epsi,x),valeur, vpColor::black);
#elif defined (VISP_HAVE_GDI) || defined (VISP_HAVE_OPENCV)
      vpDisplay::displayCharString(I,vpImagePoint(yorg + epsi,x),valeur, vpColor::black);
#endif
    }
  }
  if (power != 0)
  {
    sprintf(valeur, "x10e%d", -power);
#if defined VISP_HAVE_X11
    vpDisplay::displayCharString(I,vpImagePoint(yorg+4*epsi,dTopLeft.get_j()+dWidth-6*epsj),valeur, vpColor::black);
#elif defined (VISP_HAVE_GDI) || defined (VISP_HAVE_OPENCV)
    vpDisplay::displayCharString(I,vpImagePoint(yorg+4*epsi,dTopLeft.get_j()+dWidth-10*epsj),valeur, vpColor::black);
#endif
  }
  
  power = laFonctionSansNom(ydelt);
  for(t=ymin;t<=ymax;t=t+ydelt)
  {
    double y = yorg-(zoomy*t);
    if(gridx)
      vpDisplay::displayDotLine(I,vpImagePoint(y,dTopLeft.get_j()), vpImagePoint(y,dTopLeft.get_j()+dWidth), gridColor);
    else
      vpDisplay::displayDotLine(I,vpImagePoint(y,xorg), vpImagePoint(y,xorg+3), vpColor::black);
      
    double ttemp;
    if (power!=0)
      ttemp = t*pow(10.0,power); 
    else ttemp = t;
      
    sprintf(valeur, "%.2f", ttemp);
#if defined VISP_HAVE_X11    
    vpDisplay::displayCharString(I,vpImagePoint(y+epsi,topLeft.get_j()+epsj),valeur, vpColor::black);
#elif defined (VISP_HAVE_GDI) || defined (VISP_HAVE_OPENCV)
    vpDisplay::displayCharString(I,vpImagePoint(y-epsi,topLeft.get_j()+epsj),valeur, vpColor::black);
#endif
  }
  if (power != 0)
  {
    sprintf(valeur, "x10e%d", -power);
#if defined VISP_HAVE_X11   
    vpDisplay::displayCharString(I,vpImagePoint(dTopLeft.get_i()-3*epsi,dTopLeft.get_j()-6*epsj),valeur, vpColor::black);
#elif defined (VISP_HAVE_GDI) || defined (VISP_HAVE_OPENCV)
    vpDisplay::displayCharString(I,vpImagePoint(dTopLeft.get_i()-3*epsi,dTopLeft.get_j()-6*epsj),valeur, vpColor::black);
#endif
  }

//Ligne horizontal
  vpDisplay::displayArrow(I,vpImagePoint(yorg,dTopLeft.get_j()), vpImagePoint(yorg,dTopLeft.get_j()+dWidth), vpColor::black);
//Ligne vertical
  vpDisplay::displayArrow(I, vpImagePoint(dTopLeft.get_i()+dHeight,xorg), vpImagePoint(dTopLeft.get_i(),xorg), vpColor::black);
  
  if (dispUnit)
    displayUnit(I);
  if (dispTitle)
  displayTitle(I);
  if (dispLegend)
    displayLegend(I);
  
  //vpDisplay::flushROI(I,graphZone);
}

void
vpPlotGraph::displayUnit (vpImage<unsigned char> &
#if defined(VISP_HAVE_X11) || defined (VISP_HAVE_GDI) || defined(VISP_HAVE_OPENCV)
                          I
#endif
                          )
{ 
	unsigned int offsetx = vpMath::minimum<unsigned int>((unsigned int)strlen(unitx), dWidth);

#if defined VISP_HAVE_X11   
  vpDisplay::displayCharString(I,vpImagePoint(yorg-2*epsi,dTopLeft.get_j()+dWidth-offsetx*epsj),unitx, vpColor::black);
  vpDisplay::displayCharString(I,vpImagePoint(dTopLeft.get_i(),dTopLeft.get_j()+epsj),unity, vpColor::black);
#elif defined (VISP_HAVE_GDI) || defined (VISP_HAVE_OPENCV)
  vpDisplay::displayCharString(I,vpImagePoint(yorg-5*epsi,dTopLeft.get_j()+dWidth-offsetx*epsj),unitx, vpColor::black);
  vpDisplay::displayCharString(I,vpImagePoint(dTopLeft.get_i(),dTopLeft.get_j()+epsj),unity, vpColor::black);
#endif
}

void
vpPlotGraph::displayTitle (vpImage<unsigned char> &I)
{
  double size = (double)strlen(title);
  size = size/2.0;
  vpDisplay::displayCharString(I,
			       vpImagePoint(dTopLeft.get_i()-3*epsi,
					    dTopLeft.get_j()+dWidth/2.0-4*size),
			       title, 
			       vpColor::black);
}

void
vpPlotGraph::displayLegend (vpImage<unsigned char> &I)
{
  unsigned int offsetj = 0;
  for (unsigned int i = 0; i < curveNbr; i++) {
    unsigned int offset = epsj * strlen((curveList+i)->legend);
    offsetj = vpMath::maximum(offset, offsetj);
  }
  if (offsetj > dWidth) offsetj = dWidth;

  for (unsigned int i = 0; i < curveNbr; i++) {
    vpDisplay::displayCharString(I,
                                 vpImagePoint(dTopLeft.get_i()+i*5*epsi,
                                              dTopLeft.get_j()+dWidth-offsetj),
                                 (curveList+i)->legend,
                                 (curveList+i)->color);
  }
}

void
vpPlotGraph::rescalex(unsigned int side, double extremity)
{
  switch (side)
  {
    case 0:
      xmin = (3*extremity-xmax)/2;
      break;
    case 1:
      xmax = (3*extremity-xmin)/2;
      break;
  }
  
  xdelt = (xmax-xmin)/(double)nbDivisionx;
}

void
vpPlotGraph::rescaley(unsigned int side, double extremity)
{
  switch (side)
  {
    case 0:
      ymin = (3*extremity-ymax)/2;
      break;
    case 1:
      ymax = (3*extremity-ymin)/2;
      break;
  }
  
  ydelt = (ymax-ymin)/(double)nbDivisiony;
}

void
vpPlotGraph::initScale(vpImage<unsigned char> &I, const double xmin, const double xmax, const int nbDivx, const double ymin, const double ymax, const int nbDivy, const bool gx, const bool gy)
{
  this->xmin = xmin;
  this->xmax = xmax;
  this->ymin = ymin;
  this->ymax = ymax;
  this->gridx = gx;
  this->gridy = gy;
  this->nbDivisionx = nbDivx;
  this->nbDivisiony = nbDivy;
  computeGraphParameters();
  clearGraphZone(I);
  displayGrid(I);
  vpDisplay::flushROI(I,graphZone);
  scaleInitialized = true;
}


void
vpPlotGraph::initScale(vpImage<unsigned char> &I, const double xmin, const double xmax, const int nbDivx, const double ymin, const double ymax, const int nbDivy, const double zmin, const double zmax, const int nbDivz, const bool gx, const bool gy)
{
  this->xmin = xmin;
  this->xmax = xmax;
  this->ymin = ymin;
  this->ymax = ymax;
  this->zmin = zmin;
  this->zmax = zmax;
  this->gridx = gx;
  this->gridy = gy;
  this->nbDivisionx = nbDivx;
  this->nbDivisiony = nbDivy;
  this->nbDivisionz = nbDivz;
  computeGraphParameters();
  clearGraphZone(I);
  displayGrid(I);
  vpDisplay::flushROI(I,graphZone);
  scaleInitialized = true;
}

void
vpPlotGraph::plot (vpImage<unsigned char> &I, const unsigned int curveNb, const double x, const double y)
{
  if (!scaleInitialized)
  {
    if (x < 0)
    {
      xmax = 0;
      rescalex(0,x);
    }
    if (x > 0)
    {
      xmin = 0;
      rescalex(1,x);
    }
    if (y < 0) 
    {      
      ymax = 0;
      rescaley(0,y);
    }
    if (y > 0)
    {
      ymin = 0;
      rescaley(1,y);
    }
    scaleInitialized = true;
    computeGraphParameters();
    clearGraphZone(I);
    displayGrid(I);
    //if (y == 0)
    if (std::fabs(y) <= std::numeric_limits<double>::epsilon())
      scaleInitialized = false;
  }
  
  if (firstPoint)
  {
//     clearGraphZone(I);
//     displayGrid(I);
//     vpDisplay::flushROI(I,graphZone);
    replot(I);
    firstPoint = false;
  }
  
  double i = yorg-(zoomy*y);
  double j = xorg+(zoomx*x);
  
  vpImagePoint iP(i,j);
  
  if (!iP.inRectangle(dGraphZone))
  {
    if (x > xmax) rescalex(1,x);
    else if(x < xmin) rescalex(0,x);
    
    if (y > ymax) rescaley(1,y);
    else if(y < ymin) rescaley(0,y);
    
    computeGraphParameters();
    
    replot(I);
    i = yorg-(zoomy*y);
    j = xorg+(zoomx*x);
    
    iP.set_ij(i,j);
  }
  
  (curveList+curveNb)->plotPoint(I, iP, x, y);
#if (!defined VISP_HAVE_X11 && defined FLUSH_ON_PLOT)
  vpDisplay::flushROI(I,graphZone);
  //vpDisplay::flush(I);
#endif
}

void
vpPlotGraph::replot (vpImage<unsigned char> &I)
{
  clearGraphZone(I);
  displayGrid(I);
  for (unsigned int i = 0; i < curveNbr; i++)
    (curveList+i)->plotList(I,xorg,yorg,zoomx,zoomy);
  vpDisplay::flushROI(I,graphZone);
}

void
vpPlotGraph::clearGraphZone(vpImage<unsigned char> &I)
{
  vpDisplay::displayROI(I,graphZone);
}

bool
vpPlotGraph::getPixelValue(vpImage<unsigned char> &I, vpImagePoint &iP)
{
  if (iP.inRectangle(dGraphZone))
  {
    double x = (iP.get_j()-xorg)/zoomx;
    double y = (yorg-iP.get_i())/zoomy;
    
    vpDisplay::displayROI(I,vpRect(vpImagePoint(topLeft.get_i()+height-20,topLeft.get_j()),width-1,19));
    char valeur[200];
    sprintf(valeur, " x: %f", x);
    vpDisplay::displayCharString(I,vpImagePoint(topLeft.get_i()+height-2,topLeft.get_j()+5*epsj),valeur, vpColor::black);
    sprintf(valeur, " y: %f", y);
    vpDisplay::displayCharString(I,vpImagePoint(topLeft.get_i()+height-2,topLeft.get_j()+width/2.0),valeur, vpColor::black);
//     vpDisplay::flush(I);
    vpDisplay::flushROI(I,vpRect(vpImagePoint(topLeft.get_i()+height-20,topLeft.get_j()),width-1,19));
    return true;
  }
  return false;
}

void 
vpPlotGraph::resetPointList(const unsigned int curveNum)
{
  (curveList+curveNum)->pointListx.clear();
  (curveList+curveNum)->pointListy.clear();
  (curveList+curveNum)->pointListz.clear();
  (curveList+curveNum)->nbPoint = 0;
  firstPoint = true;
}


/************************************************************************************************/

bool
vpPlotGraph::check3Dline(vpImagePoint &iP1, vpImagePoint &iP2)
{
  bool iP1In = iP1.inRectangle(dGraphZone3D);
  bool iP2In = iP2.inRectangle(dGraphZone3D);
  
  if (!iP1In || !iP2In)
  {
    double dTopLeft_i = dTopLeft3D.get_i();
    double dTopLeft_j = dTopLeft3D.get_j();
    double dBottomRight_i = dTopLeft_i+dHeight;
    double dBottomRight_j = dTopLeft_j+dWidth;
    
    //Cas vertical
    if (vpImagePoint::distance(iP1,iP2) < 9)
      return false;
    if (fabs(iP2.get_j()-iP1.get_j()) <=2)
    {
      if (!iP1In && !iP2In)
      {
	if (iP1.get_i() < dTopLeft_i && iP2.get_i() < dTopLeft_i)
	  return false;
	if (iP1.get_i() > dBottomRight_i && iP2.get_i() > dBottomRight_i)
	  return false;
	if (iP1.get_j() < dTopLeft_j || iP1.get_j() > dBottomRight_j)
	  return false;
	if (iP1.get_i() < dTopLeft_i) iP1.set_i(dTopLeft_i);
	else iP1.set_i(dBottomRight_i);
	if (iP2.get_i() < dTopLeft_i) iP2.set_i(dTopLeft_i);
	else iP2.set_i(dBottomRight_i);
      }
      else if (!iP1In)
      {
	if (iP1.get_j() < dTopLeft_j) iP1.set_j(dTopLeft_j);
	if (iP1.get_j() > dBottomRight_j) iP1.set_j(dBottomRight_j);
	if (iP1.get_i() < dTopLeft_i) iP1.set_i(dTopLeft_i);
	if (iP1.get_i() > dBottomRight_i) iP1.set_i(dBottomRight_i);
	return true;
      }
      else if (!iP2In)
      {
	if (iP2.get_j() < dTopLeft_j) iP2.set_j(dTopLeft_j);
	if (iP2.get_j() > dBottomRight_j) iP2.set_j(dBottomRight_j);
	if (iP2.get_i() < dTopLeft_i) iP2.set_i(dTopLeft_i);
	if (iP2.get_i() > dBottomRight_i) iP2.set_i(dBottomRight_i);
	return true;
      }
    }
    //cas horizontal
    else if (fabs(iP2.get_i()-iP1.get_i()) <= 2)
    {
      if (!iP1In && !iP2In)
      {
	if (iP1.get_j() < dTopLeft_j && iP2.get_j() < dTopLeft_j)
	  return false;
	if (iP1.get_j() > dBottomRight_j && iP2.get_j() > dBottomRight_j)
	  return false;
	if (iP1.get_i() < dTopLeft_i || iP1.get_i() > dBottomRight_i)
	  return false;
	if (iP1.get_j() < dTopLeft_j) iP1.set_j(dTopLeft_j);
	else iP1.set_j(dBottomRight_j);
	if (iP2.get_j() < dTopLeft_j) iP2.set_j(dTopLeft_j);
	else iP2.set_j(dBottomRight_j);
      }
      else if (!iP1In)
      {
	if (iP1.get_j() < dTopLeft_j) iP1.set_j(dTopLeft_j);
	if (iP1.get_j() > dBottomRight_j) iP1.set_j(dBottomRight_j);
	if (iP1.get_i() < dTopLeft_i) iP1.set_i(dTopLeft_i);
	if (iP1.get_i() > dBottomRight_i) iP1.set_i(dBottomRight_i);
	return true;
      }
      else if (!iP2In)
      {
	if (iP2.get_j() < dTopLeft_j) iP2.set_j(dTopLeft_j);
	if (iP2.get_j() > dBottomRight_j) iP2.set_j(dBottomRight_j);
	if (iP2.get_i() < dTopLeft_i) iP2.set_i(dTopLeft_i);
	if (iP2.get_i() > dBottomRight_i) iP2.set_i(dBottomRight_i);
	return true;
      }
    }
    
    double a = (iP2.get_i()-iP1.get_i())/(iP2.get_j()-iP1.get_j());
    double b = iP1.get_i()-a*iP1.get_j();

    //test horizontal
    double jtop = (dTopLeft_i-b)/a;
    double jlow = (dBottomRight_i-b)/a;
    //test vertical
    double ileft = dTopLeft_j*a+b;
    double iright = (dBottomRight_j)*a+b;
    
    vpImagePoint iP[2];
    int n = 0;
    
    if(jtop >= dTopLeft_j && jtop <= dBottomRight_j)
    {
      iP[n].set_ij(dTopLeft_i,jtop);
      n++;
    }
    if(jlow >= dTopLeft_j && jlow <= dBottomRight_j)
    {
      iP[n].set_ij(dBottomRight_i,jlow);
      n++;
    }
    if(ileft >= dTopLeft_i && ileft <= dBottomRight_i && n <2)
    {
      iP[n].set_ij(ileft,dTopLeft_j);
      n++;
    }
    if(iright >= dTopLeft_i && iright <= dBottomRight_i && n <2)
    {
      iP[n].set_ij(iright,dBottomRight_j);
      n++;
    }
    
    if (n < 2)
      return false;
    
    if (!iP1In && !iP2In)
    {
      if (fabs(a) < 1)
      {
	if (vpMath::sign(iP1.get_j()-iP[0].get_j()) == vpMath::sign(iP2.get_j()-iP[0].get_j()))
	  return false;
        int sign = vpMath::sign(iP1.get_j() - iP2.get_j());
	if (sign == vpMath::sign(iP[0].get_j()-iP[1].get_j())) 
	{
	  iP1 = iP[0]; iP2 = iP[1];
	}
	else
	{
	  iP1 = iP[1]; iP2 = iP[0];
	}
      }
      else
      {
	if (vpMath::sign(iP1.get_i()-iP[0].get_i()) == vpMath::sign(iP2.get_i()-iP[0].get_i()))
	  return false;
        int sign = vpMath::sign(iP1.get_i() - iP2.get_i());
	if (sign == vpMath::sign(iP[0].get_i()-iP[1].get_i()))
	{
	  iP1 = iP[0];iP2 = iP[1];
	}
	else
	{
	  iP1 = iP[1];iP2 = iP[0];
	}
      }
    }
    else if (!iP1In)
    {
      vpImagePoint iPtemp = iP1;
      if (fabs(a) < 1)
      {
        int sign = vpMath::sign(iP1.get_j() - iP2.get_j());
	if (fabs(iP[0].get_j()-iP2.get_j()) > 5)
	{
	  if (sign == vpMath::sign(iP[0].get_j()-iP2.get_j())) iP1 = iP[0];
	  else iP1 = iP[1];
	}
	else
	{
	  if (sign == vpMath::sign(iP[1].get_j()-iP2.get_j())) iP1 = iP[1];
	  else iP1 = iP[0];
	}
      }
      else
      {
        int sign = vpMath::sign(iP1.get_i() - iP2.get_i());
	if (fabs(iP[0].get_i()-iP2.get_i()) > 5)
	{
	  if (sign == vpMath::sign(iP[0].get_i()-iP2.get_i())) iP1 = iP[0];
	  else iP1 = iP[1];
	}
	else
	{
	  if (sign == vpMath::sign(iP[1].get_i()-iP2.get_i())) iP1 = iP[1];
	  else iP1 = iP[0];
	}
      }
      if (vpImagePoint::distance(iP1,iP2) < 9)
      {
	iP1 = iPtemp;
        return false;
      }
    }
    else if (!iP2In)
    {
      vpImagePoint iPtemp = iP2;
      if (fabs(a) < 1)
      {
        int sign = vpMath::sign(iP2.get_j() - iP1.get_j());
	if (fabs(iP[0].get_j()-iP1.get_j()) > 5)
	{
	  if (sign == vpMath::sign(iP[0].get_j()-iP1.get_j())) iP2 = iP[0];
	  else iP2 = iP[1];
	}
	else
	{
	  if (sign == vpMath::sign(iP[1].get_j()-iP1.get_j())) iP2 = iP[1];
	  else iP2 = iP[0];
	}
      }
      else
      {
        int sign = vpMath::sign(iP2.get_i() - iP1.get_i());
	if (fabs(iP[0].get_i()-iP1.get_i()) > 5)
	{
	  if (sign == vpMath::sign(iP[0].get_i()-iP1.get_i())) iP2 = iP[0];
	  else iP2 = iP[1];
	}
	else
	{
	  if (sign == vpMath::sign(iP[1].get_i()-iP1.get_i())) iP2 = iP[1];
	  else iP2 = iP[0];
	}  
      }
      if (vpImagePoint::distance(iP1,iP2) < 9)
      {
	iP2 = iPtemp;
        return false;
      }
    }
  }
  return true;
}

bool 
vpPlotGraph::check3Dpoint(vpImagePoint &iP)
{ 
  if (!iP.inRectangle(dGraphZone3D))
  {
    if (iP.get_i() < dTopLeft3D.get_i())
      iP.set_i(dTopLeft3D.get_i());
    else if (iP.get_i() > dTopLeft3D.get_i()+dHeight)
      iP.set_i(dTopLeft3D.get_i()+dHeight-1);
    if (iP.get_j() <dTopLeft3D.get_j())
      iP.set_j(dTopLeft3D.get_j());
    else if (iP.get_j() > dTopLeft3D.get_j()+dWidth)
      iP.set_i(dTopLeft3D.get_j()+dWidth-1);
    return false;
  }
  return true;
}

void
vpPlotGraph::computeGraphParameters3D()
{
  zoomx_3D = w_xsize/(xmax-xmin);
  zoomy_3D = w_ysize/(ymax-ymin);
  zoomz_3D = w_zsize/(zmax-zmin);
  ptXorg = w_xval - zoomx_3D*xmax;
  ptYorg = w_yval + zoomy_3D*ymin;
  ptZorg = w_zval - zoomz_3D*zmax;
}

void getGrid3DPoint(const double pente, vpImagePoint &iPunit, vpImagePoint &ip1, vpImagePoint &ip2, vpImagePoint &ip3)
{
  if (pente <= 1)
    {
      ip1 = iPunit-vpImagePoint(3,0);
      ip2 = iPunit+vpImagePoint(3,0);
      ip3 = iPunit-vpImagePoint(6,6);
    }
    else 
    {
      ip1 = iPunit-vpImagePoint(0,3);
      ip2 = iPunit+vpImagePoint(0,3);
      ip3 = iPunit+vpImagePoint(6,6);
    }
}

void
vpPlotGraph::displayGrid3D (vpImage<unsigned char> &I)
{
  computeGraphParameters3D();
  
  xdelt = (xmax-xmin)/nbDivisionx;
  ydelt = (ymax-ymin)/nbDivisiony;
  zdelt = (zmax-zmin)/nbDivisionz;
  
  vpPoint pt[6];
  pt[0].setWorldCoordinates(-w_xval,ptYorg,ptZorg);
  pt[1].setWorldCoordinates(w_xval,ptYorg,ptZorg);
  pt[2].setWorldCoordinates(ptXorg,-w_yval,ptZorg);
  pt[3].setWorldCoordinates(ptXorg,w_yval,ptZorg);
  pt[4].setWorldCoordinates(ptXorg,ptYorg,-w_zval);
  pt[5].setWorldCoordinates(ptXorg,ptYorg,w_zval);
  
  vpImagePoint iP[6];
  for (unsigned int i = 0; i < 6; i++)
  {
    pt[i].track(cMo);
    double u=0.0,v=0.0;
    vpMeterPixelConversion::convertPoint(cam,pt[i].get_x(),pt[i].get_y(),u,v);
    iP[i].set_uv(u,v);
    iP[i] = iP[i] + dTopLeft3D;
  }
  
  int power;
  double t;
  char valeur[20];
  vpPoint ptunit;
  vpImagePoint iPunit;
  double pente;
  vpImagePoint ip1;
  vpImagePoint ip2;
  vpImagePoint ip3;
  vpImagePoint ip4;
  
  power = laFonctionSansNom(xdelt);
  ptunit.setWorldCoordinates(-w_xval,ptYorg,ptZorg);
  //if (iP[0].get_j()-iP[1].get_j() != 0) 
  if (std::fabs(iP[0].get_j()-iP[1].get_j()) > 
      vpMath::maximum(std::fabs(iP[0].get_j()), std::fabs(iP[1].get_j()))* std::numeric_limits<double>::epsilon())
    pente = fabs((iP[0].get_i()-iP[1].get_i())/(iP[0].get_j()-iP[1].get_j()));
  else pente = 2;
  
  unsigned int count = 1;
  for(t=xmin;t<=xmax;t=t+xdelt)
  {
    double x = ptXorg+(zoomx_3D*t);
    ptunit.set_oX(x);
    ptunit.track(cMo);
    double u=0.0, v=0.0;
    vpMeterPixelConversion::convertPoint(cam,ptunit.get_x(),ptunit.get_y(),u,v);
    iPunit.set_uv(u,v);
    iPunit = iPunit + dTopLeft3D;
    
    getGrid3DPoint(pente,iPunit,ip1,ip2,ip3);
    
    if(check3Dline(ip1,ip2))
    {
      vpDisplay::displayLine(I,ip2, ip1, vpColor::black);
      if (count%2 == 1)
      {
        double ttemp;
        if (power!=0)
          ttemp = t*pow(10.0,power); 
        else ttemp = t;
        sprintf(valeur, "%.1f", ttemp);
        vpDisplay::displayCharString(I,ip3,valeur, vpColor::black);
      }
    }
    count++;
  }
  if (power != 0)
  {
    ip4 = iP[1] -vpImagePoint(-15,10);
    sprintf(valeur, "x10e%d", -power);
    if(check3Dpoint(ip4))
      vpDisplay::displayCharString(I,ip4,valeur, vpColor::black);
  }
  
  power = laFonctionSansNom(ydelt);
  ptunit.setWorldCoordinates(ptXorg,-w_yval,ptZorg);
  //if (iP[2].get_j()-iP[3].get_j() != 0) 
  if (std::fabs(iP[2].get_j()-iP[3].get_j()) > 
      vpMath::maximum(std::fabs(iP[2].get_j()), std::fabs(iP[3].get_j()))* std::numeric_limits<double>::epsilon())
    pente = fabs((iP[2].get_i()-iP[3].get_i())/(iP[2].get_j()-iP[3].get_j()));
  else pente = 2;
  count = 0;
  for(t=ymin;t<=ymax;t=t+ydelt)
  {
    double y = ptYorg-(zoomy_3D*t);
    ptunit.set_oY(y);
    ptunit.track(cMo);
    double u=0.0, v=0.0;
    vpMeterPixelConversion::convertPoint(cam,ptunit.get_x(),ptunit.get_y(),u,v);
    iPunit.set_uv(u,v);
    iPunit = iPunit + dTopLeft3D;
    
    getGrid3DPoint(pente,iPunit,ip1,ip2,ip3);
    
    if(check3Dline(ip1,ip2))
    {
      vpDisplay::displayLine(I,ip1, ip2, vpColor::black);
      if (count%2 == 1)
      {
        double ttemp;
        if (power!=0)
          ttemp = t*pow(10.0,power); 
        else ttemp = t;
        sprintf(valeur, "%.1f", ttemp);
        vpDisplay::displayCharString(I,ip3,valeur, vpColor::black);
      }
    }
    count++;
  }
  if (power != 0)
  {
    ip4 = iP[2] -vpImagePoint(-15,10);
    sprintf(valeur, "x10e%d", -power);
    if(check3Dpoint(ip4))
    vpDisplay::displayCharString(I,ip4,valeur, vpColor::black);
  }
  
  power = laFonctionSansNom(zdelt);
  ptunit.setWorldCoordinates(ptXorg,ptYorg,-w_zval);
  //if (iP[4].get_j()-iP[5].get_j() != 0) 
  if (std::fabs(iP[4].get_j()-iP[5].get_j()) > 
      vpMath::maximum(std::fabs(iP[4].get_j()), std::fabs(iP[5].get_j()))* std::numeric_limits<double>::epsilon())
    pente = fabs((iP[4].get_i()-iP[5].get_i())/(iP[4].get_j()-iP[5].get_j()));
  else pente = 2;
  count = 0;
  for(t=zmin;t<=zmax;t=t+zdelt)
  {
    double z = ptZorg+(zoomz_3D*t);
    ptunit.set_oZ(z);
    ptunit.track(cMo);
    double u=0.0, v=0.0;
    vpMeterPixelConversion::convertPoint(cam,ptunit.get_x(),ptunit.get_y(),u,v);
    iPunit.set_uv(u,v);
    iPunit = iPunit + dTopLeft3D;
 
    getGrid3DPoint(pente,iPunit,ip1,ip2,ip3);
    
    if(check3Dline(ip1,ip2))
    {
      vpDisplay::displayLine(I,ip1, ip2, vpColor::black);
      if (count%2 == 1)
      {
        double ttemp;
        if (power!=0)
          ttemp = t*pow(10.0,power); 
        else ttemp = t;
        sprintf(valeur, "%.1f", ttemp);
        vpDisplay::displayCharString(I,ip3,valeur, vpColor::black);
      }
    }
    count++;
  }
  if (power != 0)
  {
    ip4 = iP[5] -vpImagePoint(-15,10);
    sprintf(valeur, "x10e%d", -power);
    if(check3Dpoint(ip4))
      vpDisplay::displayCharString(I,ip4,valeur, vpColor::black);
  }
  
  
//Ligne horizontal
  if (check3Dline(iP[0],iP[1]))
  {
    vpDisplay::displayArrow(I,iP[0],iP[1], vpColor::black);
    if (dispUnit)
    {
      vpImagePoint iPunit(iP[1].get_i(),iP[1].get_j()-10*epsj);
      check3Dpoint (iPunit);
      vpDisplay::displayCharString(I,iPunit,unitx, vpColor::black);
    }
  }
  if (check3Dline(iP[3],iP[2]))
  {
    vpDisplay::displayArrow(I,iP[3],iP[2], vpColor::black);
    if (dispUnit)
    {
      vpImagePoint iPunit(iP[2].get_i(),iP[2].get_j()-10*epsj);
      check3Dpoint (iPunit);
      vpDisplay::displayCharString(I,iPunit,unity, vpColor::black);
    }
  }
  if (check3Dline(iP[4],iP[5]))
  {
    vpDisplay::displayArrow(I,iP[4],iP[5], vpColor::black);
    if (dispUnit)
    {
      vpImagePoint iPunit(iP[5].get_i(),iP[5].get_j()-10*epsj);
      check3Dpoint (iPunit);
      vpDisplay::displayCharString(I,iPunit,unitz, vpColor::black);
    }
  }
  
  if (dispTitle)
    displayTitle(I);
  if (dispLegend)
    displayLegend(I);
}

void
vpPlotGraph::plot (vpImage<unsigned char> &I, const unsigned int curveNb, const double x, const double y, const double z)
{
  if (!scaleInitialized)
  {
    if (x < 0)
    {
      xmax = 0;
      rescalex(0,x);
    }
    if (x > 0)
    {
      xmin = 0;
      rescalex(1,x);
    }
    if (y < 0) 
    {      
      ymax = 0;
      rescaley(0,y);
    }
    if (y > 0)
    {
      ymin = 0;
      rescaley(1,y);
    }
    if (z < 0) 
    {      
      zmax = 0;
      rescalez(0,z);
    }
    if (z > 0)
    {
      zmin = 0;
      rescalez(1,z);
    }
    scaleInitialized = true;
    computeGraphParameters3D();
    clearGraphZone(I);
    displayGrid3D(I);
    //if (std::fabs(y) == 0 || z == 0)
    if (std::fabs(y) <= std::numeric_limits<double>::epsilon()  || std::fabs(z) <= std::numeric_limits<double>::epsilon())
      scaleInitialized = false;
  }
  
  if (firstPoint)
  {
    clearGraphZone(I);
    displayGrid3D(I);
    vpDisplay::flushROI(I,graphZone);
    firstPoint = false;
  }
  
  bool changed = false;
  if (x > xmax) {rescalex(1,x); changed = true;}
  else if(x < xmin) {rescalex(0,x);changed = true;}
    
  if (y > ymax) {rescaley(1,y);changed = true;}
  else if(y < ymin) {rescaley(0,y);changed = true;}
  
  if (z > zmax) {rescalez(1,z);changed = true;}
  else if(z < zmin) {rescalez(0,z);changed = true;}
  
  if (changed || move(I))
  {
    computeGraphParameters3D();
    replot3D(I);
  }

  vpPoint pointPlot;
  pointPlot.setWorldCoordinates(ptXorg+(zoomx_3D*x),ptYorg-(zoomy_3D*y),ptZorg+(zoomz_3D*z));
  pointPlot.track(cMo);
  double u=0.0, v=0.0;
  vpMeterPixelConversion::convertPoint(cam,pointPlot.get_x(),pointPlot.get_y(),u,v);
  vpImagePoint iP;
  iP.set_uv(u,v);
  iP = iP + dTopLeft3D;
  
  (curveList+curveNb)->pointListx.end();
  (curveList+curveNb)->pointListy.end();
  (curveList+curveNb)->pointListz.end();
  if((curveList+curveNb)->nbPoint)
  {
    if (check3Dline((curveList+curveNb)->lastPoint,iP))
      vpDisplay::displayLine(I,(curveList+curveNb)->lastPoint, iP, (curveList+curveNb)->color, (curveList+curveNb)->thickness);
  }
#if( defined VISP_HAVE_X11 || defined VISP_HAVE_GDI )
  double top;
  double left;
  double width;
  double height;
  
  if (iP.get_i() <= (curveList+curveNb)->lastPoint.get_i()) {top = iP.get_i()-5; height = (curveList+curveNb)->lastPoint.get_i() - top+10;}
  else {top = (curveList+curveNb)->lastPoint.get_i()-5; height = iP.get_i() - top+10;}
  if (iP.get_j() <= (curveList+curveNb)->lastPoint.get_j()) {left = iP.get_j()-5; width = (curveList+curveNb)->lastPoint.get_j() - left+10;}
  else {left = (curveList+curveNb)->lastPoint.get_j()-5; width = iP.get_j() - left+10;}
  vpDisplay::flushROI(I,vpRect(left,top,width,height));
#endif
  
  (curveList+curveNb)->lastPoint = iP;
  (curveList+curveNb)->pointListx.push_back(x);
  (curveList+curveNb)->pointListy.push_back(y);
  (curveList+curveNb)->pointListz.push_back(z);
  (curveList+curveNb)->nbPoint++;
  
  
#if( !defined VISP_HAVE_X11 && defined FLUSH_ON_PLOT)  
  vpDisplay::flushROI(I,graphZone);
#endif
}

void
vpPlotGraph::replot3D (vpImage<unsigned char> &I)
{
  clearGraphZone(I);
  displayGrid3D(I);
  
  for (unsigned int i = 0; i < curveNbr; i++)
  {
    std::list<double>::const_iterator it_ptListx = (curveList+i)->pointListx.begin();
    std::list<double>::const_iterator it_ptListy = (curveList+i)->pointListy.begin();
    std::list<double>::const_iterator it_ptListz = (curveList+i)->pointListz.begin();
  
    unsigned int k = 0;
    vpImagePoint iP;
    vpPoint pointPlot;
    double x,y,z;
    while (k < (curveList+i)->nbPoint)
    {
      x = *it_ptListx;
      y = *it_ptListy;
      z = *it_ptListz;
      pointPlot.setWorldCoordinates(ptXorg+(zoomx_3D*x),ptYorg-(zoomy_3D*y),ptZorg+(zoomz_3D*z));
      pointPlot.track(cMo);
      double u=0.0, v=0.0;
      vpMeterPixelConversion::convertPoint(cam,pointPlot.get_x(),pointPlot.get_y(),u,v);
      iP.set_uv(u,v);
      iP = iP + dTopLeft3D;
    
      //vpDisplay::displayCross(I,iP,3,vpColor::cyan);
      if (k > 0)
      {
        if (check3Dline((curveList+i)->lastPoint,iP))
          vpDisplay::displayLine(I,(curveList+i)->lastPoint, iP, (curveList+i)->color);
        //vpDisplay::displayCross(I,iP,3,vpColor::orange);
      }
    
      (curveList+i)->lastPoint = iP;
    
      ++it_ptListx;
      ++it_ptListy;
      ++it_ptListz;
      k++;
    }
  }
  vpDisplay::flushROI(I,graphZone);
}



void
vpPlotGraph::rescalez(unsigned int side, double extremity)
{
  switch (side)
  {
    case 0:
      zmin = (3*extremity-zmax)/2;
      break;
    case 1:
      zmax = (3*extremity-zmin)/2;
      break;
  }
  
  zdelt = (zmax-zmin)/(double)nbDivisionz;
}

bool
vpPlotGraph::move(const vpImage<unsigned char> &I)
{
  bool changed = false;
  vpHomogeneousMatrix displacement = navigation(I,changed);
  
  //if (displacement[2][3] != 0)
  if (std::fabs(displacement[2][3]) > std::numeric_limits<double>::epsilon())
    cMf = cMf*displacement;
  vpHomogeneousMatrix fMo = cMf.inverse()*cMo;
  
  cMo = cMf* displacement * fMo;
  return changed;
}

vpHomogeneousMatrix
vpPlotGraph::navigation(const vpImage<unsigned char> &I, bool &changed)
{
  vpImagePoint iP;
  vpImagePoint trash;
  bool clicked = false;
  bool clickedUp = false;
  vpMouseButton::vpMouseButtonType b = vpMouseButton::button1;

  vpHomogeneousMatrix mov(0,0,0,0,0,0);
  changed = false;

  //if(!blocked) vpDisplay::getClickUp(I,trash, b,false);
  
  if(!blocked) clicked = vpDisplay::getClick(I,iP,b,false);

  if(blocked) clickedUp = vpDisplay::getClickUp(I,trash, b,false);
  
  if (clicked)
  {
    if (!iP.inRectangle(graphZone))
      return mov;
  }

  if(clicked)
  {
    if (b == vpMouseButton::button1) blockedr = true;
    if (b == vpMouseButton::button2) blockedz = true;
    blocked = true;
  }
  
  else if(clickedUp)
  {
    if (b == vpMouseButton::button1)
    {
      old_iPr = vpImagePoint(-1,-1);
      blockedr = false;
    }
    if (b == vpMouseButton::button2)
    {
      old_iPz = vpImagePoint(-1,-1);
      blockedz = false;
    }
    if (!(blockedr || blockedz))
    {
      blocked = false;
      //while (vpDisplay::getClick(I,trash,b,false)) {};
    }
  }
  
  vpDisplay::getPointerPosition(I,iP);

  double anglei = 0;
  double anglej = 0;

  if (old_iPr != vpImagePoint(-1,-1) && blockedr)
  {
    double width = vpMath::minimum(I.getWidth(),I.getHeight());
    
    double diffi = iP.get_i() - old_iPr.get_i();
    double diffj = iP.get_j() - old_iPr.get_j();
    
    anglei = diffi*360/width;
    anglej = diffj*360/width;
    mov.buildFrom(0,0,0,vpMath::rad(anglei),vpMath::rad(-anglej),0);
    changed = true;
  }

  if (blockedr) old_iPr = iP;

  if (old_iPz != vpImagePoint(-1,-1) && blockedz)
  {
    double diffi = iP.get_i() - old_iPz.get_i();
    mov.buildFrom(0,0,diffi*0.01,0,0,0);
    changed = true;
  }

  if (blockedz) old_iPz = iP;

  return mov;
}

#endif
#endif
