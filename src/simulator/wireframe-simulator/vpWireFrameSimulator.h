/****************************************************************************
 *
 * $Id: vpWireFrameSimulator.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Wire frame simulator
 *
 * Authors:
 * Nicolas Melchior
 *
 *****************************************************************************/


#ifndef vpWireFrameSimulator_HH
#define vpWireFrameSimulator_HH

/*!
  \file vpWireFrameSimulator.h
  \brief Implementation of a wire frame simulator.
*/
#include <visp/vpConfig.h>
#include <stdio.h>
#include <iostream>
#include <cmath>    // std::fabs
#include <limits>   // numeric_limits
#include <list>
#include <string>

extern "C" {
#include <visp/vpMy.h>
#include <visp/vpArit.h>
#include <visp/vpBound.h>
#include <visp/vpView.h>
#include <visp/vpToken.h>
#include <visp/vpTmstack.h>
#include <visp/vpVwstack.h>

int open_display();
int close_display();
int open_clipping();
int close_clipping();
int open_keyword (Keyword *kwp);
int open_lex (void);
int open_source (FILE *fd, const char *str);
int malloc_Bound_scene (Bound_scene *bsp, const char *name,Index bn);
int free_Bound_scene (Bound_scene *bsp);
int parser (Bound_scene *bsp);
int close_source (void);
int close_lex (void);
int close_keyword (void);
void add_rfstack (int i);
void load_rfstack (int i);
void add_vwstack (const char* path, ... );
void display_scene(Matrix mat, Bound_scene sc);
int * get_rfstack (void);
Matrix	* get_tmstack (void);
int View_to_Matrix (View_parameters *vp, Matrix m);
void postmult_matrix (Matrix a, Matrix b);
Bound *clipping_Bound (Bound *bp, Matrix m);
int set_Bound_face_display (Bound *bp, Byte b);
int point_3D_2D (Point3f *p3, Index size, unsigned int xsize, unsigned int ysize, Point2i *p2);
void point_3D_4D (Point3f *p3, int size, Matrix m, Point4f *p4);
int wireframe_Face (Face *fp, Point2i *pp);
}

#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpDisplay.h>
#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
# include <visp/vpList.h>
#endif
#include <visp/vpImagePoint.h>
#include <visp/vpImageSimulator.h>

void set_scene (const char*, Bound_scene *, float);
void vp2jlc_matrix (const vpHomogeneousMatrix, Matrix&);


/*!
  \class vpWireFrameSimulator

  \ingroup VisuRobotSimu
  
  \brief Implementation of a wire frame simulator. Compared to the vpSimulator class, it does not
  require third party libraries to be used.
  
  The simulator uses several frames to display the scene in the image. There are:
  
  - The world frame : This is a fixed frame used to compute the position of the other frames against each other. By default this frame is positionned at the same location as the initial object frame position.
  
  - The object frame : It is the frame linked to the object.
  
  - The camera frame : It is the frame linked to the main camera.
  
  - The external camera frame : It is the frame which corresponds to one external point of view.
  
  The most used homogeneous matrices which describes the displacement between two frames are :
  
  - fMo which is the displacement between the world frame and the object frame.
  
  - cMo which is the displacement between the main camera frame and the object frame.
  
  - cextMf which is the displacement between one external camera frame and the world frame.
  
  The following picture summarize all the previous informations.
  
  \image html vpWireFrameSimulator.jpeg
  \image latex vpWireFrameSimulator.ps
  
  The simulator uses .bnd or .wrl files as 3D scene descriptors. Several scenes can be found in the data folder which is in the directory where you build ViSP.
  
  You can move the main external view while clicking in the image. The left click enables to turn, the middle button enables to zoom and the left to translate along x and y.
  
  The simulator is able to take into account to camera parameters. You can set the internal and external cameras parameters thanks to a vpCameraParameters.
  
  The following example shows how it is easy to use.
  
  \code
  
  #include <visp/vpImage.h>
  #include <visp/vpWireFrameSimulator.h>
  
  int main()
  {
    vpWireFrameSimulator sim;
    
    vpImage<vpRGBa> Iint(480,640,255);
    vpImage<vpRGBa> Iext(480,640,255);
    
    //Set the type of scene to use
    sim.initScene(vpWireFrameSimulator::PLATE, vpWireFrameSimulator::D_STANDARD);
    
    //Set the initial pose of the camera
    sim.setCameraPositionRelObj(vpHomogeneousMatrix(0,0,0.5,vpMath::rad(0),vpMath::rad(10),0));
    //Set the desired pose of the camera (for the internal view)
    sim.setDesiredCameraPosition(vpHomogeneousMatrix(0.0,0,0.5,0,0,0));
    //Set the pose of the reference frame (for the external view)
    sim.setExternalCameraPosition(vpHomogeneousMatrix(0.1,0,0.2,0,0,0));
    
    //Set the camera parameters
    vpCameraParameters camera(1000,1000,320,240);
    sim.setInternalCameraParameters(camera);
    sim.setExternalCameraParameters(camera);
    
    //Get the internal view
    sim.getInternalImage(Iint);
    
    //Get the external view
    sim.getExternalImage(Iext);
    
    return 0;
  }
  \endcode
*/

