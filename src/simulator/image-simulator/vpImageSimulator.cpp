/****************************************************************************
 *
 * $Id: vpPose.h 2453 2010-01-07 10:01:10Z nmelchio $
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
 * Description: Class which enables to project an image in the 3D space 
 * and get the view of a virtual camera.
 * 
 *
 * Authors:
 * Amaury Dame
 * Nicolas Melchior
 *
 *****************************************************************************/

#include <visp/vpImageSimulator.h>
#include <visp/vpRotationMatrix.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageConvert.h>
#include <visp/vpPixelMeterConversion.h>
#include <visp/vpMeterPixelConversion.h>
#include <visp/vpMatrixException.h>

/*!
  Basic constructor.
  
  You can choose if you want to use a colored or gray scaled image.
  
  \param col : Enable to choose the color space to use for the image which is projected.
  
  By default the class uses colored images.
*/
vpImageSimulator::vpImageSimulator(const vpColorPlan &col)
{
  for(int i=0;i<4;i++)
    X[i].resize(3);

  for(int i=0;i<4;i++)
    X2[i].resize(3);

  normal_obj.resize(3);
  visible=false;
  normal_Cam.resize(3);

  //Xinter.resize(3);

  vbase_u.resize(3);
  vbase_v.resize(3);
  
  focal.resize(3);
  focal=0;
  focal[2]=1;

  normal_Cam_optim = new double[3];
  X0_2_optim = new double[3];
  vbase_u_optim = new double[3];
  vbase_v_optim = new double[3];
  Xinter_optim = new double[3];
  
  colorI = col;
  interp = SIMPLE;
  bgColor = vpColor::white;
  cleanPrevImage = false;
}


/*!
  Copy constructor
*/
vpImageSimulator::vpImageSimulator(const vpImageSimulator &text)
{
  for(int i=0;i<4;i++)
  {
    X[i] = text.X[i];
    pt[i] = text.pt[i];
  }

  for(int i=0;i<4;i++)
    X2[i].resize(3);
  
  Ic = text.Ic;
  Ig = text.Ig;
  
  focal.resize(3);
  focal=0;
  focal[2]=1;

  normal_obj = text.normal_obj;
  euclideanNorm_u = text.euclideanNorm_u;
  euclideanNorm_v = text.euclideanNorm_v;
  
  normal_Cam.resize(3);
  vbase_u.resize(3);
  vbase_v.resize(3);
  
  
  normal_Cam_optim = new double[3];
  X0_2_optim = new double[3];
  vbase_u_optim = new double[3];
  vbase_v_optim = new double[3];
  Xinter_optim = new double[3];
  
  colorI = text.colorI;
  interp = text.interp;
  bgColor = text.bgColor;
  cleanPrevImage = text.cleanPrevImage;
  
  setCameraPosition(text.cMt);
}

/*!
  Basic destructor.
*/
vpImageSimulator::~vpImageSimulator()
{
  delete[] normal_Cam_optim;
  delete[] X0_2_optim;
  delete[] vbase_u_optim;
  delete[] vbase_v_optim;
  delete[] Xinter_optim;
}


vpImageSimulator&
vpImageSimulator::operator=(const vpImageSimulator& sim)
{
  for(int i=0;i<4;i++)
  {
    X[i] = sim.X[i];
    pt[i] = sim.pt[i];
  }
  
  Ic = sim.Ic;
  Ig = sim.Ig;
  
  bgColor = sim.bgColor;
  cleanPrevImage = sim.cleanPrevImage;
  
  focal = sim.focal;
  
  normal_obj = sim.normal_obj;
  euclideanNorm_u = sim.euclideanNorm_u;
  euclideanNorm_v = sim.euclideanNorm_v;
  
  colorI = sim.colorI;
  interp = sim.interp;
  
  setCameraPosition(sim.cMt);
  
  return *this;
}

/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!
  
  \param I : The image used to store the result.
  \param cam : The parameters of the virtual camera.
*/
void
vpImageSimulator::getImage(vpImage<unsigned char> &I, 
			   const vpCameraParameters &cam)
{
  int nb_point_dessine = 0;
  if (cleanPrevImage)
  {
    unsigned char col = (unsigned char)(0.2126 * bgColor.R + 0.7152 * bgColor.G + 0.0722 * bgColor.B);
    for (unsigned int i = 0; i < I.getHeight(); i++)
    {
      for (unsigned int j = 0; j < I.getWidth(); j++)
      {
	I[i][j] = col;
      }
    }
  }
  if(visible)
  {
    getRoi(I.getWidth(),I.getHeight(),cam,pt,rect);
    
    double top = rect.getTop();
    double bottom = rect.getBottom();
    double left = rect.getLeft();
    double right= rect.getRight();
    
    unsigned char *bitmap = I.bitmap;
    unsigned int width = I.getWidth();
    vpImagePoint ip;
    
    for (unsigned int i = (unsigned int)top; i < (unsigned int)bottom; i++)
    {
      for (unsigned int j = (unsigned int)left; j < (unsigned int)right; j++)
      {
        double x=0,y=0;
	ip.set_ij(i,j);
        vpPixelMeterConversion::convertPoint(cam,ip, x,y);
	ip.set_ij(y,x);
	if (colorI == GRAY_SCALED)
	{
	  unsigned char Ipixelplan = 0;
	  if(getPixel(ip,Ipixelplan))
	  {
	    *(bitmap+i*width+j)=Ipixelplan;
	    nb_point_dessine++;
	  }
	}
	else if (colorI == COLORED)
	{
	  vpRGBa Ipixelplan;
	  if(getPixel(ip,Ipixelplan))
	  {
	    unsigned char pixelgrey = (unsigned char)(0.2126 * Ipixelplan.R + 0.7152 * Ipixelplan.G + 0.0722 * Ipixelplan.B);
	    *(bitmap+i*width+j)=pixelgrey;
	    nb_point_dessine++;
	  }
	}
      }
    }
  }
}


/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay! In this method you specify directly the image which is projected.
  
  \param I : The image used to store the result.
  \param Isrc : The image which is projected into \f$ I \f$.
  \param cam : The parameters of the virtual camera.
*/
void
vpImageSimulator::getImage(vpImage<unsigned char> &I, 
			   vpImage<unsigned char> &Isrc, 
			   const vpCameraParameters &cam)
{
  int nb_point_dessine = 0;
  if (cleanPrevImage)
  {
    unsigned char col = (unsigned char)(0.2126 * bgColor.R + 0.7152 * bgColor.G + 0.0722 * bgColor.B);
    for (unsigned int i = 0; i < I.getHeight(); i++)
    {
      for (unsigned int j = 0; j < I.getWidth(); j++)
      {
	I[i][j] = col;
      }
    }
  }
  if(visible)
  {
    getRoi(I.getWidth(),I.getHeight(),cam,pt,rect);
    
    double top = rect.getTop();
    double bottom = rect.getBottom();
    double left = rect.getLeft();
    double right= rect.getRight();
    
    unsigned char *bitmap = I.bitmap;
    unsigned int width = I.getWidth();
    vpImagePoint ip;
    
    for (unsigned int i = (unsigned int)top; i < (unsigned int)bottom; i++)
    {
      for (unsigned int j = (unsigned int)left; j < (unsigned int)right; j++)
      {
        double x=0,y=0;
	ip.set_ij(i,j);
        vpPixelMeterConversion::convertPoint(cam,ip, x,y);
	ip.set_ij(y,x);
	unsigned char Ipixelplan = 0;
	if(getPixel(Isrc,ip,Ipixelplan))
	{
	  *(bitmap+i*width+j)=Ipixelplan;
	  nb_point_dessine++;
	}
      }
    }
  }
}

