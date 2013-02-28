/****************************************************************************
 *
 * $Id: vpMomentObject.cpp 3660 2012-03-29 10:41:21Z fspindle $
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
 * Object input structure used by moments.
 *
 * Authors:
 * Filip Novotny
 *
 *****************************************************************************/

#include <visp/vpMomentBasic.h>
#include <visp/vpMomentObject.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpPixelMeterConversion.h>
#include <visp/vpConfig.h>
#include <cmath>
#include <limits>
#ifdef VISP_HAVE_OPENMP
#include <omp.h>
#endif
#include <cassert>

/*!
  Computes moments from a vector of points describing a polygon.
  The points must be stored in a clockwise order. Used internally.
  \param p : moment order (first index)
  \param q : moment order (second index)
  \param points : vector of points in a clockwise order

  \return moment value
*/
double vpMomentObject::calc_mom_polygon(unsigned int p, unsigned int q, const std::vector<vpPoint>& points){
    unsigned int i,k,l;
    double den,mom,s;
    double x_k;
    double x_p_k;
    double y_l;
    double y_q_l;

    den = static_cast<double>( (p+q+2)*(p+q+1)*vpMath::comb(p+q,p) );

    mom = 0.0;
    for (i=1;i<=points.size()-1;i++)
    {
      s = 0.0;
        x_k=1;
      for (k=0;k<=p;k++)
      {
        y_l=1;
        x_p_k = pow(points[i-1].get_x(), (int)(p-k));
        for (l=0;l<=q;l++)
        {
            y_q_l=pow(points[i-1].get_y(), (int)(q-l));

          s += static_cast<double>(
            vpMath::comb(k+l,l)
            *vpMath::comb(p+q-k-l,q-l)
            *x_k
            *x_p_k
            *y_l
            *y_q_l );

            y_l*=points[i].get_y();

        }
        x_k*=points[i].get_x();

      }

      s *= ((points[i-1].get_x())*(points[i].get_y())-(points[i].get_x())*(points[i-1].get_y()));
      mom += s;
    }
    mom /= den;
    return(mom);
}

/*!
  Caching to avoid redundant multiplications.

  \param cache : Lookup table that contains the order by order values. For example,
  if the order is 3, cache will contain:
  \code
  1   x     x^2
  y   x*y   x^2*y
  y^2 x*y^2 x^2*y^2
  \endcode

  \param x, y : Coordinates of a point.
*/
void vpMomentObject::cacheValues(std::vector<double>& cache,double x, double y){
    cache[0]=1;

    for(register unsigned int i=1;i<order;i++)
        cache[i]=cache[i-1]*x;

    for(register unsigned int j=order;j<order*order;j+=order)
        cache[j]=cache[j-order]*y;

    for(register unsigned int j=1;j<order;j++){
        for(register unsigned int i=1;i<order-j;i++){
            cache[j*order+i] = cache[j*order]*cache[i];
        }
    }
}

