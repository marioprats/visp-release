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

#ifndef vpImageSimulator_h
#define vpImageSimulator_h

/*!
  \class vpImageSimulator

  \ingroup ImageSimulator

  \brief Class which enables to project an image in the 3D space and get the view of a virtual camera.
  
  The image is represented by a rectangular image whose corners coordinates are known in the 3D frame linked to the 3D rectangle.
  
  The 3D rectangle is positionned relative to a virtual camera (represented by its intrinsic parameters). Indeed, the pose \f$ cMt \f$ has to be given by the user of the class.
  
  And finally, the view of the virtual camera is given by the geImage() method.
  
  You can use a colored or a gray scaled image.
  
  To avoid the aliasing especially when the camera is very near from the image plane, a bilinear interpolation can be done for every pixels which have to be filled in. By default this functionality is not used because it consumes lot of time.
  
  The  following example explain how to use the class.
  
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
    X[0][2] = 0;
  
    // Top right corner
    X[1][0] = 1;
    X[1][1] = -1;
    X[1][2] = 0;
  
    // Bottom right corner
    X[2][0] = 1;
    X[2][1] = 1;
    X[2][2] = 0;
  
    //Bottom left corner
    X[3][0] = -1;
    X[3][1] = 1;
    X[3][2] = 0;
    
    vpImageSimulator sim;
    sim.init(Iimage, X);
    
    sim.setCameraPosition(vpHomogeneousMatrix(0,0,5,vpMath::rad(60),vpMath::rad(0),0));
    
    vpCameraParameters cam(868.0, 869.0, 320, 240);
    
    sim.getImage(Icamera,cam);
    
    return 0;
  }
  \endcode
*/

#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpColVector.h>
#include <visp/vpTriangle.h>
#include <visp/vpRect.h>
#include <visp/vpImage.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpPoint.h>
#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
#  include <visp/vpList.h>
#endif

#include <vector>
#include <list>

class VISP_EXPORT vpImageSimulator
{
  public:
    typedef enum {
      COLORED,
      GRAY_SCALED
    } vpColorPlan;
    
    typedef enum {
      SIMPLE,
      BILINEAR_INTERPOLATION
    } vpInterpolationType;
    
  private:
    vpColVector X[4];
    vpPoint pt[4];
    vpHomogeneousMatrix cMt;
    vpColVector X2[4];
    
    vpInterpolationType interp;

    //normal repere objet
    vpColVector normal_obj;
    //normal repere cam
    vpColVector normal_Cam;
    //normal repere cam sous forme de pointeur sinon vpColVector prend trop de temps
    double *normal_Cam_optim;

    //distance du plan au centre de camera
    double distance;
    double visible_result;
    bool visible;


    //point 3D du plan definissant ou sera le pixel (0,0) de l'image
    double *X0_2_optim;

    //variable pour intersection algebre : vecteurs de la base 2D et leur norme
    double euclideanNorm_u,euclideanNorm_v;

    //2 vecteur 3D (typiquement deux cotes du plan) qui definissent le repere 2D u,v de l'image
    vpColVector vbase_u,vbase_v;
    //version pointeur des vecteurs de base
    double *vbase_u_optim;
    double *vbase_v_optim;

    //allocation memoire du point 3D d'intersection entre le vecteur (centre_cam - point_plan_image) et le plan
    double *Xinter_optim;

    //triangles de projection du plan
    vpTriangle T1,T2;
    
    //image de texture
    vpColorPlan colorI;
    vpImage<unsigned char> Ig;
    vpImage<vpRGBa> Ic;

    vpRect rect;
    bool cleanPrevImage;
    vpColor bgColor;
    
    vpColVector focal;
    
  public:
    vpImageSimulator(const vpColorPlan &col = COLORED);
    vpImageSimulator(const vpImageSimulator &text);
    virtual ~vpImageSimulator();
    
    vpImageSimulator& operator=(const vpImageSimulator& sim);

    
    //creation du plan a partir de ses coordonnees 3D ds repere objet et de son image texture
    void init(const vpImage<unsigned char> &I,vpColVector* _X);
    void init(const vpImage<vpRGBa> &I,vpColVector* _X);
    void init(const char* file_image,vpColVector* _X);
    void init(const vpImage<unsigned char> &I, const std::vector<vpPoint>& _X);
    void init(const vpImage<vpRGBa> &I, const std::vector<vpPoint>& _X);
    void init(const char* file_image, const std::vector<vpPoint>& _X);