/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!
  
  To take into account the projection of several images, a matrix \f$ zBuffer \f$ is given as argument. This matrix contains the z coordinates of all the pixel of the image \f$ I \f$ in the camera frame. During the projection, the pixels are updated if there is no other plan between the camera and the projected image. The matrix \f$ zBuffer \f$ is updated in this case.
  
  \param I : The image used to store the result.
  \param cam : The parameters of the virtual camera.
  \param zBuffer : A matrix containing the z coordinates of the pixels of the image \f$ I \f$
*/
void
vpImageSimulator::getImage(vpImage<unsigned char> &I, 
			   const vpCameraParameters &cam, vpMatrix &zBuffer)
{
  if (I.getWidth() != (unsigned int)zBuffer.getCols() || I.getHeight() != (unsigned int)zBuffer.getRows())
    throw (vpMatrixException(vpMatrixException::incorrectMatrixSizeError, " zBuffer must have the same size as the image I ! "));
  
  int nb_point_dessine = 0;
  if (cleanPrevImage)
  {
    unsigned char col = (unsigned char)(0.2126 * bgColor.R + 0.7152 * bgColor.G + 0.0722 * bgColor.B);
    for (unsigned int i = 0; i < I.getHeight(); i++)
    {
      for (unsigned int j = 0; j < I.getWidth(); j++)
      {
	I[i][j] = col;
      }
    }
  }
  if(visible)
  {
    getRoi(I.getWidth(),I.getHeight(),cam,pt,rect);
    
    double top = rect.getTop();
    double bottom = rect.getBottom();
    double left = rect.getLeft();
    double right= rect.getRight();
    
    unsigned char *bitmap = I.bitmap;
    unsigned int width = I.getWidth();
    vpImagePoint ip;
    
    for (unsigned int i = (unsigned int)top; i < (unsigned int)bottom; i++)
    {
      for (unsigned int j = (unsigned int)left; j < (unsigned int)right; j++)
      {
        double x=0,y=0;
	ip.set_ij(i,j);
        vpPixelMeterConversion::convertPoint(cam,ip, x,y);
	ip.set_ij(y,x);
	if (colorI == GRAY_SCALED)
	{
	  unsigned char Ipixelplan;
	  if(getPixel(ip,Ipixelplan))
	  {
	    if (Xinter_optim[2] < zBuffer[i][j] || zBuffer[i][j] < 0)
	    {
	      *(bitmap+i*width+j)=Ipixelplan;
	      nb_point_dessine++;
	      zBuffer[i][j] = Xinter_optim[2];
	    }
	  }
	}
	else if (colorI == COLORED)
	{
	  vpRGBa Ipixelplan;
	  if(getPixel(ip,Ipixelplan))
	  {
	    if (Xinter_optim[2] < zBuffer[i][j] || zBuffer[i][j] < 0)
	    {
	      unsigned char pixelgrey = (unsigned char)(0.2126 * Ipixelplan.R + 0.7152 * Ipixelplan.G + 0.0722 * Ipixelplan.B);
	      *(bitmap+i*width+j)=pixelgrey;
	      nb_point_dessine++;
	      zBuffer[i][j] = Xinter_optim[2];
	    }
	  }
	}
      }
    }
  }
}

/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!
  
  \param I : The image used to store the result.
  \param cam : The parameters of the virtual camera.
*/
void
vpImageSimulator::getImage(vpImage<vpRGBa> &I, const vpCameraParameters &cam)
{
  int nb_point_dessine = 0;
  if (cleanPrevImage)
  {
    for (unsigned int i = 0; i < I.getHeight(); i++)
    {
      for (unsigned int j = 0; j < I.getWidth(); j++)
      {
	I[i][j] = bgColor;
      }
    }
  }
  
  if(visible)
  {
    getRoi(I.getWidth(),I.getHeight(),cam,pt,rect);
    
    double top = rect.getTop();
    double bottom = rect.getBottom();
    double left = rect.getLeft();
    double right= rect.getRight();
    
    vpRGBa *bitmap = I.bitmap;
    unsigned int width = I.getWidth();
    vpImagePoint ip;
    
    for (unsigned int i = (unsigned int)top; i < (unsigned int)bottom; i++)
    {
      for (unsigned int j = (unsigned int)left; j < (unsigned int)right; j++)
      {
        double x=0,y=0;
	ip.set_ij(i,j);
        vpPixelMeterConversion::convertPoint(cam,ip, x,y);
	ip.set_ij(y,x);
	if (colorI == GRAY_SCALED)
	{
	  unsigned char Ipixelplan;
	  if(getPixel(ip,Ipixelplan))
	  {
	    vpRGBa pixelcolor;
	    pixelcolor.R = Ipixelplan;
	    pixelcolor.G = Ipixelplan;
	    pixelcolor.B = Ipixelplan;
	    *(bitmap+i*width+j) = pixelcolor;
	    nb_point_dessine++;
	  }
	}
	else if (colorI == COLORED)
	{
	  vpRGBa Ipixelplan;
	  if(getPixel(ip,Ipixelplan))
	  {
	    *(bitmap+i*width+j) = Ipixelplan;
	    nb_point_dessine++;
	  }
	}
      }
    }
  }
}


/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay! In this method you specify directly the image which is projected.
  
  \param I : The image used to store the result.
  \param Isrc : The image which is projected into \f$ I \f$.
  \param cam : The parameters of the virtual camera.
*/
void
vpImageSimulator::getImage(vpImage<vpRGBa> &I, vpImage<vpRGBa> &Isrc, 
			   const vpCameraParameters &cam)
{
  int nb_point_dessine = 0;
  if (cleanPrevImage)
  {
    for (unsigned int i = 0; i < I.getHeight(); i++)
    {
      for (unsigned int j = 0; j < I.getWidth(); j++)
      {
	I[i][j] = bgColor;
      }
    }
  }
  
  if(visible)
  {
    getRoi(I.getWidth(),I.getHeight(),cam,pt,rect);
    
    double top = rect.getTop();
    double bottom = rect.getBottom();
    double left = rect.getLeft();
    double right= rect.getRight();
    
    vpRGBa *bitmap = I.bitmap;
    unsigned int width = I.getWidth();
    vpImagePoint ip;
    
    for (unsigned int i = (unsigned int)top; i < (unsigned int)bottom; i++)
    {
      for (unsigned int j = (unsigned int)left; j < (unsigned int)right; j++)
      {
        double x=0,y=0;
	ip.set_ij(i,j);
        vpPixelMeterConversion::convertPoint(cam,ip, x,y);
	ip.set_ij(y,x);
	vpRGBa Ipixelplan;
	if(getPixel(Isrc,ip,Ipixelplan))
	{
	  *(bitmap+i*width+j) = Ipixelplan;
	  nb_point_dessine++;
	}
      }
    }
  }
}

