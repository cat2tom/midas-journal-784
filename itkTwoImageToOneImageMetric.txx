/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTwoImageToOneImageMetric.txx,v $
  Language:  C++
  Date:      $Date: 2010/12/20 $
  Version:   $Revision: 1.0 $
  Author:    Jian Wu (eewujian@hotmail.com)
             Univerisity of Florida
		     Virginia Commonwealth University

  This program was modified from the ITK program--itkImageToImageMetric.txx

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkTwoImageToOneImageMetric_txx
#define _itkTwoImageToOneImageMetric_txx


#include "itkTwoImageToOneImageMetric.h"


namespace itk
{

/*
 * Constructor
 */
template <class TFixedImage, class TMovingImage> 
TwoImageToOneImageMetric<TFixedImage,TMovingImage>
::TwoImageToOneImageMetric()
{
  m_FixedImage1    = 0; // has to be provided by the user.
  m_FixedImage2    = 0; // has to be provided by the user.
  m_MovingImage   = 0; // has to be provided by the user.
  m_Transform     = 0; // has to be provided by the user.
  m_Interpolator1  = 0; // has to be provided by the user.
  m_Interpolator2  = 0; // has to be provided by the user.
  m_GradientImage = 0; // will receive the output of the filter;
  m_ComputeGradient = true; // metric computes gradient by default
  m_NumberOfPixelsCounted = 0; // initialize to zero
  m_GradientImage = NULL; // computed at initialization
}


/*
 * Set the parameters that define a unique transform
 */
template <class TFixedImage, class TMovingImage> 
void
TwoImageToOneImageMetric<TFixedImage,TMovingImage>
::SetTransformParameters( const ParametersType & parameters ) const
{
  if( !m_Transform )
    {
    itkExceptionMacro(<<"Transform has not been assigned");
    }
  m_Transform->SetParameters( parameters );
}



/*
 * Initialize
 */
template <class TFixedImage, class TMovingImage> 
void
TwoImageToOneImageMetric<TFixedImage,TMovingImage>
::Initialize(void) throw ( ExceptionObject )
{

  if( !m_Transform )
    {
    itkExceptionMacro(<<"Transform is not present");
    }

  if( !m_Interpolator1 )
    {
    itkExceptionMacro(<<"Interpolator1 is not present");
    }
  if( !m_Interpolator2 )
    {
    itkExceptionMacro(<<"Interpolator2 is not present");
    }

  if( !m_MovingImage )
    {
    itkExceptionMacro(<<"MovingImage is not present");
    }

  if( !m_FixedImage1 )
    {
    itkExceptionMacro(<<"FixedImage1 is not present");
    }

  if( !m_FixedImage2 )
    {
    itkExceptionMacro(<<"FixedImage2 is not present");
    }

  if( m_FixedImageRegion1.GetNumberOfPixels() == 0 )
    {
    itkExceptionMacro(<<"FixedImageRegion1 is empty");
    }

  if( m_FixedImageRegion2.GetNumberOfPixels() == 0 )
    {
    itkExceptionMacro(<<"FixedImageRegion2 is empty");
    }

  // If the image is provided by a source, update the source.
  if( m_MovingImage->GetSource() )
    {
    m_MovingImage->GetSource()->Update();
    }

  // If the image is provided by a source, update the source.
  if( m_FixedImage1->GetSource() )
    {
    m_FixedImage1->GetSource()->Update();
    }

  if( m_FixedImage2->GetSource() )
    {
    m_FixedImage2->GetSource()->Update();
    }

  // Make sure the FixedImageRegion is within the FixedImage buffered region
  if ( !m_FixedImageRegion1.Crop( m_FixedImage1->GetBufferedRegion() ) )
    {
    itkExceptionMacro(<<"FixedImageRegion1 does not overlap the fixed image buffered region" );
    }

  if ( !m_FixedImageRegion2.Crop( m_FixedImage2->GetBufferedRegion() ) )
    {
    itkExceptionMacro(<<"FixedImageRegion2 does not overlap the fixed image buffered region" );
    }

  m_Interpolator1->SetInputImage( m_MovingImage );
  m_Interpolator2->SetInputImage( m_MovingImage );
 
  if ( m_ComputeGradient )
    {

    GradientImageFilterPointer gradientFilter
      = GradientImageFilterType::New();

    gradientFilter->SetInput( m_MovingImage );

    const typename MovingImageType::SpacingType&
      spacing = m_MovingImage->GetSpacing();
    double maximumSpacing=0.0;
    for(unsigned int i=0; i<MovingImageDimension; i++)
      {
      if( spacing[i] > maximumSpacing )
        {
        maximumSpacing = spacing[i];
        }
      }
    gradientFilter->SetSigma( maximumSpacing );
    gradientFilter->SetNormalizeAcrossScale( true );

    gradientFilter->Update();

    m_GradientImage = gradientFilter->GetOutput();

    }

  // If there are any observers on the metric, call them to give the
  // user code a chance to set parameters on the metric
  this->InvokeEvent( InitializeEvent() );
}
 

/*
 * PrintSelf
 */
template <class TFixedImage, class TMovingImage> 
void
TwoImageToOneImageMetric<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "ComputeGradient: "
     << static_cast<typename NumericTraits<bool>::PrintType>(m_ComputeGradient)
     << std::endl;
  os << indent << "Moving Image: " << m_MovingImage.GetPointer()  << std::endl;
  os << indent << "Fixed  Image 1: " << m_FixedImage1.GetPointer()   << std::endl;
  os << indent << "Fixed  Image 2: " << m_FixedImage2.GetPointer()   << std::endl;
  os << indent << "Gradient Image: " << m_GradientImage.GetPointer()   << std::endl;
  os << indent << "Transform:    " << m_Transform.GetPointer()    << std::endl;
  os << indent << "Interpolator 1: " << m_Interpolator1.GetPointer() << std::endl;
  os << indent << "Interpolator 2: " << m_Interpolator2.GetPointer() << std::endl;
  os << indent << "FixedImageRegion 1: " << m_FixedImageRegion1 << std::endl;
  os << indent << "FixedImageRegion 2: " << m_FixedImageRegion2 << std::endl;
  os << indent << "Moving Image Mask: " << m_MovingImageMask.GetPointer() << std::endl;
  os << indent << "Fixed Image Mask 1: " << m_FixedImageMask1.GetPointer() << std::endl;
  os << indent << "Fixed Image Mask 2: " << m_FixedImageMask2.GetPointer() << std::endl;
  os << indent << "Number of Pixels Counted: " << m_NumberOfPixelsCounted << std::endl;

}


} // end namespace itk

#endif