/*!
  Computes basic moments from a vector of points.
  There are two cases:
  -# Dense case specified by setType(vpMomentObject::DENSE_POLYGON):
     - The points are the vertices describing a closed contour polygon.
     - They must be stored in a clockwise order.
     - The first and the last points should be the same to close the contour.
  -# Discrete case specified by setType(vpMomentObject::DISCRETE)
     - The points will be interpreted as a discrete point cloud.

  \param points : Vector of points.

  The code below shows how to use this function to consider a dense object defined by a closed contour.

  \code
#include <visp/vpMomentObject.h>
#include <visp/vpPoint.h>

int main()
{
  // Define the contour of an object by a 5 clockwise vertices on a plane
  vpPoint p;
  std::vector<vpPoint> vec_p; // vector that contains the vertices of the contour polygon

  p.set_x(-0.2); p.set_y(0.1); // coordinates in meters in the image plane (vertex 1)
  vec_p.push_back(p);
  p.set_x(+0.3); p.set_y(0.1); // coordinates in meters in the image plane (vertex 2)
  vec_p.push_back(p);
  p.set_x(+0.2); p.set_y(-0.1); // coordinates in meters in the image plane (vertex 3)
  vec_p.push_back(p);
  p.set_x(-0.2); p.set_y(-0.15); // coordinates in meters in the image plane (vertex 4)
  vec_p.push_back(p);
  p.set_x(-0.2); p.set_y(0.1); // close the contour (vertex 5 = vertex 1)
  vec_p.push_back(p);

  vpMomentObject obj(4); // Create an image moment object with 4 as maximum order
  obj.setType(vpMomentObject::DENSE_POLYGON); // The object is defined by a countour polygon
  obj.fromVector(vec_p); // Init the dense object with the polygon

  return 0;
}
  \endcode

  This other example shows how to consider an object as a discrete set of four points.

  \code
#include <visp/vpMomentObject.h>
#include <visp/vpPoint.h>

int main()
{
  // Define 4 discrete points on a plane
  vpPoint p;
  std::vector<vpPoint> vec_p; // vector that contains the 4 points

  p.set_x(-0.2); p.set_y(0.1); // coordinates in meters in the image plane (point 1)
  vec_p.push_back(p);
  p.set_x(+0.3); p.set_y(0.1); // coordinates in meters in the image plane (point 2)
  vec_p.push_back(p);
  p.set_x(+0.2); p.set_y(-0.1); // coordinates in meters in the image plane (point 3)
  vec_p.push_back(p);
  p.set_x(-0.2); p.set_y(-0.15); // coordinates in meters in the image plane (point 4)
  vec_p.push_back(p);

  vpMomentObject obj(4); // Create an image moment object with 4 as maximum order
  obj.setType(vpMomentObject::DISCRETE); // The object is constituted by discrete points
  obj.fromVector(vec_p); // Init the dense object with the points

  return 0;
}
  \endcode
*/
void vpMomentObject::fromVector(std::vector<vpPoint>& points){
    if(type==vpMomentObject::DENSE_POLYGON){
        if(
                std::abs(points.rbegin()->get_x()-points.begin()->get_x())>std::numeric_limits<double>::epsilon() ||
                std::abs(points.rbegin()->get_y()-points.begin()->get_y())>std::numeric_limits<double>::epsilon()
                ){
            points.resize(points.size()+1);
            points[points.size()-1] = points[0];
        }
        for(register unsigned int j=0;j<order*order;j++){
            values[j]=calc_mom_polygon(j%order,j/order,points);
        }
    }else{
      std::vector<double> cache(order*order,0.);
        values.assign(order*order,0);
        for(register unsigned int i=0;i<points.size();i++){
            cacheValues(cache,points[i].get_x(),points[i].get_y());
            for(register unsigned int j=0;j<order;j++){
                for(register unsigned int k=0;k<order-j;k++){
                    values[j*order+k]+=cache[j*order+k];
                }
            }
        }
    }
}

/*!
  Computes basic moments from an image.
  There is no assumption made about whether the input is dense or discrete but it's more common to use vpMomentObject::DENSE_FULL_OBJECT with this method.

  \param image : Image to consider.
  \param threshold : Pixels with a luminance lower than this threshold will be considered.
  \param cam : Camera parameters used to convert pixels coordinates in meters in the image plane.

  The code below shows how to use this function.
  \code
#include <visp/vpMomentObject.h>
#include <visp/vpImage.h>

int main()
{
  vpCameraParameters cam;             // Camera parameters used for pixel to meter conversion
  vpImage<unsigned char> I(288, 384); // Image used to define the object
  // ... Initialize the image

  unsigned char threshold = 128; // Gray level used to define which part of the image belong to the dense object

  vpMomentObject obj(3); // Create an image moment object with 3 as maximum order
  obj.fromImage(I, threshold, cam); // Initialize the object from the image

  return 0;
}
  \endcode
*/

