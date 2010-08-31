/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageViewer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkImageViewer - Display a 2d image.
// .SECTION Description
// vtkImageViewer is a convenience class for displaying a 2d image.  It
// packages up the functionality found in vtkRenderWindow, vtkRenderer,
// vtkActor2D and vtkImageMapper into a single easy to use class.  Behind the
// scenes these four classes are actually used to to provide the required
// functionality. vtkImageViewer is simply a wrapper around them.

// .SECTION See Also
// vtkRenderWindow vtkRenderer vtkImageMapper vtkActor2D

#ifndef __vtkImageViewer_h
#define __vtkImageViewer_h

#include "vtkObject.h"

#include "vtkImageMapper.h" // For all the inline methods
#include "vtkRenderWindow.h" // For all the inline methods

class vtkInteractorStyleImage;

class VTK_RENDERING_EXPORT vtkImageViewer : public vtkObject 
{
public:
  static vtkImageViewer *New();
  
  vtkTypeMacro(vtkImageViewer,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get name of rendering window
  char *GetWindowName() {return this->RenderWindow->GetWindowName();};

  // Description:
  // Render the resulting image.
  virtual void Render(void);
  
  // Description:
  // Set/Get the input to the viewer.
  void SetInput(vtkImageData *in) {this->ImageMapper->SetInput(in);};
  vtkImageData *GetInput() { return this->ImageMapper->GetInput();};
  virtual void SetInputConnection(vtkAlgorithmOutput* input) {
    this->ImageMapper->SetInputConnection(input);};

  // Description:
  // What is the possible Min/ Max z slices available.
  int GetWholeZMin() {return this->ImageMapper->GetWholeZMin();};
  int GetWholeZMax() {return this->ImageMapper->GetWholeZMax();};
  
  // Description:
  // Set/Get the current Z Slice to display
  int GetZSlice() {return this->ImageMapper->GetZSlice();};
  void SetZSlice(int s) {this->ImageMapper->SetZSlice(s);};
  
  // Description:
  // Sets window/level for mapping pixels to colors.
  double GetColorWindow() {return this->ImageMapper->GetColorWindow();};
  double GetColorLevel() {return this->ImageMapper->GetColorLevel();};
  void SetColorWindow(double s) {this->ImageMapper->SetColorWindow(s);};
  void SetColorLevel(double s) {this->ImageMapper->SetColorLevel(s);};

  // Description:
  // These are here for using a tk window.
  void SetDisplayId(void *a) {this->RenderWindow->SetDisplayId(a);};
  void SetWindowId(void *a) {this->RenderWindow->SetWindowId(a);};
  void SetParentId(void *a) {this->RenderWindow->SetParentId(a);};
  
  // Description:
  // By default this is a color viewer.  GrayScaleHintOn will improve the
  // appearance of gray scale images on some systems.
  VTK_LEGACY(int GetGrayScaleHint());
  VTK_LEGACY(void SetGrayScaleHint(int vtkNotUsed(a)));
  VTK_LEGACY(void GrayScaleHintOn());
  VTK_LEGACY(void GrayScaleHintOff());

  // Description:
  // Set/Get the position in screen coordinates of the rendering window.
  int *GetPosition() {return this->RenderWindow->GetPosition();};
  void SetPosition(int a,int b) {this->RenderWindow->SetPosition(a,b);};
  virtual void SetPosition(int a[2]);

  // Description:
  // Set/Get the size of the window in screen coordinates in pixels.
  int *GetSize() {return this->RenderWindow->GetSize();};
  void SetSize(int a,int b) {this->RenderWindow->SetSize(a,b);};
  virtual void SetSize(int a[2]);
  
  // Description:
  // Get the internal objects
  vtkGetObjectMacro(RenderWindow,vtkRenderWindow);
  vtkGetObjectMacro(Renderer, vtkRenderer);
  vtkGetObjectMacro(ImageMapper,vtkImageMapper);
  vtkGetObjectMacro(Actor2D,vtkActor2D);
  
  // Description:
  // Create and attach an interactor for this window
  void SetupInteractor(vtkRenderWindowInteractor *);
  
  // Description:  
  // Create a window in memory instead of on the screen. This may not
  // be supported for every type of window and on some windows you may
  // need to invoke this prior to the first render.
  void SetOffScreenRendering(int);
  int GetOffScreenRendering();
  void OffScreenRenderingOn();
  void OffScreenRenderingOff();

protected:
  vtkImageViewer();
  ~vtkImageViewer();

  vtkRenderWindow *RenderWindow;
  vtkRenderer *Renderer;
  vtkImageMapper *ImageMapper;
  vtkActor2D     *Actor2D;
  int FirstRender;
  vtkRenderWindowInteractor *Interactor;
  vtkInteractorStyleImage *InteractorStyle;

private:
  vtkImageViewer(const vtkImageViewer&);  // Not implemented.
  void operator=(const vtkImageViewer&);  // Not implemented.
};

#endif

