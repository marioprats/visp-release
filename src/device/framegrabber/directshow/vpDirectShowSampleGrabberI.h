/****************************************************************************
 *
 * $Id: vpDirectShowSampleGrabberI.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * DirectShow framegrabber callback.
 *
 * Authors:
 * Bruno Renier
 *
 *****************************************************************************/

#ifndef vpDirectShowSampleGrabberI_hh
#define vpDirectShowSampleGrabberI_hh

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <visp/vpConfig.h>
#if ( defined(VISP_HAVE_DIRECTSHOW) ) 

#include <qedit.h>
#include <stdio.h>
#include <dshow.h>

#include <visp/vpImage.h>
#include <visp/vpRGBa.h>

/*!
	This class is needed in order to implement a callback function
	associated with the grabber
*/
class VISP_EXPORT vpDirectShowSampleGrabberI : public ISampleGrabberCB
{
	vpDirectShowSampleGrabberI();
	virtual ~vpDirectShowSampleGrabberI();

	//needed by the interface
	STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }
 
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
 
	//not implemented
    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
    {
		return E_NOTIMPL;
    }
 
	//our callback function
    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);

private:
	//the currently connected media type
	AM_MEDIA_TYPE connectedMediaType;

	//true if the source media type is not a standard one
	bool specialMediaType;
	//true if the image needs to be flipped (only for special media types)
	bool invertedSource;

	//booleans used to signal a demand from acquire
	bool acqGrayDemand;
	bool acqRGBaDemand;

	//pointer on the image to fill during the next callback if there has been a demand
	vpImage<vpRGBa> * rgbaIm;
	vpImage<unsigned char> * grayIm;

	//semaphore used to synchronize the productor (callback) and the consumer (acquire)
	HANDLE copySem;

	friend class vpDirectShowGrabberImpl;
};

#endif
#endif
#endif