void vpMomentObject::fromImage(const vpImage<unsigned char>& image, unsigned char threshold, const vpCameraParameters& cam){
#ifdef VISP_HAVE_OPENMP    
  #pragma omp parallel shared(threshold)
  {        
    std::vector<double> curvals(order*order); 
    curvals.assign(order*order,0.);
    unsigned int i_, j_;

    #pragma omp for nowait//automatically organize loop counter between threads
    for(int i=0;i<(int)image.getCols();i++){
      for(int j=0;j<(int)image.getRows();j++){
        i_ = static_cast<unsigned int>(i);
        j_ = static_cast<unsigned int>(j);
        if(image[j_][i_]>threshold){
          double x=0;
          double y=0;
          vpPixelMeterConversion::convertPoint(cam,i_,j_,x,y);

          double xval=1.;
          double yval=1.;
          for(register unsigned int k=0;k<order;k++){            
            xval=1.;
            for(register unsigned int l=0;l<order-k;l++){
              curvals[(k*order+l)]+=(xval*yval);
              xval*=x;
            }
            yval*=y;
          }          
        }
      }
    }    
    
    #pragma omp master //only set this variable in master thread
    {      
      values.assign(order*order, 0.);      
    }

    #pragma omp barrier
    
    for(register unsigned int k=0;k<order;k++){    
      for(register unsigned int l=0;l<order-k;l++){
        #pragma omp atomic
        values[k*order+l]+= curvals[k*order+l];
      }
    }
    
  }    
  
#else
  std::vector<double> cache(order*order,0.);
    values.assign(order*order,0);
    for(register unsigned int i=0;i<image.getCols();i++){
        for(register unsigned int j=0;j<image.getRows();j++){
            if(image[j][i]>threshold){
                double x=0;
                double y=0;
                vpPixelMeterConversion::convertPoint(cam,i,j,x,y);
                cacheValues(cache,x,y);
                for(register unsigned int k=0;k<order;k++){
                    for(register unsigned int l=0;l<order-k;l++){
                        values[k*order+l]+=cache[k*order+l];
                    }
                }
            }
        }
    }
#endif
}



/*!
  Default constructor.
  Initializes the object with the maximum used order. You cannot use higher order moments than the order of the moment object.
  The parameter specified is the highest desired included order.
  All orders up to this values will be computed. In other words, a vpMomentObject will compute all \f$ m_{ij} \f$ moments with \f$ i+j \in [0..order] \f$.

  \param order : Maximum reached order (i+j) to be used. All
  considered i+j will be of order smaller or equal than this
  parameter. For example if this parameter is 5, all moment values of
  order 0 to 5 included will be computed.
*/
vpMomentObject::vpMomentObject(unsigned int order) : order(order+1),type(DENSE_FULL_OBJECT){
    values.resize((order+1)*(order+1)*12);
    values.assign((order+1)*(order+1)*12,0);
}

/*!
  Returns all basic moment values \f$m_{ij}\f$ with \f$i \in [0:\mbox{order}]\f$ and \f$j \in [0:\mbox{order}]\f$.

  \return Vector of moment values. To access \f$m_{ij}\f$, you have to read vpMomentObject::get()[j*(order+1)+i].

  For example, if the maximal order is 3, the following values are provided:
  \code
m00 m10 m20 m01 m11 m21 m02 m12 m12 m30 m03
  \endcode

  To access for example to the basic moment m12, you should use this kind of code:
  \code
vpMomentObject obj(3);
// ... initialise the object using fromVector() or fromImage()
std::vector mij = obj.get();
double m12;
m12 = mij[2*(obj.getOrder()+1)+1]; // i=1 and j=2
  \endcode
*/
std::vector<double>& vpMomentObject::get() {
    return values;

}

/*!
  Returns the basic moment value \f$m_{ij}\f$ corresponding to i,j indexes

  \param i : First moment index, with \f$i+j \leq order\f$.
  \param j : Second moment index, with \f$i+j \leq order\f$.
*/
double vpMomentObject::get(unsigned int i, unsigned int j) const {
    assert(i+j<=getOrder());
    if(i+j>=order) throw vpException(vpException::badValue,"The requested value has not been computed, you should specify a higher order.");

    return values[j*order+i];
}

/*!
  Outputs the basic moment's values \f$m_{ij}\f$ to a stream presented as a matrix.    
  The first line corresponds to \f$m_{0[0:order]}\f$, the second one to \f$m_{1[0:order]}\f$
  Values in table corresponding to a higher order are marked with an "x" and not computed.

  For example, if the maximal order is 3, the following values are provided:

  \code
  m00 m10 m20 m30
  m01 m11 m21 x
  m02 m12  x  x
  m03 x    x  x
  \endcode

*/
std::ostream & operator<<(std::ostream & os, const vpMomentObject& m){
    for(unsigned int i = 0;i<m.values.size();i++){

        if(i%(m.order)==0)
          os << std::endl;

        if((i%(m.order)+i/(m.order))<m.order)
            os << m.values[i] ;
        else
            os << "x";

        os  << "\t";

    }

    return os;
}