class VISP_EXPORT vpWireFrameSimulator
{
  public:
    
    /*!
      Type of scene used to display the object at the current position.
    */
    typedef enum  
    {
      THREE_PTS,
      CUBE,
      PLATE,
      SMALL_PLATE,
      RECTANGLE,
      SQUARE_10CM,
      DIAMOND,
      TRAPEZOID,
      THREE_LINES,
      ROAD,
      TIRE,
      PIPE,
      CIRCLE,
      SPHERE,
      CYLINDER,
      PLAN,
      POINT_CLOUD,
    } vpSceneObject;
    
    /*!
      Type of scene used to display the object at the desired pose (in the internal view).
      
      - D_STANDARD will use the vpSceneObject used to be the object at the current position.
      - D_OUTIL will display a tool which is attached to the camera.
    */
    typedef enum  
    {
      D_STANDARD,
      D_CIRCLE,
      D_TOOL
    } vpSceneDesiredObject;
    
    typedef enum  
    {
      CT_LINE,
      CT_POINT
    } vpCameraTrajectoryDisplayType;
    
    
  protected:
    Bound_scene scene;
    Bound_scene desiredScene;
    Bound_scene camera;
    std::list<vpImageSimulator> objectImage;
    
    
    vpHomogeneousMatrix fMo;
    vpHomogeneousMatrix fMc;
    vpHomogeneousMatrix camMf;
    vpHomogeneousMatrix refMo;
    vpHomogeneousMatrix cMo;
    vpHomogeneousMatrix cdMo;
    
    vpSceneObject object;
    vpSceneDesiredObject desiredObject;
    
    vpColor camColor;
    vpColor camTrajColor;
    vpColor curColor;
    vpColor desColor;
    
    bool sceneInitialized;
    
    bool displayCameraTrajectory;
    std::list<vpImagePoint> cameraTrajectory;
    std::list<vpHomogeneousMatrix> poseList;
    std::list<vpHomogeneousMatrix> fMoList;
    unsigned int nbrPtLimit;
    
    
    vpImagePoint old_iPr;
    vpImagePoint old_iPz;
    vpImagePoint old_iPt;
    bool blockedr;
    bool blockedz;
    bool blockedt;
    bool blocked;
    
    vpHomogeneousMatrix camMf2;
    vpHomogeneousMatrix  f2Mf;
    
    double px_int;
    double py_int;
    double px_ext;
    double py_ext;
    
    bool displayObject;
    bool displayDesiredObject;
    bool displayCamera;
    bool displayImageSimulator;
    
    float cameraFactor;
    
    vpCameraTrajectoryDisplayType camTrajType;
    
    bool extCamChanged;
    
    vpHomogeneousMatrix rotz;

 private:
    std::string scene_dir;
  
  public:
    vpWireFrameSimulator();
    virtual ~vpWireFrameSimulator();
    
    void initScene(vpSceneObject obj, vpSceneDesiredObject desiredObject);
    void initScene(const char* obj, const char* desiredObject);
    void initScene(vpSceneObject obj);
    void initScene(const char* obj);

    void initScene(vpSceneObject obj, vpSceneDesiredObject desiredObject, const std::list<vpImageSimulator> &imObj);
    void initScene(const char* obj, const char* desiredObject, const std::list<vpImageSimulator> &imObj);
    void initScene(vpSceneObject obj, const std::list<vpImageSimulator> &imObj);
    void initScene(const char* obj, const std::list<vpImageSimulator> &imObj);
    
    /*!
      Set the position of the camera relative to the object.
      
      \param cMo : The pose of the camera.
    */
    void setCameraPositionRelObj(const vpHomogeneousMatrix cMo) {this->cMo = rotz * cMo; fMc = fMo*this->cMo.inverse();}
    
    /*!
      Set the position of the the world reference frame relative to the camera.
      
      \param fMc : The pose of the camera.
    */
    void setCameraPositionRelWorld(const vpHomogeneousMatrix fMc) {this->fMc = fMc*rotz; cMo = this->fMc.inverse()*fMo;}
    
    /*!
      Set the desired position of the camera relative to the object.
      
      \param cdMo : The desired pose of the camera.
    */
    void setDesiredCameraPosition(const vpHomogeneousMatrix cdMo) {this->cdMo = rotz * cdMo;}
    
    /*!
      Set the external camera point of view.
      
      \param camMf : The pose of the external camera relative to the world reference frame.
    */
    void setExternalCameraPosition(const vpHomogeneousMatrix camMf) 
    {
      this->camMf = rotz * camMf;
      vpTranslationVector T;
      this->camMf.extract (T);
      this->camMf2.buildFrom(0,0,T[2],0,0,0);
      f2Mf = camMf2.inverse()*this->camMf;
      extCamChanged = true;
    }
    
    /*!
      Get the main external camera's position relative to the the world reference frame.
      
      \return the main external camera position relative to the the world reference frame.
    */
    inline vpHomogeneousMatrix getExternalCameraPosition() const { return rotz * camMf;}
    
    /*!
      Set the color used to display the camera in the external view.
      
      \param col : The desired color.
    */
    void setCameraColor(const vpColor col) {camColor = col;}
    
    /*!
      Set the color used to display the camera trajectory in the external view.
      
      \param col : The desired color.
    */
    void setCameraTrajectoryColor(const vpColor col) {camTrajColor = col;}
    
    /*!
      Set the color used to display the object at the current position.
      
      \param col : The desired color.
    */
    void setCurrentViewColor(const vpColor col) {curColor = col;}
    
    /*!
      Set the color used to display the object at the desired position.
      
      \param col : The desired color.
    */
    void setDesiredViewColor(const vpColor col) {desColor = col;}
    
    /*!
      Enable or disable the displaying of the camera trajectory in the main external camera view.
      
      By default the trajectory is displayed.
      
      \param displayCameraTrajectory : Set to true to display the camera trajectory.
    */
    void setDisplayCameraTrajectory (const bool displayCameraTrajectory) {this->displayCameraTrajectory = displayCameraTrajectory;}
    
    /*!
      Get the parameters of the virtual internal camera.
      
      \param I : The image used to display the view of the camera.
      
      \return It returns the camera parameters.
    */
    vpCameraParameters getInternalCameraParameters(const vpImage<vpRGBa> &I) const {
      //if(px_int != 1 && py_int != 1)
      // we assume px_int and py_int > 0
      if( (std::fabs(px_int-1.) > vpMath::maximum(px_int,1.)*std::numeric_limits<double>::epsilon()) 
	  && (std::fabs(py_int-1) > vpMath::maximum(py_int,1.)*std::numeric_limits<double>::epsilon()))
        return vpCameraParameters(px_int,py_int,I.getWidth()/2,I.getHeight()/2);
      else
      {
        unsigned int size = vpMath::minimum(I.getWidth(),I.getHeight())/2;
        return vpCameraParameters(size,size,I.getWidth()/2,I.getHeight()/2);
      }
    }
    
    /*!
      Get the parameters of the virtual internal camera.
      
      \param I : The image used to display the view of the camera.
      
      \return It returns the camera parameters.
    */
    vpCameraParameters getInternalCameraParameters(const vpImage<unsigned char> &I) const {
      //if(px_int != 1 && py_int != 1)
      // we assume px_int and py_int > 0
      if( (std::fabs(px_int-1.) > vpMath::maximum(px_int,1.)*std::numeric_limits<double>::epsilon()) 
	  && (std::fabs(py_int-1) > vpMath::maximum(py_int,1.)*std::numeric_limits<double>::epsilon()))
        return vpCameraParameters(px_int,py_int,I.getWidth()/2,I.getHeight()/2);
      else
      {
        unsigned int size = vpMath::minimum(I.getWidth(),I.getHeight())/2;
        return vpCameraParameters(size,size,I.getWidth()/2,I.getHeight()/2);
      }
    }
    
    /*!
      Get the parameters of the virtual external camera.
      
      \param I : The image used to display the view of the camera.
      
      \return It returns the camera parameters.
    */
    vpCameraParameters getExternalCameraParameters(const vpImage<vpRGBa> &I) const {
      //if(px_ext != 1 && py_ext != 1)
      // we assume px_ext and py_ext > 0
      if( (std::fabs(px_ext-1.) > vpMath::maximum(px_ext,1.)*std::numeric_limits<double>::epsilon()) 
	  && (std::fabs(py_ext-1) > vpMath::maximum(py_ext,1.)*std::numeric_limits<double>::epsilon()))
        return vpCameraParameters(px_ext,py_ext,I.getWidth()/2,I.getHeight()/2);
      else
      {
        unsigned int size = vpMath::minimum(I.getWidth(),I.getHeight())/2;
        return vpCameraParameters(size,size,I.getWidth()/2,I.getHeight()/2);
      }
    }
    
    /*!
      Get the parameters of the virtual external camera.
      
      \param I : The image used to display the view of the camera.
      
      \return It returns the camera parameters.
    */
    vpCameraParameters getExternalCameraParameters(const vpImage<unsigned char> &I) const {
      //if(px_ext != 1 && py_ext != 1)
      // we assume px_ext and py_ext > 0
      if( (std::fabs(px_ext-1.) > vpMath::maximum(px_ext,1.)*std::numeric_limits<double>::epsilon()) 
	  && (std::fabs(py_ext-1) > vpMath::maximum(py_ext,1.)*std::numeric_limits<double>::epsilon()))
        return vpCameraParameters(px_ext,py_ext,I.getWidth()/2,I.getHeight()/2);
      else
      {
        unsigned int size = vpMath::minimum(I.getWidth(),I.getHeight())/2;
        return vpCameraParameters(size,size,I.getWidth()/2,I.getHeight()/2);
      }
    }
    
    /*!
      Set the pose between the object and the fixed world frame.
      
      \param fMo : The pose between the object and the fixed world frame.
    */
    void set_fMo(const vpHomogeneousMatrix &fMo) {this->fMo = fMo;this->cMo = fMc.inverse()*fMo;}
    
    /*!
      Get the pose between the object and the fixed world frame.
      
      \return The pose between the object and the fixed world frame.
    */
    vpHomogeneousMatrix get_fMo() const {return fMo;}
    
    /*!
      Get the pose between the object and the camera.
      
      \return The pose between between the object and the camera.
    */
    vpHomogeneousMatrix get_cMo() const {return rotz*cMo;}
    
    /*!
      Set the internal camera parameters.
      
      \param cam : The desired camera parameters.
    */
    inline void setInternalCameraParameters(const vpCameraParameters cam) {
      px_int = cam.get_px();
      py_int = cam.get_py();
    }
    
    /*!
      Set the internal camera parameters.
      
      \param cam : The desired camera parameters.
    */
    inline void setExternalCameraParameters(const vpCameraParameters cam) {
      px_ext = cam.get_px();
      py_ext = cam.get_py();
    }
    
    /*!
      Set the maximum number of main camera's positions which are stored. Those position can be displayed in the external camera field of view. By default this parameter is set to 1000. 
      
      \param nbPt : The desired number of position which are saved.
    */
    inline void setNbPtTrajectory(const unsigned int nbPt) {nbrPtLimit = nbPt;}
    
    /*!
      Set the parameter which enables to choose the size of the main camera in the external camera views. By default this parameter is set to 1.
      
      \param factor : The ration for the camera size.
    */
    inline void setCameraSizeFactor (const float factor) {cameraFactor = factor;}
    
    /*!
      Delete the history of the main camera position which are displayed in the external views.
    */
    inline void deleteCameraPositionHistory() {
      cameraTrajectory.clear();
      poseList.clear();
      fMoList.clear();}
      
    /*!
      Set the way to display the history of the main camera trajectory in the main external view. The choice is given between displaying lines and points.
      
      \param camTrajType : The chosen way to display the camera trajectory.
    */
    inline void setCameraTrajectoryDisplayType (const vpCameraTrajectoryDisplayType camTrajType) {this->camTrajType = camTrajType;}
    
    /*!
      Get the homogeneous matrices cMo stored to display the camera trajectory.
      
      \param cMo_history : The list of the homogeneous matrices cMo.
    */
    void get_cMo_History(std::list<vpHomogeneousMatrix>& cMo_history) {
      cMo_history.clear();
      for(std::list<vpHomogeneousMatrix>::const_iterator it=poseList.begin(); it!=poseList.end(); ++it){
        cMo_history.push_back(rotz*(*it));
      }
    }
    
    /*!
      Get the homogeneous matrices fMo stored to display the camera trajectory.
      
      \param fMo_history : The list of the homogeneous matrices fMo.
    */
    void get_fMo_History(std::list<vpHomogeneousMatrix>& fMo_history) {fMo_history = fMoList;}
    