/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!
  
  To take into account the projection of several images, a matrix \f$ zBuffer \f$ is given as argument. This matrix contains the z coordinates of all the pixel of the image \f$ I \f$ in the camera frame. During the projection, the pixels are updated if there is no other plan between the camera and the projected image. The matrix \f$ zBuffer \f$ is updated in this case.
  
  \param I : The image used to store the result.
  \param cam : The parameters of the virtual camera.
  \param zBuffer : A matrix containing the z coordinates of the pixels of the image \f$ I \f$
*/
void
vpImageSimulator::getImage(vpImage<vpRGBa> &I, const vpCameraParameters &cam, 
			   vpMatrix &zBuffer)
{
  if (I.getWidth() != (unsigned int)zBuffer.getCols() || I.getHeight() != (unsigned int)zBuffer.getRows())
    throw (vpMatrixException(vpMatrixException::incorrectMatrixSizeError, " zBuffer must have the same size as the image I ! "));
  
  int nb_point_dessine = 0;
  if (cleanPrevImage)
  {
    for (unsigned int i = 0; i < I.getHeight(); i++)
    {
      for (unsigned int j = 0; j < I.getWidth(); j++)
      {
	I[i][j] = bgColor;
      }
    }
  }
  if(visible)
  {
    getRoi(I.getWidth(),I.getHeight(),cam,pt,rect);
    
    double top = rect.getTop();
    double bottom = rect.getBottom();
    double left = rect.getLeft();
    double right= rect.getRight();
    
    vpRGBa *bitmap = I.bitmap;
    unsigned int width = I.getWidth();
    vpImagePoint ip;
    
    for (unsigned int i = (unsigned int)top; i < (unsigned int)bottom; i++)
    {
      for (unsigned int j = (unsigned int)left; j < (unsigned int)right; j++)
      {
        double x=0,y=0;
	ip.set_ij(i,j);
        vpPixelMeterConversion::convertPoint(cam,ip, x,y);
	ip.set_ij(y,x);
	if (colorI == GRAY_SCALED)
	{
	  unsigned char Ipixelplan;
	  if(getPixel(ip,Ipixelplan))
	  {
	    if (Xinter_optim[2] < zBuffer[i][j] || zBuffer[i][j] < 0)
	    {
	      vpRGBa pixelcolor;
	      pixelcolor.R = Ipixelplan;
	      pixelcolor.G = Ipixelplan;
	      pixelcolor.B = Ipixelplan;
	      *(bitmap+i*width+j) = pixelcolor;
	      nb_point_dessine++;
	      zBuffer[i][j] = Xinter_optim[2];
	    }
	  }
	}
	else if (colorI == COLORED)
	{
	  vpRGBa Ipixelplan;
	  if(getPixel(ip,Ipixelplan))
	  {
	    if (Xinter_optim[2] < zBuffer[i][j] || zBuffer[i][j] < 0)
	    {
	      *(bitmap+i*width+j) = Ipixelplan;
	      nb_point_dessine++;
	      zBuffer[i][j] = Xinter_optim[2];
	    }
	  }
	}
      }
    }
  }
}

#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
/*!
  \deprecated  This method is deprecated, you should use:
  vpImageSimulator::getImage(vpImage<unsigned char> &,
                             std::list<vpImageSimulator> &,
                             const vpCameraParameters &) instead.

  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!
  
  With this method, a list of image is projected into the image. Thus, you have to initialise a list of vpImageSimulator. Then you store them into a vpList. And finally with this method you project them into the image \f$ I \f$. The depth of the 3D scene is managed such as an image in foreground hides an image background.
  

  The following example shows how to use the method:
  
  \code
  #include <visp/vpImage.h>
  #include <visp/vpImageSimulator.h>
  
  int main()
  {
    vpImage<vpRGBa> Icamera(480,640,0);
    vpImage<vpRGBa> Iimage(60,60);
    
    // Initialise the image which will be projected into the image Icamera
    vpRGBa colorb(0,0,255);
    vpRGBa colorw(255,255,255);
    vpRGBa colorr(255,0,0);
    for(int i = 0; i < 60; i++)
    {
      for(int j = 0; j < 20; j++)
        Iimage[i][j] = colorb;
      for(int j = 20; j < 40; j++)
        Iimage[i][j] = colorw;
      for(int j = 40; j < 60; j++)
        Iimage[i][j] = colorr;
    }
    
    // Initialise the 3D coordinates of the Iimage corners
    vpColVector X[4];
    for (int i = 0; i < 4; i++) X[i].resize(3);
    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;
  
    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;
  
    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;
  
    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;
    
    vpImageSimulator sim;
    sim.init(Iimage, X);
    
    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;
  
    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;
  
    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;
  
    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;
    
    vpImageSimulator sim2;
    sim2.init(Iimage, X);
    
    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(30),0));
    sim2.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(-30),0));
    
    vpList<vpImageSimulator> listSim;
    listSim.addRight(sim);
    listSim.addRight(sim2);
    
    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(60),vpMath::rad(0),0));
    
    vpCameraParameters cam(868.0, 869.0, 320, 240);
    
    vpImageSimulator::getImage(Icamera,listSim,cam);
    
    return 0;
  }
  \endcode
  
  \param I : The image used to store the result
  \param list : List of vpImageSimulator to project
  \param cam : The parameters of the virtual camera
*/
void
vpImageSimulator::getImage(vpImage<unsigned char> &I, 
			   vpList<vpImageSimulator> &list, 
			   const vpCameraParameters &cam)
{
  
  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();
  
  unsigned int nbsimList = list.nbElements();
  
  if (nbsimList < 1)
    return;
  
  vpImageSimulator** simList = new vpImageSimulator* [nbsimList];
  
  double topFinal = height+1;;
  double bottomFinal = -1;
  double leftFinal = width+1;
  double rightFinal = -1;
  
  list.front();
  
  unsigned int unvisible = 0;
  for (unsigned int i = 0; i < nbsimList; i++)
  {
    vpImageSimulator* sim = &(list.value());
    list.next();
    if (sim->visible)
      simList[i] = sim;
    else
      unvisible++;
  }
  nbsimList = nbsimList - unvisible;
  
   if (nbsimList < 1)
   {
     delete[] simList;
     return;
   }
  
      
  for (unsigned int i = 0; i < nbsimList; i++)
  {
    
    simList[i]->getRoi(width,height,cam,simList[i]->pt,simList[i]->rect);
    
    if (topFinal > simList[i]->rect.getTop()) topFinal = simList[i]->rect.getTop();
    if (bottomFinal < simList[i]->rect.getBottom()) bottomFinal = simList[i]->rect.getBottom();
    if (leftFinal > simList[i]->rect.getLeft()) leftFinal = simList[i]->rect.getLeft();
    if (rightFinal < simList[i]->rect.getRight()) rightFinal = simList[i]->rect.getRight();  
  }
  
  double zmin = -1;
  int indice = -1;
  unsigned char *bitmap = I.bitmap;
  vpImagePoint ip;
    
  for (unsigned int i = (unsigned int)topFinal; i < (unsigned int)bottomFinal; i++)
  {
    for (unsigned int j = (unsigned int)leftFinal; j < (unsigned int)rightFinal; j++)
    {
      zmin = -1;
      double x=0,y=0;
      ip.set_ij(i,j);
      vpPixelMeterConversion::convertPoint(cam,ip, x,y);
      ip.set_ij(y,x);
      for (int k = 0; k < (int)nbsimList; k++)
      {
	double z = 0;
	if(simList[k]->getPixelDepth(ip,z))
	{
	  if (z < zmin || zmin < 0)
	  {
	    zmin = z;
	    indice = k;
	  }
	}
      }
      if (indice >= 0)
      {
        if (simList[indice]->colorI == GRAY_SCALED)
        {
	  unsigned char Ipixelplan = 255;
          simList[indice]->getPixel(ip,Ipixelplan);
	  *(bitmap+i*width+j)=Ipixelplan;
        }
        else if (simList[indice]->colorI == COLORED)
        {
	  vpRGBa Ipixelplan(255,255,255);
	  simList[indice]->getPixel(ip,Ipixelplan);
	  unsigned char pixelgrey = (unsigned char)(0.2126 * Ipixelplan.R + 0.7152 * Ipixelplan.G + 0.0722 * Ipixelplan.B);
	  *(bitmap+i*width+j)=pixelgrey;
        }
      }
    }
  }
  
  delete[] simList;
}


