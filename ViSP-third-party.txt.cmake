                    ViSP third-party libraries

Below you will find the list of third party libraries used to 
build ViSP on your computer.

Mathematics:
  Gnu Scientific Library      : ${VISP_HAVE_GSL_FOUND}
  Lapack/blas                 : ${VISP_HAVE_LAPACK_FOUND}
Simulator:
  Ogre simulator              : ${VISP_HAVE_OGRE_FOUND}
  \- Ogre3D                   : ${VISP_HAVE_OGRE_FOUND}
  \- OIS                      : ${VISP_HAVE_OIS_FOUND}
  Coin simulator              : ${VISP_HAVE_COIN_AND_GUI_FOUND}
  \- Coin3D                   : ${VISP_HAVE_COIN_FOUND}
  \- SoWin                    : ${VISP_HAVE_SOWIN_FOUND}
  \- SoXt                     : ${VISP_HAVE_SOXT_FOUND}
  \- SoQt                     : ${VISP_HAVE_SOQT_FOUND}
  \- Qt4                      : ${VISP_HAVE_QT4_FOUND}
  \- Qt3                      : ${VISP_HAVE_QT3_FOUND}
Robots
  Afma6                       : ${VISP_HAVE_AFMA6_FOUND}
  Afma4                       : ${VISP_HAVE_AFMA4_FOUND}
  Biclops                     : ${VISP_HAVE_BICLOPS_FOUND}
  Ptu46                       : ${VISP_HAVE_PTU46_FOUND}
  Pioneer                     : ${VISP_HAVE_PIONEER_FOUND}
  Viper S850                  : ${VISP_HAVE_VIPER850_FOUND}
  Cycab                       : ${VISP_HAVE_CYCAB_FOUND}
Video devices (display)                  
  X11                         : ${VISP_HAVE_X11_FOUND}
  GTK                         : ${VISP_HAVE_GTK_FOUND} 
  OpenCV                      : ${VISP_HAVE_OPENCV_FOUND} 
  GDI                         : ${VISP_HAVE_GDI_FOUND}
  Direct3D                    : ${VISP_HAVE_D3D9_FOUND}
Framegrabbers
  Firewire libdc1394-1.x      : ${VISP_HAVE_DC1394_1_FOUND}
  Firewire libdc1394-2.x      : ${VISP_HAVE_DC1394_2_FOUND}
  Video For Linux Two         : ${VISP_HAVE_V4L2_FOUND}
  DirectShow                  : ${VISP_HAVE_DIRECTSHOW_FOUND}
  CMU 1394 Digital Camera SDK : ${VISP_HAVE_CMU1394_FOUND}
  OpenCV                      : ${VISP_HAVE_OPENCV_FOUND} 
Specific devices
  Yarp                        : ${VISP_HAVE_YARP_FOUND}
  Kinect                      : ${VISP_HAVE_LIBFREENECT_AND_DEPENDENCIES_FOUND}
  \-libfreenect               : ${VISP_HAVE_LIBFREENECT_FOUND}
  \-libusb-1.0                : ${VISP_HAVE_LIBUSB_1_FOUND}
  \-pthread                   : ${VISP_HAVE_PTHREAD_FOUND}
Video and image Read/Write:
  FFMPEG                      : ${VISP_HAVE_FFMPEG_FOUND}
  libjpeg                     : ${VISP_HAVE_LIBJPEG_FOUND}
  libpng                      : ${VISP_HAVE_LIBPNG_FOUND}
Misc:
  XML2                        : ${VISP_HAVE_XML2_FOUND}
  pthread                     : ${VISP_HAVE_PTHREAD_FOUND}
  OpenMP                      : ${VISP_HAVE_OPENMP_FOUND}
Documentation:
  Doxygen                     : ${VISP_HAVE_DOXYGEN_FOUND}
  Graphviz dot                : ${VISP_HAVE_DOT_FOUND}
ViSP built with C++11 features: ${VISP_HAVE_CPP11_COMPATIBILITY_FOUND}
