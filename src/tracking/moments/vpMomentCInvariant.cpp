/****************************************************************************
 *
 * $Id: vpMomentCInvariant.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 * Descriptor for various invariants used to drive space roations around X and Y axis.
 *
 * Authors:
 * Filip Novotny
 *
 *****************************************************************************/

#include <visp/vpMomentCInvariant.h>
#include <visp/vpMomentCentered.h>
#include <visp/vpMomentObject.h>

/*!
  Default constructor.
*/
vpMomentCInvariant::vpMomentCInvariant() : vpMoment(),I(16),II(4),c(4),s(4){
    values.resize(14);
}

/*!
  Computes some temporary invariants which are used internally to compute the final invariants.
  \param momentCentered : centered moments
  \param I : invariant output values
*/
void vpMomentCInvariant::computeI(vpMomentCentered& momentCentered, std::vector<double>& I){
    double mu30 = momentCentered.get(3,0);
    double mu30_2 = mu30*mu30;

    double mu03 = momentCentered.get(0,3);
    double mu03_2 = mu03*mu03;
    double mu03_3 = mu03*mu03_2;

    double mu20 = momentCentered.get(2,0);
    double mu02 = momentCentered.get(0,2);
    double mu50 = momentCentered.get(5,0);
    double mu32 = momentCentered.get(3,2);
    double mu14 = momentCentered.get(1,4);
    double mu05 = momentCentered.get(0,5);
    double mu23 = momentCentered.get(2,3);
    double mu41 = momentCentered.get(4,1);
    double mu40 = momentCentered.get(4,0);
    double mu04 = momentCentered.get(0,4);
    double mu31 = momentCentered.get(3,1);
    double mu13 = momentCentered.get(1,3);
    double mu22 = momentCentered.get(2,2);
    double mu21 = momentCentered.get(2,1);
    double mu12 = momentCentered.get(1,2);
    double mu11 = momentCentered.get(1,1);


    double mu11_2 = mu11*mu11;
    double mu12_2 = mu12*mu12;
    double mu21_2 = mu21*mu21;
    double mu22_2 = mu22*mu22;
    double mu13_2 = mu13*mu13;
    double mu31_2 = mu31*mu31;
    double mu04_2 = mu04*mu04;
    double mu40_2 = mu40*mu40;
    double mu21_3 = mu21*mu21_2;
    double mu12_3 = mu12_2*mu12;
    double mu12_4 = mu12_3*mu12;
    double mu21_4 = mu21_2*mu21_2;


    double kappa = mu30_2+mu03_2-3*mu21_2+6*mu21*mu03;
    double zeta = mu20-mu02;
    double zeta_2 = zeta * zeta;
    double omicron = (mu03_2+3*mu03*mu21+mu30*(mu30+3*mu12));
    double omega = mu50+2*mu32+mu14;
    double nu = mu05+2*mu23+mu41;
    double ro = mu50-2*mu32-3*mu14;
    double gamma = mu05-2*mu23-3*mu41;

    double delta = mu50-10*mu32+5*mu14;
    double phi = mu05-10*mu23+5*mu41;
    double omega_2 = omega*omega;
    double nu_2 = nu*nu;
    double ro_2 = ro*ro;
    double gamma_2 = gamma*gamma;
    double delta_2 = delta*delta;
    double phi_2 = phi*phi;

    I[1]=-mu20*mu02+mu11_2;
    I[2]=zeta_2+4*mu11_2;
    I[3]=(mu30-3*mu12)*(mu30-3*mu12)+(mu03-3*mu21)*(mu03-3*mu21);
    I[4]=(mu30+mu12)*(mu30+mu12)+(mu21+mu03)*(mu21+mu03);
    I[5]=-mu30_2*mu03_2+(-4*mu12_3+6*mu21*mu12*mu03)*mu30-4*mu21_3*mu03+3*mu21_2*mu12_2;
    I[6]=3*mu12_4+2*mu30*mu12_3+(3*mu30_2-6*mu03*mu21)*mu12_2-6*mu30*mu21*(mu21+mu03)*mu12+2*mu30_2*mu03_2+2*mu21_3*mu03+3*mu21_2*mu03_2+3*mu21_4;

    I[7]=(3*mu21+2*mu03)*mu12_3+3*mu30*(mu03+2*mu21)*mu12_2-3*mu21*(mu30+mu03+mu21)*(-mu30+mu03+mu21)*mu12+mu30*(-mu30_2*mu03-2*mu21_3-3*mu03*mu21_2+mu03_3);
    I[8]=3*mu21_4-3*mu21_3*mu03+(3*mu03_2+kappa-6*mu12_2)*mu21_2-mu03*(-15*mu12_2+kappa)*mu21-(-3*mu12_2*mu30+(2*kappa-3*mu03_2)*mu12+kappa*mu30)*mu12;
    I[9]=omicron*omicron;

    I[10]=mu40*mu04-4*mu31*mu13+3*mu22_2;
    I[11]=3*mu13_2+2*mu31*mu13+(-3*mu40-3*mu04)*mu22-2*mu40*mu04+3*mu31_2;
    I[12]=3*mu04_2+(2*mu40+12*mu22)*mu04+3*mu40_2+12*mu40*mu22+16*mu31*mu13;
    I[13]=omega_2+nu_2;
    I[14]=ro_2+gamma_2;
    I[15]=delta_2+phi_2;

    double a;
    if(getObject().getType()==vpMomentObject::DISCRETE)
        a = momentCentered.get(2,0)+momentCentered.get(0,2);
    else
        a = getObject().get(0,0);

    c[1]=momentCentered.get(2,0)-momentCentered.get(0,2);
    s[1]=2*momentCentered.get(1,1);
    c[2]=momentCentered.get(0,3)-3*momentCentered.get(2,1);
    s[2]=momentCentered.get(3,0)-3*momentCentered.get(1,2);
    c[3]=c[1]*c[1]-s[1]*s[1];
    s[3]=2*s[1]*c[1];

    II[1]=c[1]*c[1]+s[1]*s[1];
    II[2]=c[2]*c[2]+s[2]*s[2];
    II[3]=momentCentered.get(2,0)+momentCentered.get(0,2);

    K=(II[1]*(II[3]*sqrt(II[3])))/sqrt(a);



}