/*!
  \deprecated This method is deprecated, you should use:
  vpImageSimulator::getImage(vpImage<vpRGBa> &,
                             std::list<vpImageSimulator> &,
                             const vpCameraParameters &) instead.

  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!
  
  With this method, a list of image is projected into the image. Thus, you have to initialise a list of vpImageSimulator. Then you store them into a vpList. And finally with this method you project them into the image \f$ I \f$. The depth of the 3D scene is managed such as an image in foreground hides an image background.

  The following example shows how to use the method:
  
  \code
  #include <visp/vpImage.h>
  #include <visp/vpImageSimulator.h>
  
  int main()
  {
    vpImage<vpRGBa> Icamera(480,640,0);
    vpImage<vpRGBa> Iimage(60,60);
    
    // Initialise the image which will be projected into the image Icamera
    vpRGBa colorb(0,0,255);
    vpRGBa colorw(255,255,255);
    vpRGBa colorr(255,0,0);
    for(int i = 0; i < 60; i++)
    {
      for(int j = 0; j < 20; j++)
        Iimage[i][j] = colorb;
      for(int j = 20; j < 40; j++)
        Iimage[i][j] = colorw;
      for(int j = 40; j < 60; j++)
        Iimage[i][j] = colorr;
    }
    
    // Initialise the 3D coordinates of the Iimage corners
    vpColVector X[4];
    for (int i = 0; i < 4; i++) X[i].resize(3);
    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;
  
    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;
  
    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;
  
    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;
    
    vpImageSimulator sim;
    sim.init(Iimage, X);
    
    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;
  
    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;
  
    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;
  
    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;
    
    vpImageSimulator sim2;
    sim2.init(Iimage, X);
    
    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(30),0));
    sim2.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(-30),0));
    
    vpList<vpImageSimulator> listSim;
    listSim.addRight(sim);
    listSim.addRight(sim2);
    
    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(60),vpMath::rad(0),0));
    
    vpCameraParameters cam(868.0, 869.0, 320, 240);
    
    vpImageSimulator::getImage(Icamera,listSim,cam);
    
    return 0;
  }
  \endcode
  
  \param I : The image used to store the result
  \param list : List of vpImageSimulator to project
  \param cam : The parameters of the virtual camera
*/
void
vpImageSimulator::getImage(vpImage<vpRGBa> &I, vpList<vpImageSimulator> &list,
			   const vpCameraParameters &cam)
{
  
  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();
  
  unsigned int nbsimList = list.nbElements();
  
  if (nbsimList < 1)
    return;
  
  vpImageSimulator** simList = new vpImageSimulator* [nbsimList];
  
  double topFinal = height+1;;
  double bottomFinal = -1;
  double leftFinal = width+1;
  double rightFinal = -1;
  
  list.front();
  
  unsigned int unvisible = 0;
  for (unsigned int i = 0; i < nbsimList; i++)
  {
    vpImageSimulator* sim = &(list.value());
    list.next();
    if (sim->visible)
      simList[i] = sim;
    else
      unvisible++;
  }
  nbsimList = nbsimList - unvisible;
  
   if (nbsimList < 1)
   {
     delete[] simList;
     return;
   }  
      
  for (unsigned int i = 0; i < nbsimList; i++)
  {
    
    simList[i]->getRoi(width,height,cam,simList[i]->pt,simList[i]->rect);
    
    if (topFinal > simList[i]->rect.getTop()) topFinal = simList[i]->rect.getTop();
    if (bottomFinal < simList[i]->rect.getBottom()) bottomFinal = simList[i]->rect.getBottom();
    if (leftFinal > simList[i]->rect.getLeft()) leftFinal = simList[i]->rect.getLeft();
    if (rightFinal < simList[i]->rect.getRight()) rightFinal = simList[i]->rect.getRight();  
  }
  
  double zmin = -1;
  int indice = -1;
  vpRGBa *bitmap = I.bitmap;
  vpImagePoint ip;
    
  for (unsigned int i = (unsigned int)topFinal; i < (unsigned int)bottomFinal; i++)
  {
    for (unsigned int j = (unsigned int)leftFinal; j < (unsigned int)rightFinal; j++)
    {
      zmin = -1;
      double x=0,y=0;
      ip.set_ij(i,j);
      vpPixelMeterConversion::convertPoint(cam,ip, x,y);
      ip.set_ij(y,x);
      for (int k = 0; k < (int)nbsimList; k++)
      {
	double z = 0;
	if(simList[k]->getPixelDepth(ip,z))
	{
	  if (z < zmin || zmin < 0)
	  {
	    zmin = z;
	    indice = k;
	  }
	}
      }
      if (indice >= 0)
      {
        if (simList[indice]->colorI == GRAY_SCALED)
        {
	  unsigned char Ipixelplan = 255;
          simList[indice]->getPixel(ip,Ipixelplan);
	  vpRGBa pixelcolor;
	  pixelcolor.R = Ipixelplan;
	  pixelcolor.G = Ipixelplan;
	  pixelcolor.B = Ipixelplan;
	  *(bitmap+i*width+j) = pixelcolor;
        }
        else if (simList[indice]->colorI == COLORED)
        {
	  vpRGBa Ipixelplan(255,255,255);
	  simList[indice]->getPixel(ip,Ipixelplan);
	  //unsigned char pixelgrey = 0.2126 * Ipixelplan.R + 0.7152 * Ipixelplan.G + 0.0722 * Ipixelplan.B;
	  *(bitmap+i*width+j)=Ipixelplan;
        }
      }
    }
  }
  
  delete[] simList;
}
#endif

