/****************************************************************************
 *
 * $Id: SickLDMRS-Process.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Sick LD-MRS laser driver.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/


/*!
  \example SickLDMRS-Process.cpp

  \brief Example that shows how to acquire Sick LD-MRS laser measurements.

  This example shows by multithreading how to:
  - acquire Sick LD-MRS laser measurements
  - how to process the data in order to display the laser scans and how 
    to save the scan data in a file
  - and how to acquire and display images from a firewire camera if connected.

  \warning For the moment, this example is only working on UNIX
  platforms since the Sick LD-MRS driver was not ported to Windows.

  Concerning the laser, layer 1 is displayed in red, layer 2 in green,
  l ayer 3 in blue and layer 4 in yellow.

  Thanks to the -layer command line option, this example allows to
  select the layers to proceed.

*/
#include <visp/vpDebug.h>
#include <visp/vpImagePoint.h>
#include <visp/vpSickLDMRS.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplay.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpParseArgv.h>
#include <visp/vp1394TwoGrabber.h>
#include <visp/vpIoTools.h>

#if defined(UNIX) && (defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined(VISP_HAVE_GTK)) 

static int save = 0;
static int layerToDisplay = 0xF; // 0xF = 1111 => all the layers are selected
static vpLaserScan shm_laserscan[4];
double time_offset = 0;
#ifdef VISP_HAVE_PTHREAD
pthread_mutex_t shm_mutex;
#endif
std::string output_path;


void *laser_display_and_save_loop(void *)
{
  vpImage<unsigned char> map(700, 300);
  map = 0;
  unsigned int width = map.getWidth();
  unsigned int height = map.getHeight();
  vpImagePoint O; // Beam origin
  O.set_i(height);
  O.set_j(width/2.);
  vpScanPoint p;
  vpColor color[4]; // one color per layer
  char filename[FILENAME_MAX];
  std::ofstream fdscan;
  vpLaserScan laserscan[4];

  for (int layer=0; layer<4; layer++) {
    switch (layer) {
    case 0: color[layer] = vpColor::red; break;
    case 1: color[layer] = vpColor::green; break;
    case 2: color[layer] = vpColor::blue; break;
    case 3: color[layer] = vpColor::yellow; break;
    }
  }

  vpDisplay *display;
#if defined VISP_HAVE_X11
  display = new vpDisplayX;
#elif defined VISP_HAVE_GDI
  display = new vpDisplayGDI;
#elif defined VISP_HAVE_GTK
  display = new vpDisplayGTK;
#endif
  display->init (map, 10, 10, "Laser scan");

  unsigned int iter = 0;
  for ( ; ; ) {
    
#if (defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined (VISP_HAVE_GTK) )
    vpDisplay::display(map);
#endif
    
#ifdef VISP_HAVE_PTHREAD
    pthread_mutex_lock(&shm_mutex);
#endif
    for (int layer=0; layer < 4; layer++)
      laserscan[layer] = shm_laserscan[layer];
#ifdef VISP_HAVE_PTHREAD
    pthread_mutex_unlock(&shm_mutex);
#endif

    //     std::cout << "laser start timestamp "
    // 	      << laserscan[0].getStartTimestamp() - time_offset << std::endl;
    
    // Parse the four layers
    for (int layer=0; layer<4; layer++) {
      if (! ((0x1<<layer) & layerToDisplay)) {
        std::cout << "Layer " << layer+1 << " is not displayed" << std::endl;
        continue;
      }
      
      std::vector<vpScanPoint> pointsLayer = laserscan[layer].getScanPoints();

      if (save) {
        // Set the scan data filename to store the measures
        sprintf(filename, "%s/scan%04d-layer%d.txt",
                output_path.c_str(), iter, layer+1);
        fdscan.open(filename);

        // Write the file header
        fdscan << "# Scan layer [1 to 4] : " << layer+1 << std::endl
               << "# Start timestamp (s) : "
               << laserscan[layer].getStartTimestamp() - time_offset
               << std::endl
               << "# End timestamp (s)   : "
               << laserscan[layer].getEndTimestamp() - time_offset
               << std::endl
               << "# Data : \"radial distance (m)\" \"horizontal angle (rad)\" \"vertical angle (rad)\" \"X (m)\" \"Y (m)\" \"Z (m)\""
               << std::endl;
      }
      

      vpImagePoint E; // Beam echo
      double resolution = 5; // 100 pixels = 1 meter - increase this value to see better near info
      //       std::cout << "display layer " << layer << " nb points: "
      // 		<< pointsLayer.size() << std::endl;
      for (unsigned int i=0; i<pointsLayer.size(); i++) {
        p = pointsLayer[i];
        E.set_i(height   - resolution * p.getRadialDist() * cos(p.getHAngle()));
        E.set_j(width/2. - resolution * p.getRadialDist() * sin(p.getHAngle()));
        //std::cout << "E: " << E << std::endl;
#if (defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined (VISP_HAVE_GTK) )
        vpDisplay::displayLine(map, O, E, color[layer]);
#endif
        if (save) {
          // Save the measures in the file
          fdscan << p << std::endl;
        }
      }
      if (save) {
        fdscan.close();
      }
    }
#if (defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined (VISP_HAVE_GTK) )
    vpDisplay::flush(map);
#endif
    iter ++;
    //std::cout << "display time: " << vpTime::measureTimeMs() - t1 << std::endl;

  }
  delete display;
  return NULL;
}