    //projection du plan par cMo => creation des deux triangles definissant projection du plan sur plan image (coord en metre)
    void setCameraPosition(const vpHomogeneousMatrix &_cMt);
    
    void setInterpolationType (const vpInterpolationType interp) {this->interp = interp;}
    
    void getImage(vpImage<unsigned char> &I, const vpCameraParameters &cam);
    void getImage(vpImage<vpRGBa> &I, const vpCameraParameters &cam);
    
    void getImage(vpImage<unsigned char> &I, vpImage<unsigned char> &Isrc, 
		  const vpCameraParameters &cam);
    void getImage(vpImage<vpRGBa> &I, vpImage<vpRGBa> &Isrc, 
		  const vpCameraParameters &cam);
    
    void getImage(vpImage<unsigned char> &I, const vpCameraParameters &cam, 
		  vpMatrix &zBuffer);
    void getImage(vpImage<vpRGBa> &I, const vpCameraParameters &cam, 
		  vpMatrix &zBuffer);
    
    static void getImage(vpImage<unsigned char> &I,
                         std::list<vpImageSimulator> &list,
                         const vpCameraParameters &cam);
    static void getImage(vpImage<vpRGBa> &I,
                         std::list <vpImageSimulator> &list,
                         const vpCameraParameters &cam);

    /*!
      As it can be time consuming to reset all the image to a default baground value, this function enable to reset only the pixel which changed the previous time.
      
      By default this functionality is disabled. and the background color is white.
      
      \param clean : Enable the reset method.
      \param color : Color of the back ground.
    */
    void setCleanPreviousImage(const bool &clean, const vpColor &color = vpColor::white) {
      cleanPrevImage = clean;
      bgColor = color;
    }

#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
  /*!
    @name Deprecated functions
  */
    vp_deprecated static void getImage(vpImage<unsigned char> &I,
       vpList<vpImageSimulator> &list,
       const vpCameraParameters &cam);
    vp_deprecated static void getImage(vpImage<vpRGBa> &I, vpList<vpImageSimulator> &list,
       const vpCameraParameters &cam);
#endif
    
  private:
    void initPlan(vpColVector* _X);
    
    //result = plan est visible.
    //ie: un plan est oriente dans si normal_plan.focal < 0 => plan est visible sinon invisible.
    bool isVisible() {return visible;};
    
    //function that project a point x,y on the plane, return true if the projection is on the limited plane
    // and in this case return the corresponding image pixel Ipixelplan
    bool getPixel(const vpImagePoint &iP,unsigned char &Ipixelplan);
    bool getPixel(const vpImagePoint &iP,vpRGBa &Ipixelplan);
    bool getPixel(vpImage<unsigned char> &Isrc, const vpImagePoint &iP,
		  unsigned char &Ipixelplan);
    bool getPixel(vpImage<vpRGBa> &Isrc, const vpImagePoint &iP,
		  vpRGBa &Ipixelplan);
    bool getPixelDepth(const vpImagePoint &iP, double &Zpixelplan);
    bool getPixelVisibility(const vpImagePoint &iP, double &Zpixelplan);
    
        //operation 3D de base :
    void project(const vpColVector &_vin, const vpHomogeneousMatrix &_cMt,
		 vpColVector &_vout);
    //donne coordonnes homogenes de _v;
    void getHomogCoord(const vpColVector &_v, vpColVector &_vH);
    //donne coordonnes _v en fction coord homogenes _vH;
    void getCoordFromHomog(const vpColVector &_vH, vpColVector &_v);
    
    void getRoi(const unsigned int &Iwidth, const unsigned int &Iheight, 
		const vpCameraParameters &cam, vpPoint* pt, vpRect &rect);
};

VISP_EXPORT inline std::ostream& operator<< (std::ostream &os, const vpImageSimulator& /*ip*/)
{
  os << "";
  return os;
}


#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */


