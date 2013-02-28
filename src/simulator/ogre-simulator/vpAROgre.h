/****************************************************************************
 *
 * $Id: vpAROgre.h 3677 2012-04-06 13:17:57Z bdelabar $
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
 * Augmented Reality viewer using Ogre3D.
 *
 * Authors:
 * Bertrand Delabarre
 *
 *****************************************************************************/

/*!
  \file vpAROgre.h

  \brief Class that implements augmented reality viewer based on Ogre3D.

  \warning The content of this file is only available if Ogre3D and
  one of the renderer (OpenGL or DirectX) are installed.

*/

#ifndef __VP_AROGRE__
#define __VP_AROGRE__

#include <visp/vpConfig.h>

#ifdef VISP_HAVE_OGRE
#include <list>

#include <visp/vpImage.h>
#include <visp/vpImageConvert.h>
#include <visp/vpRGBa.h>

#include <visp/vpImageTools.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpRotationMatrix.h>
#include <visp/vpRxyzVector.h>

#include <Ogre.h>
#include <OgreFrameListener.h>

#ifdef VISP_HAVE_OIS
#  include <OIS/OIS.h>
#endif

/*!
  \class vpAROgre 

  \ingroup VisuRobotSimu

  \brief Implementation of an augmented reality viewer.

  This class provides methods to show a 3D scene in a real world. To do that
  you will need to initialise it with the parameters of your camera, then each
  frame you will need to compute a pose for your camera and give it to the
  application.  With that information and the image to be shown in background
  it will set up the 3D scene correspondingly. 

*/
class VISP_EXPORT vpAROgre : public Ogre::FrameListener, public Ogre::WindowEventListener
#ifdef VISP_HAVE_OIS
			   , public OIS::KeyListener