/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!

  With this method, a list of image is projected into the image. Thus, you have to initialise a list of vpImageSimulator. Then you store them into a vpList. And finally with this method you project them into the image \f$ I \f$. The depth of the 3D scene is managed such as an image in foreground hides an image background.

  The following example shows how to use the method:

  \code
  #include <list>
  #include <visp/vpImage.h>
  #include <visp/vpImageSimulator.h>

  int main()
  {
    vpImage<vpRGBa> Icamera(480,640,0);
    vpImage<vpRGBa> Iimage(60,60);

    // Initialise the image which will be projected into the image Icamera
    vpRGBa colorb(0,0,255);
    vpRGBa colorw(255,255,255);
    vpRGBa colorr(255,0,0);
    for(int i = 0; i < 60; i++)
    {
      for(int j = 0; j < 20; j++)
        Iimage[i][j] = colorb;
      for(int j = 20; j < 40; j++)
        Iimage[i][j] = colorw;
      for(int j = 40; j < 60; j++)
        Iimage[i][j] = colorr;
    }

    // Initialise the 3D coordinates of the Iimage corners
    vpColVector X[4];
    for (int i = 0; i < 4; i++) X[i].resize(3);
    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;

    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;

    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;

    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;

    vpImageSimulator sim;
    sim.init(Iimage, X);

    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;

    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;

    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;

    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;

    vpImageSimulator sim2;
    sim2.init(Iimage, X);

    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(30),0));
    sim2.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(-30),0));

    std::list<vpImageSimulator> listSim;
    listSim.addRight(sim);
    listSim.addRight(sim2);

    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(60),vpMath::rad(0),0));

    vpCameraParameters cam(868.0, 869.0, 320, 240);

    vpImageSimulator::getImage(Icamera,listSim,cam);

    return 0;
  }
  \endcode

  \param I : The image used to store the result
  \param list : List of vpImageSimulator to project
  \param cam : The parameters of the virtual camera
*/
void
vpImageSimulator::getImage(vpImage<unsigned char> &I,
                           std::list<vpImageSimulator> &list,
                           const vpCameraParameters &cam)
{

  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();

  unsigned int nbsimList = list.size();

  if (nbsimList < 1)
    return;

  vpImageSimulator** simList = new vpImageSimulator* [nbsimList];

  double topFinal = height+1;;
  double bottomFinal = -1;
  double leftFinal = width+1;
  double rightFinal = -1;

  unsigned int unvisible = 0;
  unsigned int indexSimu=0;
  for(std::list<vpImageSimulator>::iterator it=list.begin(); it!=list.end(); ++it, ++indexSimu){
    vpImageSimulator* sim = &(*it);
    if (sim->visible)
      simList[indexSimu] = sim;
    else
      unvisible++;
  }
  nbsimList = nbsimList - unvisible;

   if (nbsimList < 1)
   {
     delete[] simList;
     return;
   }


  for (unsigned int i = 0; i < nbsimList; i++)
  {

    simList[i]->getRoi(width,height,cam,simList[i]->pt,simList[i]->rect);

    if (topFinal > simList[i]->rect.getTop()) topFinal = simList[i]->rect.getTop();
    if (bottomFinal < simList[i]->rect.getBottom()) bottomFinal = simList[i]->rect.getBottom();
    if (leftFinal > simList[i]->rect.getLeft()) leftFinal = simList[i]->rect.getLeft();
    if (rightFinal < simList[i]->rect.getRight()) rightFinal = simList[i]->rect.getRight();
  }

  double zmin = -1;
  int indice = -1;
  unsigned char *bitmap = I.bitmap;
  vpImagePoint ip;

  for (unsigned int i = (unsigned int)topFinal; i < (unsigned int)bottomFinal; i++)
  {
    for (unsigned int j = (unsigned int)leftFinal; j < (unsigned int)rightFinal; j++)
    {
      zmin = -1;
      double x=0,y=0;
      ip.set_ij(i,j);
      vpPixelMeterConversion::convertPoint(cam,ip, x,y);
      ip.set_ij(y,x);
      for (int k = 0; k < (int)nbsimList; k++)
      {
  double z = 0;
  if(simList[k]->getPixelDepth(ip,z))
  {
    if (z < zmin || zmin < 0)
    {
      zmin = z;
      indice = k;
    }
  }
      }
      if (indice >= 0)
      {
        if (simList[indice]->colorI == GRAY_SCALED)
        {
    unsigned char Ipixelplan = 255;
          simList[indice]->getPixel(ip,Ipixelplan);
    *(bitmap+i*width+j)=Ipixelplan;
        }
        else if (simList[indice]->colorI == COLORED)
        {
    vpRGBa Ipixelplan(255,255,255);
    simList[indice]->getPixel(ip,Ipixelplan);
    unsigned char pixelgrey = (unsigned char)(0.2126 * Ipixelplan.R + 0.7152 * Ipixelplan.G + 0.0722 * Ipixelplan.B);
    *(bitmap+i*width+j)=pixelgrey;
        }
      }
    }
  }

  delete[] simList;
}


/*!
  Get the view of the virtual camera. Be carefull, the image I is modified. The projected image is not added as an overlay!

  With this method, a list of image is projected into the image. Thus, you have to initialise a list of vpImageSimulator. Then you store them into a vpList. And finally with this method you project them into the image \f$ I \f$. The depth of the 3D scene is managed such as an image in foreground hides an image background.

  The following example shows how to use the method:

  \code
  #include <list>
  #include <visp/vpImage.h>
  #include <visp/vpImageSimulator.h>

  int main()
  {
    vpImage<vpRGBa> Icamera(480,640,0);
    vpImage<vpRGBa> Iimage(60,60);

    // Initialise the image which will be projected into the image Icamera
    vpRGBa colorb(0,0,255);
    vpRGBa colorw(255,255,255);
    vpRGBa colorr(255,0,0);
    for(int i = 0; i < 60; i++)
    {
      for(int j = 0; j < 20; j++)
        Iimage[i][j] = colorb;
      for(int j = 20; j < 40; j++)
        Iimage[i][j] = colorw;
      for(int j = 40; j < 60; j++)
        Iimage[i][j] = colorr;
    }

    // Initialise the 3D coordinates of the Iimage corners
    vpColVector X[4];
    for (int i = 0; i < 4; i++) X[i].resize(3);
    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;

    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;

    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;

    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;

    vpImageSimulator sim;
    sim.init(Iimage, X);

    // Top left corner
    X[0][0] = -1;
    X[0][1] = -1;
    X[0][2] = 1;

    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 1;

    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 1;

    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 1;

    vpImageSimulator sim2;
    sim2.init(Iimage, X);

    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(30),0));
    sim2.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(0),vpMath::rad(-30),0));

    std::list<vpImageSimulator> listSim;
    listSim.addRight(sim);
    listSim.addRight(sim2);

    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(60),vpMath::rad(0),0));

    vpCameraParameters cam(868.0, 869.0, 320, 240);

    vpImageSimulator::getImage(Icamera,listSim,cam);

    return 0;
  }
  \endcode

  \param I : The image used to store the result
  \param list : List of vpImageSimulator to project
  \param cam : The parameters of the virtual camera
*/
void
vpImageSimulator::getImage(vpImage<vpRGBa> &I,
                           std::list<vpImageSimulator> &list,
                           const vpCameraParameters &cam)
{

  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();

  unsigned int nbsimList = list.size();

  if (nbsimList < 1)
    return;

  vpImageSimulator** simList = new vpImageSimulator* [nbsimList];

  double topFinal = height+1;;
  double bottomFinal = -1;
  double leftFinal = width+1;
  double rightFinal = -1;

  unsigned int unvisible = 0;
  unsigned int indexSimu = 0;
  for(std::list<vpImageSimulator>::iterator it=list.begin(); it!=list.end(); ++it, ++indexSimu){
    vpImageSimulator* sim = &(*it);
    if (sim->visible)
      simList[indexSimu] = sim;
    else
      unvisible++;
  }

  nbsimList = nbsimList - unvisible;

   if (nbsimList < 1)
   {
     delete[] simList;
     return;
   }

  for (unsigned int i = 0; i < nbsimList; i++)
  {

    simList[i]->getRoi(width,height,cam,simList[i]->pt,simList[i]->rect);

    if (topFinal > simList[i]->rect.getTop()) topFinal = simList[i]->rect.getTop();
    if (bottomFinal < simList[i]->rect.getBottom()) bottomFinal = simList[i]->rect.getBottom();
    if (leftFinal > simList[i]->rect.getLeft()) leftFinal = simList[i]->rect.getLeft();
    if (rightFinal < simList[i]->rect.getRight()) rightFinal = simList[i]->rect.getRight();
  }

  double zmin = -1;
  int indice = -1;
  vpRGBa *bitmap = I.bitmap;
  vpImagePoint ip;

  for (unsigned int i = (unsigned int)topFinal; i < (unsigned int)bottomFinal; i++)
  {
    for (unsigned int j = (unsigned int)leftFinal; j < (unsigned int)rightFinal; j++)
    {
      zmin = -1;
      double x=0,y=0;
      ip.set_ij(i,j);
      vpPixelMeterConversion::convertPoint(cam,ip, x,y);
      ip.set_ij(y,x);
      for (int k = 0; k < (int)nbsimList; k++)
      {
  double z = 0;
  if(simList[k]->getPixelDepth(ip,z))
  {
    if (z < zmin || zmin < 0)
    {
      zmin = z;
      indice = k;
    }
  }
      }
      if (indice >= 0)
      {
        if (simList[indice]->colorI == GRAY_SCALED)
        {
    unsigned char Ipixelplan = 255;
          simList[indice]->getPixel(ip,Ipixelplan);
    vpRGBa pixelcolor;
    pixelcolor.R = Ipixelplan;
    pixelcolor.G = Ipixelplan;
    pixelcolor.B = Ipixelplan;
    *(bitmap+i*width+j) = pixelcolor;
        }
        else if (simList[indice]->colorI == COLORED)
        {
    vpRGBa Ipixelplan(255,255,255);
    simList[indice]->getPixel(ip,Ipixelplan);
    //unsigned char pixelgrey = 0.2126 * Ipixelplan.R + 0.7152 * Ipixelplan.G + 0.0722 * Ipixelplan.B;
    *(bitmap+i*width+j)=Ipixelplan;
        }
      }
    }
  }

  delete[] simList;
}