void *laser_acq_loop(void *)
{
  std::string ip = "131.254.12.119";

  vpSickLDMRS laser;
  laser.setIpAddress(ip);
  laser.setup();
  vpLaserScan laserscan[4];

  unsigned int iter = 0;
  for ( ; ; ) {
    double t1 = vpTime::measureTimeMs();
    if (laser.measure(laserscan) == false)
      continue;

#ifdef VISP_HAVE_PTHREAD
    pthread_mutex_lock(&shm_mutex);
#endif
    for (int layer=0; layer < 4; layer++)
      shm_laserscan[layer] = laserscan[layer];
#ifdef VISP_HAVE_PTHREAD
    pthread_mutex_unlock(&shm_mutex);
#endif

    iter ++;
    std::cout << "laser acq time: " << vpTime::measureTimeMs() - t1 << std::endl;

  }

  return NULL;
}

void *camera_acq_and_display_loop(void *)
{
#ifdef VISP_HAVE_DC1394_2
  try {
    // Initialize the firewire framegrabber
    vp1394TwoGrabber g;       // Create a grabber based on libdc1394-2.x third party lib

    // If no camera found return
    if (g.getNumCameras() == 0)
      return NULL;

    //     g.setVideoMode(vp1394TwoGrabber::vpVIDEO_MODE_640x480_MONO8);
    //     g.setFramerate(vp1394TwoGrabber::vpFRAMERATE_60);

    vpImage<unsigned char> I; // Create a gray level image container
    vpImage<unsigned char> Q; // Create a quarter size gray level image container
    g.acquire(I);                        // Acquire an image
    I.quarterSizeImage(Q);

    vpDisplay *display;
#if defined VISP_HAVE_X11
    display = new vpDisplayX;
#elif defined VISP_HAVE_GDI
    display = new vpDisplayGDI;
#elif defined VISP_HAVE_GTK
    display = new vpDisplayGTK;
#endif
    display->init (Q, 320, 10, "Camera");
    
    // Create a file with cameraimage time stamps
    std::ofstream fdimage_ts;
    if (save) {
      std::string filename = output_path + "/image_timestamp.txt";
      fdimage_ts.open( filename.c_str() );
      fdimage_ts << "# [image name] [time stamp in second]" << std::endl;
    }
    dc1394video_frame_t *frame;
    unsigned iter = 0;
    char filename[FILENAME_MAX];
    uint64_t timestamp;
    uint32_t id;
    double image_timestamp;
    for ( ; ; ) {
      frame = g.dequeue(I, timestamp, id); // Acquire an image
      I.quarterSizeImage(Q);
      image_timestamp = timestamp/1000000. - time_offset;
      std::cout << "camera timestamp: " << image_timestamp << " s " << std::endl;
      if (save) {
        // Set the image filename
        sprintf(filename, "%s/image%04d.png", output_path.c_str(), iter);
        vpImageIo::write(Q, filename);
        fdimage_ts << filename << " " << image_timestamp << std::endl;
      }
#if (defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI) || defined (VISP_HAVE_GTK) )
      vpDisplay::display(Q);
      vpDisplay::flush(Q);
#endif
      g.enqueue(frame);

      iter ++;
    }
    delete display;
    if (save) {
      fdimage_ts.close();
    }
  }
  catch(...) {

  }
#endif
  return NULL;
}


int main(int argc, const char ** argv)
{
  output_path = "data";
  // Test if the output path directory exist. If no try to create it
  if (vpIoTools::checkDirectory(output_path) == false) {
    try {
      // Create a directory with name "username"
      vpIoTools::makeDirectory(output_path);
    }
    catch (...) {
      std::cout << "Cannot create " << output_path << " directory" << std::endl;
      return false;
    }
  }

  // Parse the command line to set the variables
  vpParseArgv::vpArgvInfo argTable[] =
  {
    {"-layer", vpParseArgv::ARGV_INT, (char*) NULL, (char *) &layerToDisplay,
     "The layer to display:\n"
     "\t\t. 0x1 for layer 1.\n"
     "\t\t. 0x2 for layer 2.\n"
     "\t\t. 0x4 for layer 3.\n"
     "\t\t. 0x8 for layer 4.\n"
     "\t\tTo display all the layers you should set 0xF value."
    },
    {"-save", vpParseArgv::ARGV_INT, (char*) NULL, (char *) &save,
     "Turn to 1 in order to save data."
    },
    {"-h", vpParseArgv::ARGV_HELP, (char*) NULL, (char *) NULL,
     "Display one or more measured layers form a Sick LD-MRS laser scanner."},
    {(char*) NULL, vpParseArgv::ARGV_END, (char*) NULL, (char*) NULL, (char*) NULL}
  } ;

  // Read the command line options
  if(vpParseArgv::parse(&argc, argv, argTable, 
                        vpParseArgv::ARGV_NO_LEFTOVERS |
                        vpParseArgv::ARGV_NO_ABBREV |
                        vpParseArgv::ARGV_NO_DEFAULTS)) {
    return (false);
  }
  
  time_offset = vpTime::measureTimeSecond();
#ifdef VISP_HAVE_PTHREAD
  pthread_t thread_camera_acq;
  pthread_t thread_laser_acq;
  pthread_t thread_laser_display;
  pthread_create(&thread_camera_acq, NULL, &camera_acq_and_display_loop, NULL);
  pthread_create(&thread_laser_acq, NULL, &laser_acq_loop, NULL);
  pthread_create(&thread_laser_display, NULL, &laser_display_and_save_loop, NULL);
  pthread_join(thread_camera_acq, 0);
  pthread_join(thread_laser_acq, 0);
  pthread_join(thread_laser_display, 0);
#endif

  return 0;
}

#else // #ifdef UNIX and display

int main()
{
  std::cout << "This example is only working on UNIX platforms \n"
            << "since the Sick LD-MRS driver was not ported to Windows."
            << std::endl;
  return 0;
}

#endif // #ifdef UNIX
