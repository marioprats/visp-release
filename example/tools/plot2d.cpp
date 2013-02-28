/****************************************************************************
 *
 * $Id: plot2d.cpp 3821 2012-06-27 13:50:37Z fspindle $
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
 * Example which describes how to use the vpPlot class
 *
 * Author:
 * Nicolas Melchior
 *
 *****************************************************************************/


/*!
  \example plot2d.cpp

  Plot 2D curves example.
*/

#include <visp/vpConfig.h>
#include <visp/vpDebug.h>

#include <visp/vpPlot.h>
#include <visp/vpMath.h>

#if defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined(VISP_HAVE_OPENCV) 

int main ()
{
  vpPlot plot(2, 700, 700, 100, 200, "Curves...");

  // Change the default font
  //  plot.setFont("-misc-fixed-bold-r-semicondensed--0-0-75-75-c-0-iso8859-10");

  //Initialize the number of curve for each graphic
  plot.initGraph(0,1);
  plot.initGraph(1,1);

  //Set the color of the curves
  plot.setColor(0,0,vpColor::green);
  plot.setColor(1,0,vpColor::red);

  //Set the titles of the graphic
  char title[40];
  strncpy( title, "cos function", 40 );
  plot.setTitle(0,title);
  strncpy( title, "sin function", 40 );
  plot.setTitle(1, title);

  //Set the legend of each curves
  char legend[40];
  strncpy( legend, "cos x", 40 );
  plot.setLegend(0,0,legend);
  strncpy( legend, "sin x", 40 );
  plot.setLegend(1,0, legend);

  //Set the x axis legend of each curves
  char unit[40];
  strncpy( unit, "x", 40 );
  plot.setUnitX(0,unit);
  strncpy( unit, "x", 40 );
  plot.setUnitX(1,unit);

  //Set the y axis legend of each curves
  strncpy( unit, "y", 40 );
  plot.setUnitY(0,unit);
  strncpy( unit, "y", 40 );
  plot.setUnitY(1,unit);

  //Plot the cosinus and sinus functions
  double i = 0;
  while(i <= 20*2*M_PI)
  {
    double co = cos(i); 
    double si = sin(i);
    plot.plot(0,0,i,co);
    plot.plot(1,0,i,si);
    i+=0.1;
  }
  
  vpDisplay::getClick(plot.I);
  
  //Save the datas as text files
  plot.saveData(0, "dataCos.txt");
  plot.saveData(0, "dataSin.txt");
}

#else
int main()
{
  std::cout << "Plot functionalities are not avalaible since no display is available." << std::endl;
  return 0;
}
#endif