/*!
  Enable to set the position of the 3D plane relative to the virtual camera.
  
  \param _cMt : The pose of the plane relative to the virtual camera.
*/
void
vpImageSimulator::setCameraPosition(const vpHomogeneousMatrix &_cMt)
{
  cMt = _cMt;
  vpRotationMatrix R;
  cMt.extract(R);

  normal_Cam = R * normal_obj;	
  
  visible_result = vpColVector::dotProd(normal_Cam,focal);
  
  for(int i = 0; i < 4; i++)
    pt[i].track(cMt);
  
  vpColVector e1(3) ;
  vpColVector e2(3) ;
  vpColVector facenormal(3) ;

  e1[0] = pt[1].get_X() - pt[0].get_X() ;
  e1[1] = pt[1].get_Y() - pt[0].get_Y() ;
  e1[2] = pt[1].get_Z() - pt[0].get_Z() ;

  e2[0] = pt[2].get_X() - pt[1].get_X() ;
  e2[1] = pt[2].get_Y() - pt[1].get_Y() ;
  e2[2] = pt[2].get_Z() - pt[1].get_Z() ;

  facenormal = vpColVector::crossProd(e1,e2) ;

  double angle = pt[0].get_X()*facenormal[0] +  pt[0].get_Y()*facenormal[1]  +  pt[0].get_Z()*facenormal[2]  ;

  if (angle > 0)
    visible=true;
  else 
    visible=false;

  if(visible)
  {
    for(int i = 0; i < 4; i++)
    {
      project(X[i],cMt,X2[i]);
      pt[i].track(cMt);
    }

    vbase_u = X2[1]-X2[0];
    vbase_v = X2[3]-X2[0];

    distance = vpColVector::dotProd(normal_Cam,X2[1]);
    
    if(distance < 0)
    {
      visible = false;
      return;
    }

    for(unsigned int i = 0; i < 3; i++)
    {
      normal_Cam_optim[i] = normal_Cam[i];
      X0_2_optim[i] = X2[0][i];
      vbase_u_optim[i] = vbase_u[i];
      vbase_v_optim[i] = vbase_v[i];
    }
    
    vpImagePoint iPa[4];
    for(unsigned int i = 0; i < 4; i++)
    {
      iPa[i].set_j(X2[i][0]/X2[i][2]);
      iPa[i].set_i(X2[i][1]/X2[i][2]);
    }
    
    T1.buildFrom(iPa[0],iPa[1],iPa[3]);
    T2.buildFrom(iPa[2],iPa[1],iPa[3]);
  }
}

void
vpImageSimulator::initPlan(vpColVector* _X)
{
  for (unsigned int i = 0; i < 4; i++)
  {
    X[i]=_X[i];
    pt[i].setWorldCoordinates(_X[i][0],_X[i][1],_X[i][2]);
  }

  normal_obj=vpColVector::crossProd(X[1]-X[0],X[3]-X[0]);
  normal_obj=normal_obj/normal_obj.euclideanNorm();

  euclideanNorm_u=(X[1]-X[0]).euclideanNorm();
  euclideanNorm_v=(X[3]-X[0]).euclideanNorm();
}

/*!
  Initialise the image thanks to an image \f$ I \f$ and a table of vector containing the 3D coordinates of the image's corners.
  
  The table must have a size of 4!
  
  - \f$ X[0] \f$ :Top left corner.
  - \f$ X[1] \f$ :Top right corner.
  - \f$ X[2] \f$ :Bottom right corner.
  - \f$ X[3] \f$ :Bottom left corner.
  
  \param I : The image which is projected.
  \param _X : table of the 3D coordinates corresponding to the image corners.
*/
void
vpImageSimulator::init(const vpImage<unsigned char> &I,vpColVector* _X)
{
  Ig = I;
  vpImageConvert::convert(I,Ic);
  initPlan(_X);
}

/*!
  Initialise the image thanks to an image \f$ I \f$ and a table of vector containing the 3D coordinates of the image's corners.
  
  The table must have a size of 4!
  
  - \f$ X[0] \f$ :Top left corner.
  - \f$ X[1] \f$ :Top right corner.
  - \f$ X[2] \f$ :Bottom right corner.
  - \f$ X[3] \f$ :Bottom left corner.
  
  \param I : The image which is projected.
  \param _X : table of the 3D coordinates corresponding to the image corners.
*/
void
vpImageSimulator::init(const vpImage<vpRGBa> &I,vpColVector* _X)
{
  Ic = I;
  vpImageConvert::convert(I,Ig);
  initPlan(_X);
}

