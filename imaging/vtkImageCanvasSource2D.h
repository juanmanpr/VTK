/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageCanvasSource2D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Thanks:    Thanks to C. Charles Law who developed this class.

Copyright (c) 1993-1995 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
// .NAME vtkImageCanvasSource2D - Paints on a canvas
// .SECTION Description
// vtkImageCanvasSource2D is a source that starts as a blank image.
// you may add to the image with two-dimensional drawing routines.
// It can paint multi-spectral images.


#ifndef __vtkImageCanvasSource2D_h
#define __vtkImageCanvasSource2D_h

#include <math.h>
#include "vtkStructuredPoints.h"

//
// Special classes for manipulating data
//
//BTX - begin tcl exclude
//
// For the fill functionality (use connector ??)
class vtkImageCanvasSource2DPixel { //;prevent man page generation
public:
  static vtkImageCanvasSource2DPixel *New() 
    { return new vtkImageCanvasSource2DPixel ;}
  int X;
  int Y;
  void *Pointer;
  vtkImageCanvasSource2DPixel *Next;
};
//ETX - end tcl exclude
//


class VTK_EXPORT vtkImageCanvasSource2D : public vtkStructuredPoints
{
public:
  // Description:
  // Construct an instance of vtkImageCanvasSource2D with no data.
  static vtkImageCanvasSource2D *New();

  vtkTypeMacro(vtkImageCanvasSource2D,vtkStructuredPoints);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // To drawing into a different image, set it with this method.
  void SetImageData(vtkImageData *image);
  vtkGetObjectMacro(ImageData, vtkImageData);
  
  // Description:
  // Set/Get DrawValue.  This is the value that is used when filling datas
  // or drawing lines.
  void SetDrawColor(int dim, float *color);
  void GetDrawColor(int dim, float *color);
  vtkSetVector4Macro(DrawColor, float);
  vtkGetVector4Macro(DrawColor, float);
  void SetDrawColor(float a) {this->SetDrawColor(a, 0.0, 0.0, 0.0);}
  void SetDrawColor(float a,float b) {this->SetDrawColor(a, b, 0.0, 0.0);}
  void SetDrawColor(float a, float b, float c) {this->SetDrawColor(a, b, c, 0.0);}
    
  void FillBox(int min0, int max0, int min1, int max1);
  void FillTube(int x0, int y0, int x1, int y1, float radius);
  void FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
  void DrawCircle(int c0, int c1, float radius);
  void DrawPoint(int p0, int p1);
  void DrawSegment(int x0, int y0, int x1, int y1);
  void DrawSegment3D(float *p0, float *p1);
  void DrawSegment3D(float x1, float y1, float z1, float x2, float y2, float z2) 
    { float p1[3], p2[3]; 
    p1[0] = x1; p1[1] = y1; p1[2] = z1; p2[0] = x2; p2[1] = y2; p2[2] = z2;
    this->DrawSegment3D(p1, p2);}

  // Description:
  // Fill a colored area with another color. (like connectivity)
  // All pixels connected to pixel (x, y) get replaced by draw color.
  void FillPixel(int x, int y);

  // Description:
  // To make Canvas source more like other sources, this get output
  // method should be used.
  vtkImageData *GetOutput() {return this;}
  
  // Description:
  // These methods also set the WholeExtent of this "DataObject".
  void SetExtent(int *extent);
  void SetExtent(int x1, int x2, int y1, int y2, int z1, int z2);
  
protected:
  vtkImageCanvasSource2D();
  // Destructor: Deleting a vtkImageCanvasSource2D automatically deletes the
  // associated vtkImageData.  However, since the data is reference counted,
  // it may not actually be deleted.
  ~vtkImageCanvasSource2D();
  vtkImageCanvasSource2D(const vtkImageCanvasSource2D&) {};
  void operator=(const vtkImageCanvasSource2D&) {};

  vtkImageData *ImageData;
  float DrawColor[4];
  
  int ClipSegment(int &a0, int &a1, int &b0, int &b1);
};



#endif