    void displayTrajectory(vpImage<unsigned char> &I, const std::list<vpHomogeneousMatrix> &list_cMo, const std::list<vpHomogeneousMatrix> &list_fMo, const vpHomogeneousMatrix &camMf);
    
    void displayTrajectory(vpImage<vpRGBa> &I, const std::list<vpHomogeneousMatrix> &list_cMo, const std::list<vpHomogeneousMatrix> &list_fMo, const vpHomogeneousMatrix &camMf);
    
    void getInternalImage(vpImage<vpRGBa> &I);
    void getExternalImage(vpImage<vpRGBa> &I);
    void getExternalImage(vpImage<vpRGBa> &I, vpHomogeneousMatrix camMf);
    
    void getInternalImage(vpImage<unsigned char> &I);
    void getExternalImage(vpImage<unsigned char> &I);
    void getExternalImage(vpImage<unsigned char> &I, vpHomogeneousMatrix camMf);
    
  protected:
    void display_scene(Matrix mat, Bound_scene &sc, vpImage<vpRGBa> &I, vpColor color);
    void display_scene(Matrix mat, Bound_scene &sc, vpImage<unsigned char> &I, vpColor color);
    vpHomogeneousMatrix navigation(vpImage<vpRGBa> &I, bool &changed);
    vpHomogeneousMatrix navigation(vpImage<unsigned char> &I, bool &changed);
    vpImagePoint projectCameraTrajectory (vpImage<vpRGBa> &I, vpHomogeneousMatrix cMo, vpHomogeneousMatrix fMo);
    vpImagePoint projectCameraTrajectory (vpImage<unsigned char> &I, vpHomogeneousMatrix cMo, vpHomogeneousMatrix fMo);
    vpImagePoint projectCameraTrajectory (vpImage<vpRGBa> &I, vpHomogeneousMatrix cMo, vpHomogeneousMatrix fMo, vpHomogeneousMatrix cMf);
    vpImagePoint projectCameraTrajectory (vpImage<unsigned char> &I, vpHomogeneousMatrix cMo, vpHomogeneousMatrix fMo, vpHomogeneousMatrix cMf);

  public:

#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
  /*!
    @name Deprecated functions
  */

    /*!
      \deprecated This method is deprecated. You should use
      get_fMo_History(std::list<vpHomogeneousMatrix> &) instead.

      Get the homogeneous matrices fMo stored to display the camera trajectory.

      \return Returns the list of the homogeneous matrices fMo.
    */
    vp_deprecated vpList<vpHomogeneousMatrix> get_fMo_History () {
      vpList<vpHomogeneousMatrix> fMoHistory;
      for(std::list<vpHomogeneousMatrix>::const_iterator iter = fMoList.begin(); iter != fMoList.end(); ++iter){
        fMoHistory.addRight(*iter);
      }
      return fMoHistory;}

    /*!
      \deprecated This method is deprecated. You should use
      get_cMo_History(std::list<vpHomogeneousMatrix> &) instead.

      Get the homogeneous matrices cMo stored to display the camera trajectory.

       \return Returns the list of the homogeneous matrices cMo.
    */
    vp_deprecated vpList<vpHomogeneousMatrix> get_cMo_History () {
      vpList<vpHomogeneousMatrix> list_cMo;
      vpHomogeneousMatrix tmp_;
      for(std::list<vpHomogeneousMatrix>::const_iterator it=poseList.begin(); it!=poseList.end(); ++it){
        tmp_ = (rotz*(*it));
        list_cMo.addRight(tmp_);
      }
      return list_cMo;}

    vp_deprecated void initScene(vpSceneObject obj, vpSceneDesiredObject desiredObject, vpList<vpImageSimulator> &imObj);
    vp_deprecated void initScene(const char* obj, const char* desiredObject, vpList<vpImageSimulator> &imObj);
    vp_deprecated void initScene(vpSceneObject obj, vpList<vpImageSimulator> &imObj);
    vp_deprecated void initScene(const char* obj, vpList<vpImageSimulator> &imObj);

    vp_deprecated void displayTrajectory(vpImage<unsigned char> &I, vpList<vpHomogeneousMatrix> &list_cMo, vpList<vpHomogeneousMatrix> &list_fMo, vpHomogeneousMatrix camMf);
    vp_deprecated void displayTrajectory(vpImage<vpRGBa> &I, vpList<vpHomogeneousMatrix> &list_cMo, vpList<vpHomogeneousMatrix> &list_fMo, vpHomogeneousMatrix camMf);
#endif
};

#endif