/*!
  Initialise the image thanks to an image whose adress is given by \f$ file_image \f$ and a table of vector containing the 3D coordinates of the image's corners.
  
  The table must have a size of 4!
  
  - \f$ X[0] \f$ :Top left corner.
  - \f$ X[1] \f$ :Top right corner.
  - \f$ X[2] \f$ :Bottom right corner.
  - \f$ X[3] \f$ :Bottom left corner.
  
  \param file_image : The adress of an image file.
  \param _X : table of the 3D coordinates corresponding to the image corners.
*/
void
vpImageSimulator::init(const char* file_image,vpColVector* _X)
{
  vpImageIo::read(Ig,file_image);
  vpImageIo::read(Ic,file_image);
  initPlan(_X);
}

/*!
  Initialise the image thanks to an image \f$ I \f$ and a table of vector containing the 3D coordinates of the image's corners.

  \throw vpException::dimensionError if the _X vector is not of size 4.

  - \f$ X[0] \f$ :Top left corner.
  - \f$ X[1] \f$ :Top right corner.
  - \f$ X[2] \f$ :Bottom right corner.
  - \f$ X[3] \f$ :Bottom left corner.

  \param I : The image which is projected.
  \param _X : Vector of the 3D coordinates in the object frame (oX, oY, oZ)
  corresponding to the image corners.
*/
void
vpImageSimulator::init(const vpImage<unsigned char> &I, const std::vector<vpPoint>& _X)
{
  if(_X.size() != 4){
    throw vpException(vpException::dimensionError, "the vector must contains 4 points to initialise the simulator");
  }
  vpColVector Xvec[4];
  for(unsigned int i=0; i<4; ++i){
    Xvec[i].resize(3);
    Xvec[i][0] = _X[i].get_oX();
    Xvec[i][1] = _X[i].get_oY();
    Xvec[i][2] = _X[i].get_oZ();
  }

  Ig = I;
  vpImageConvert::convert(I,Ic);
  initPlan(Xvec);
}
/*!
  Initialise the image thanks to an image \f$ I \f$ and a table of vector containing the 3D coordinates of the image's corners.

  \throw vpException::dimensionError if the _X vector is not of size 4.

  - \f$ X[0] \f$ :Top left corner.
  - \f$ X[1] \f$ :Top right corner.
  - \f$ X[2] \f$ :Bottom right corner.
  - \f$ X[3] \f$ :Bottom left corner.

  \param I : The image which is projected.
  \param _X : Vector of the 3D coordinates in the object frame (oX, oY, oZ)
  corresponding to the image corners.
*/
void
vpImageSimulator::init(const vpImage<vpRGBa> &I, const std::vector<vpPoint>& _X)
{
  if(_X.size() != 4){
    throw vpException(vpException::dimensionError, "the vector must contains 4 points to initialise the simulator");
  }
  vpColVector Xvec[4];
  for(unsigned int i=0; i<4; ++i){
    Xvec[i].resize(3);
    Xvec[i][0] = _X[i].get_oX();
    Xvec[i][1] = _X[i].get_oY();
    Xvec[i][2] = _X[i].get_oZ();
  }

  Ic = I;
  vpImageConvert::convert(I,Ig);
  initPlan(Xvec);
}
/*!
  Initialise the image thanks to an image whose adress is given by \f$ file_image \f$ and a table of vector containing the 3D coordinates of the image's corners.

  \throw vpException::dimensionError if the _X vector is not of size 4.

  - \f$ X[0] \f$ :Top left corner.
  - \f$ X[1] \f$ :Top right corner.
  - \f$ X[2] \f$ :Bottom right corner.
  - \f$ X[3] \f$ :Bottom left corner.

  \param file_image : The adress of an image file.
  \param _X : Vector of the 3D coordinates in the object frame (oX, oY, oZ)
  corresponding to the image corners.
*/
void
vpImageSimulator::init(const char* file_image, const std::vector<vpPoint>& _X)
{
  if(_X.size() != 4){
    throw vpException(vpException::dimensionError, "the vector must contains 4 points to initialise the simulator");
  }
  vpColVector Xvec[4];
  for(unsigned int i=0; i<4; ++i){
    Xvec[i].resize(3);
    Xvec[i][0] = _X[i].get_oX();
    Xvec[i][1] = _X[i].get_oY();
    Xvec[i][2] = _X[i].get_oZ();
  }

  vpImageIo::read(Ig,file_image);
  vpImageIo::read(Ic,file_image);
  initPlan(Xvec);
}

bool
vpImageSimulator::getPixel(const vpImagePoint &iP, unsigned char &Ipixelplan)
{
  //test si pixel dans zone projetee
  if(!T1.inTriangle(iP) && !T2.inTriangle(iP))
    return false;

  //methoed algebrique
  double z;

  //calcul de la profondeur de l'intersection
  z = distance/(normal_Cam_optim[0]*iP.get_u()+normal_Cam_optim[1]*iP.get_v()+normal_Cam_optim[2]);
  //calcul coordonnees 3D intersection
  Xinter_optim[0]=iP.get_u()*z;
  Xinter_optim[1]=iP.get_v()*z;
  Xinter_optim[2]=z;

  //recuperation des coordonnes de l'intersection dans le plan objet
  //repere plan object : 
  //	centre = X0_2_optim[i] (premier point definissant le plan)
  //	base =  u:(X[1]-X[0]) et v:(X[3]-X[0])
  //ici j'ai considere que le plan est un rectangle => coordonnees sont simplement obtenu par un produit scalaire
  double u = 0, v = 0;
  double diff = 0;
  for(unsigned int i = 0; i < 3; i++)
  {
    diff = (Xinter_optim[i]-X0_2_optim[i]);
    u += diff*vbase_u_optim[i];
    v += diff*vbase_v_optim[i];
  }
  u = u/(euclideanNorm_u*euclideanNorm_u);
  v = v/(euclideanNorm_v*euclideanNorm_v);

  if( u > 0 && v > 0 && u < 1. && v < 1.)
  {
    double i2,j2;
    i2=v*(Ig.getHeight()-1);
    j2=u*(Ig.getWidth()-1);
    if (interp == BILINEAR_INTERPOLATION)
      Ipixelplan = Ig.getValue(i2,j2);
    else if (interp == SIMPLE)
      Ipixelplan = Ig[(unsigned int)i2][(unsigned int)j2];
    return true;
  }
  else
    return false;
}

bool
vpImageSimulator::getPixel(vpImage<unsigned char> &Isrc, 
			   const vpImagePoint &iP, unsigned char &Ipixelplan)
{
  //test si pixel dans zone projetee
  if(!T1.inTriangle(iP) && !T2.inTriangle(iP))
    return false;

  //methoed algebrique
  double z;

  //calcul de la profondeur de l'intersection
  z = distance/(normal_Cam_optim[0]*iP.get_u()+normal_Cam_optim[1]*iP.get_v()+normal_Cam_optim[2]);
  //calcul coordonnees 3D intersection
  Xinter_optim[0]=iP.get_u()*z;
  Xinter_optim[1]=iP.get_v()*z;
  Xinter_optim[2]=z;

  //recuperation des coordonnes de l'intersection dans le plan objet
  //repere plan object : 
  //	centre = X0_2_optim[i] (premier point definissant le plan)
  //	base =  u:(X[1]-X[0]) et v:(X[3]-X[0])
  //ici j'ai considere que le plan est un rectangle => coordonnees sont simplement obtenu par un produit scalaire
  double u = 0, v = 0;
  double diff = 0;
  for(unsigned int i = 0; i < 3; i++)
  {
    diff = (Xinter_optim[i]-X0_2_optim[i]);
    u += diff*vbase_u_optim[i];
    v += diff*vbase_v_optim[i];
  }
  u = u/(euclideanNorm_u*euclideanNorm_u);
  v = v/(euclideanNorm_v*euclideanNorm_v);

  if( u > 0 && v > 0 && u < 1. && v < 1.)
  {
    double i2,j2;
    i2=v*(Isrc.getHeight()-1);
    j2=u*(Isrc.getWidth()-1);
    if (interp == BILINEAR_INTERPOLATION)
      Ipixelplan = Isrc.getValue(i2,j2);
    else if (interp == SIMPLE)
      Ipixelplan = Isrc[(unsigned int)i2][(unsigned int)j2];
    return true;
  }
  else
    return false;
}