/*!
  Computes translation-plane-rotation-scale invariants.
  Depends on vpMomentCentered.
  All possible invariants are computed here. The selection of the invariant is done afterwards.
*/
void vpMomentCInvariant::compute(){
    if(getObject().getOrder()<5) throw vpException(vpException::notInitialized,"Order is not high enough for vpMomentCInvariant. Specify at least order 5.");
    bool found_moment_centered;
    vpMomentCentered& momentCentered = (static_cast<vpMomentCentered&>(getMoments().get("vpMomentCentered",found_moment_centered)));

    if(!found_moment_centered) throw vpException(vpException::notInitialized,"vpMomentCentered not found");

    computeI(momentCentered,I);
    double II3_2 = II[3]*II[3];
    double II3_3 = II3_2*II[3];

    double a;
    if(getObject().getType()==vpMomentObject::DISCRETE)
        a = momentCentered.get(2,0)+momentCentered.get(0,2);
    else
        a = getObject().get(0,0);

    values[0] = I[1]/I[2];
    values[1] = I[3]/I[4];

    values[2] = I[5]/I[6];

    values[3] = I[7]/I[6];

    values[4] = I[8]/I[6];

    values[5] = I[9]/I[6];

    values[6] = I[11]/I[10];

    values[7] = I[12]/I[10];

    values[8] = I[13]/I[15];

    values[9] = I[14]/I[15];

    values[10] = (c[2]*c[3]+s[2]*s[3])/K;
    values[11] = (s[2]*c[3]-c[2]*s[3])/K;

    values[12] = II[1]/(II3_2);
    values[13] = a*II[2]/(II3_3);
}

/*!
  Prints the temporary invariants.  Used for debug purposes only
  \param index : index of the temporary invariant
*/
void vpMomentCInvariant::printI(unsigned int index){
  std::cout << "I("<< index << ")=" << I[index] << std::endl;
}

/*!
  Outputs the moment's values to a stream.
*/
std::ostream & operator<<(std::ostream & os, const vpMomentCInvariant& c){
    for(unsigned int i = 0;i<c.values.size();i++){
      os << c.values[i] << "," << std::endl;
    }

    return os;
}




