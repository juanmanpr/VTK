/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMesaRenderer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

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
// .NAME vtkMesaRenderer - OpenGL renderer
// .SECTION Description
// vtkMesaRenderer is a concrete implementation of the abstract class
// vtkRenderer. vtkMesaRenderer interfaces to the mesa graphics library.
// This file is created, by a copy of vtkOpenGLRenderer

#ifndef __vtkMesaRenderer_h
#define __vtkMesaRenderer_h

#include <stdlib.h>
#include "vtkRenderer.h"

class VTK_EXPORT vtkMesaRenderer : public vtkRenderer
{
protected:
  int NumberOfLightsBound;

public:
  static vtkMesaRenderer *New();
  vtkTypeMacro(vtkMesaRenderer,vtkRenderer);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Concrete open gl render method.
  void DeviceRender(void); 

  // Description:
  // Internal method temporarily removes lights before reloading them
  // into graphics pipeline.
  void ClearLights(void);

  void Clear(void);

  // Description:
  // Ask lights to load themselves into graphics pipeline.
  int UpdateLights(void);
  
protected:
  vtkMesaRenderer();
  ~vtkMesaRenderer();
  vtkMesaRenderer(const vtkMesaRenderer&) {};
  void operator=(const vtkMesaRenderer&) {};

  //BTX
  // Picking functions to be implemented by sub-classes
  virtual void DevicePickRender();
  virtual void StartPick(unsigned int pickFromSize);
  virtual void SetPickId(unsigned int pickID);
  virtual void DonePick();
  virtual unsigned int GetPickedID();
  virtual float GetPickedZ();
  // Ivars used in picking
  class vtkGLPickInfo* PickInfo;
  //ETX
  float PickedZ;
};

#endif