bool
vpImageSimulator::getPixel(const vpImagePoint &iP, vpRGBa &Ipixelplan)
{
  //test si pixel dans zone projetee
  if(!T1.inTriangle(iP) && !T2.inTriangle(iP))
    return false;

  //methoed algebrique
  double z;

  //calcul de la profondeur de l'intersection
  z = distance/(normal_Cam_optim[0]*iP.get_u()+normal_Cam_optim[1]*iP.get_v()+normal_Cam_optim[2]);
  //calcul coordonnees 3D intersection
  Xinter_optim[0]=iP.get_u()*z;
  Xinter_optim[1]=iP.get_v()*z;
  Xinter_optim[2]=z;

  //recuperation des coordonnes de l'intersection dans le plan objet
  //repere plan object : 
  //	centre = X0_2_optim[i] (premier point definissant le plan)
  //	base =  u:(X[1]-X[0]) et v:(X[3]-X[0])
  //ici j'ai considere que le plan est un rectangle => coordonnees sont simplement obtenu par un produit scalaire
  double u = 0, v = 0;
  double diff = 0;
  for(unsigned int i = 0; i < 3; i++)
  {
    diff = (Xinter_optim[i]-X0_2_optim[i]);
    u += diff*vbase_u_optim[i];
    v += diff*vbase_v_optim[i];
  }
  u = u/(euclideanNorm_u*euclideanNorm_u);
  v = v/(euclideanNorm_v*euclideanNorm_v);

  if( u > 0 && v > 0 && u < 1. && v < 1.)
  {
    double i2,j2;
    i2=v*(Ic.getHeight()-1);
    j2=u*(Ic.getWidth()-1);
    if (interp == BILINEAR_INTERPOLATION)
      Ipixelplan = Ic.getValue(i2,j2);
    else if (interp == SIMPLE)
      Ipixelplan = Ic[(unsigned int)i2][(unsigned int)j2];
    return true;
  }
  else
    return false;
}

bool
vpImageSimulator::getPixel(vpImage<vpRGBa> &Isrc, const vpImagePoint &iP,
			   vpRGBa &Ipixelplan)
{
  //test si pixel dans zone projetee
  if(!T1.inTriangle(iP) && !T2.inTriangle(iP))
    return false;

  //methoed algebrique
  double z;

  //calcul de la profondeur de l'intersection
  z = distance/(normal_Cam_optim[0]*iP.get_u()+normal_Cam_optim[1]*iP.get_v()+normal_Cam_optim[2]);
  //calcul coordonnees 3D intersection
  Xinter_optim[0]=iP.get_u()*z;
  Xinter_optim[1]=iP.get_v()*z;
  Xinter_optim[2]=z;

  //recuperation des coordonnes de l'intersection dans le plan objet
  //repere plan object : 
  //	centre = X0_2_optim[i] (premier point definissant le plan)
  //	base =  u:(X[1]-X[0]) et v:(X[3]-X[0])
  //ici j'ai considere que le plan est un rectangle => coordonnees sont simplement obtenu par un produit scalaire
  double u = 0, v = 0;
  double diff = 0;
  for(unsigned int i = 0; i < 3; i++)
  {
    diff = (Xinter_optim[i]-X0_2_optim[i]);
    u += diff*vbase_u_optim[i];
    v += diff*vbase_v_optim[i];
  }
  u = u/(euclideanNorm_u*euclideanNorm_u);
  v = v/(euclideanNorm_v*euclideanNorm_v);

  if( u > 0 && v > 0 && u < 1. && v < 1.)
  {
    double i2,j2;
    i2=v*(Isrc.getHeight()-1);
    j2=u*(Isrc.getWidth()-1);
    if (interp == BILINEAR_INTERPOLATION)
      Ipixelplan = Isrc.getValue(i2,j2);
    else if (interp == SIMPLE)
      Ipixelplan = Isrc[(unsigned int)i2][(unsigned int)j2];
    return true;
  }
  else
    return false;
}

bool 
vpImageSimulator::getPixelDepth(const vpImagePoint &iP, double &Zpixelplan)
{
  //test si pixel dans zone projetee
  if(!T1.inTriangle(iP) && !T2.inTriangle(iP))
    return false;

  Zpixelplan = distance/(normal_Cam_optim[0]*iP.get_u()+normal_Cam_optim[1]*iP.get_v()+normal_Cam_optim[2]);
  return true;
}

bool
vpImageSimulator::getPixelVisibility(const vpImagePoint &iP, 
				     double &Visipixelplan)
{
  //test si pixel dans zone projetee
  if(!T1.inTriangle(iP) && !T2.inTriangle(iP))
    return false;
  
  Visipixelplan = visible_result;
  return true;
}

void
vpImageSimulator::project(const vpColVector &_vin, 
			  const vpHomogeneousMatrix &_cMt, vpColVector &_vout)
{
  vpColVector XH(4);
  getHomogCoord(_vin,XH);
  getCoordFromHomog(_cMt*XH,_vout);
}

void
vpImageSimulator::getHomogCoord(const vpColVector &_v, vpColVector &_vH)
{
  for(unsigned int i=0;i<3;i++)
    _vH[i]=_v[i];
  _vH[3]=1.;	
}

void
vpImageSimulator::getCoordFromHomog(const vpColVector &_vH, vpColVector &_v)
{
  for(unsigned int i=0;i<3;i++)
    _v[i]=_vH[i]/_vH[3];
}


void
vpImageSimulator::getRoi(const unsigned int &Iwidth, 
			 const unsigned int &Iheight, 
			 const vpCameraParameters &cam, 
			 vpPoint* point, vpRect &rectangle)
{
  double top = Iheight+1;
  double bottom = -1;
  double right = -1;
  double left= Iwidth+1;
  for( int i = 0; i < 4; i++)
  {
    double u=0,v=0;
    vpMeterPixelConversion::convertPoint(cam,point[i].get_x(),point[i].get_y(),u,v);
    if (v < top) top = v;
    if (v > bottom) bottom = v;
    if (u < left) left = u;
    if (u > right) right = u;
  }
  if (top < 0) top = 0;
  if(top >= Iheight) top = Iheight-1;
  if (bottom < 0) bottom = 0;
  if(bottom >= Iheight) bottom = Iheight-1;
  if(left < 0) left = 0;
  if(left >= Iwidth) left = Iwidth-1;
  if(right < 0) right = 0;
  if(right >= Iwidth) right = Iwidth-1;
    
  rectangle.setTop(top);
  rectangle.setBottom(bottom);
  rectangle.setLeft(left);
  rectangle.setRight(right);
}