#endif
{
 public:	
  vpAROgre(const vpCameraParameters &cam = vpCameraParameters(), 
	   unsigned int width = 640, unsigned int height = 480,
	   const char* resourcePath = 
#ifdef VISP_HAVE_OGRE_RESOURCES_PATH
	   VISP_HAVE_OGRE_RESOURCES_PATH,
#else
	   ".",
#endif
	   const char* pluginsPath = 
#ifdef VISP_HAVE_OGRE_PLUGINS_PATH
	   VISP_HAVE_OGRE_PLUGINS_PATH
#else
	   "."
#endif
	   );
	
  virtual void init(vpImage<unsigned char> &I, bool bufferedKeys=false);
  virtual void init(vpImage<vpRGBa> &I, bool bufferedKeys=false);

  virtual ~vpAROgre(void);

  virtual bool customframeStarted(const Ogre::FrameEvent& evt);

  virtual bool customframeEnded(const Ogre::FrameEvent& evt);
	
#ifdef VISP_HAVE_OIS
  /**
   * Default event handler
   */
  virtual bool keyPressed( const OIS::KeyEvent & /*e*/) { return true; }
  /**
   * Default event handler
   */
  virtual bool keyReleased( const OIS::KeyEvent & /*e*/) {  return true; }
#endif

  virtual void windowClosed(Ogre::RenderWindow* rw);

  virtual void display(const vpImage<unsigned char> &I, 
           const vpHomogeneousMatrix &cMw);

  virtual void display(const vpImage<vpRGBa> &I, 
           const vpHomogeneousMatrix &cMw);

  void setCameraParameters(const vpCameraParameters &cameraP);

  void load(const std::string &name, const std::string &model);

  void setPosition(const std::string &name,
       const vpTranslationVector &wTo);
  void setPosition(const std::string &name, const vpHomogeneousMatrix &wMo);

  vpTranslationVector getPosition(const std::string &name)const;

  void setRotation(const std::string &name, const vpRotationMatrix &wRo);

  void addRotation(const std::string &name, const vpRotationMatrix &wRo);


  void setVisibility(const std::string &name, bool isVisible);

  void setScale(const std::string &name, const float factorx, const float factory, const float factorz);

  bool continueRendering(void);

  void getRenderingOutput(vpImage<vpRGBa> &I, vpHomogeneousMatrix &cMo);
  /*!
    Set the width of the grabbed image. 
  
    \warning To be effective, this method must be called before the init() one. 
  
    \param width : Width of the grabbed image.
  */
  inline void setImageWidth(const unsigned int width){
    mWidth =width;
  }
  
  /*!
    Set the height of the grabbed image. 
  
    \warning To be effective, this method must be called before the init() one. 
  
    \param height : Height of the grabbed image.
  */
  inline void setImageHeight(const unsigned int height){
    mHeight = height;
  }
  
  /*!
    Enable/Disable the apparition of the config dialog on startup.
  
    \warning To be effective, this method must be called before the init() one. 
  
    \param showConfigDialog : if true, shows the dialog window (used to set the 
    display options)
  */
  inline void setShowConfigDialog(const bool showConfigDialog){
    mshowConfigDialog = showConfigDialog;
  }

  /*!
    Set the resource path used to locate the resource.cfg file.
  
    By default, this path is set to VISP_HAVE_OGRE_RESOURCES_PATH defined in vpConfig.h

    \warning To be effective, this method must be called before the init() one. 
  
    \param resourcePath : The new resource path (must not have a terminate /).
  */
  inline void setResourcePath(const char* resourcePath){
    mResourcePath = resourcePath;
  }

  /*!
    Set the plugins path used to locate the plugins.cfg file.
  
    By default, this path is set to VISP_HAVE_OGRE_PLUGINS_PATH defined in vpConfig.h

    \warning To be effective, this method must be called before the init() one. 
  
    \param pluginsPath : The new plugins path (must not have a terminate /).
  */
  inline void setPluginsPath(const char* pluginsPath){
    mPluginsPath = pluginsPath;
  }


  /*!
    Add optional resource location. Since a resource file cannot be always
    sufficient to manage multiple location media (depending on the computer and 
    the executable path), this method may be used to add such paths.
    
    \warning To be effective, this method must be called before the init() one. 
    
    \param resourceLocation : The resource location (it may be a folder or a zip
    file).
  */
  inline void addResource(const std::string& resourceLocation){
    mOptionnalResourceLocation.push_back(resourceLocation);
  }
  
  /*!
    Set the window position in the screen.
    
    \throw vpException::notInitialised if the window has not been created (using
    the init() method).
    
    \param win_x : x coordinate of the new top left corner of the window in the 
    screen.
    \param win_y : y coordinate of the new top left corner of the window in the 
    screen.
  */
  inline void setWindowPosition(const unsigned int win_x, const unsigned int win_y){
    if(mWindow == NULL){
      throw vpException(vpException::notInitialized, "Window not initialised, cannot set its position");
    }
    mWindow->reposition(static_cast<int>(win_x), static_cast<int>(win_y));
  }
  
 protected:

  virtual void init(bool bufferedKeys=false);
  virtual void createCamera(void);

  /**
   * Build the 3D scene
   * Override this to show what you want
   */
  virtual void createScene(void){};

  virtual void closeOIS(void);

  /*!
    Update the 3D scene
    
    \return Always true.
   */
  virtual bool updateScene(const Ogre::FrameEvent & /*evt*/) {return true;};

  /*!
    Check for keyboard, mouse and joystick inputs.
    
    \return Always true.
  */
  virtual bool processInputEvent(const Ogre::FrameEvent & /*evt*/) {return true;};

  /*!
    Clean up the 3D scene.
    
    \return Always true.
  */
  virtual bool destroyScene(void) {return true;};

  virtual void updateCameraProjection(void);

  virtual void updateBackgroundTexture(const vpImage<unsigned char> &I);

  virtual void updateBackgroundTexture(const vpImage<vpRGBa> &I);

  virtual void updateCameraParameters (const vpHomogeneousMatrix &cMo);

 private:

  void createBackground(vpImage<unsigned char> &I);
  void createBackground(vpImage<vpRGBa> &I);

  bool frameStarted(const Ogre::FrameEvent& evt);

  bool frameEnded( const Ogre::FrameEvent& evt);

  bool stopTest(const Ogre::FrameEvent& evt);

 protected:
  // Attributes

  // OGRE 3D System
  Ogre::Root*	    mRoot;                         /** Application's root */
  Ogre::Camera*	    mCamera;                   /** Camera */
  Ogre::SceneManager* mSceneMgr;                 /** Scene manager */
  Ogre::RenderWindow* mWindow;                   /** Display window */
  Ogre::String	    mResourcePath;             /** Path to resources.cfg */
  Ogre::String	    mPluginsPath;             /** Path to plugins.cfg */

#ifdef VISP_HAVE_OIS
  // OIS Input manager and devices
  OIS::InputManager* mInputManager;
  OIS::Keyboard*	   mKeyboard;
#endif

  // ViSP AR System
  bool keepOn;                                   /** Has the application received a signal to stop(false) or not (true) */
  vpImage<vpRGBa> mImageRGBA;                    /** vpImage to store grabbed image */
  vpImage<unsigned char> mImage;                 /** vpImage to store grabbed image */
  Ogre::HardwarePixelBufferSharedPtr mPixelBuffer;   /** Pointer to the pixel buffer */
  Ogre::Rectangle2D* mBackground;                /** Background image */
  unsigned int mHeight;                                   /** Height of the acquired image */
  unsigned int mWidth;                                    /** Width of the acquired image */

  // Camera calculations
  vpCameraParameters mcam;                       /** The intrinsic camera parameters */
  
  bool mshowConfigDialog;                         /** if true, shows the dialog window (used to set the display options) */
  
  std::list<std::string> mOptionnalResourceLocation;  /** Optional resource location (used to load mesh and material) */

};

#endif // VISP_HAVE_OGRE

#endif
