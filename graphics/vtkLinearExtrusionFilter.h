/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLinearExtrusionFilter.h
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
// .NAME vtkLinearExtrusionFilter - sweep polygonal data creating a "skirt" from free edges and lines, and lines from vertices
// .SECTION Description
// vtkLinearExtrusionFilter is a modelling filter. It takes polygonal data as 
// input and generates polygonal data on output. The input dataset is swept
// according to some extrusion function and creates new polygonal primitives.
// These primitives form a "skirt" or swept surface. For example, sweeping a
// line results in a quadrilateral, and sweeping a triangle creates a "wedge".
//
// There are a number of control parameters for this filter. You can 
// control whether the sweep of a 2D object (i.e., polygon or triangle strip) 
// is capped with the generating geometry via the "Capping" ivar. Also, you
// can extrude in the direction of a user specified vector, towards a point,
// or in the direction of vertex normals (normals must be provided - use 
// vtkPolyDataNormals if necessary). The amount of extrusion is controlled by
// the "ScaleFactor" instance variable.
//
// The skirt is generated by locating certain topological features. Free 
// edges (edges of polygons or triangle strips only used by one polygon or
// triangle strips) generate surfaces. This is true also of lines or 
// polylines. Vertices generate lines.
//
// This filter can be used to create 3D fonts, 3D irregular bar charts,
// or to model 2 1/2D objects like punched plates. It also can be used to 
// create solid objects from 2D polygonal meshes.

// .SECTION Caveats
// Some polygonal objects have no free edges (e.g., sphere). When swept,
// this will result in two separate surfaces if capping is on, or no surface
// if capping is off.

// .SECTION See Also
// vtkRotationalExtrusionFilter

#ifndef __vtkLinearExtrusionFilter_h
#define __vtkLinearExtrusionFilter_h

#include "vtkPolyDataToPolyDataFilter.h"

#define VTK_VECTOR_EXTRUSION 1
#define VTK_NORMAL_EXTRUSION 2
#define VTK_POINT_EXTRUSION 3

class VTK_EXPORT vtkLinearExtrusionFilter : public vtkPolyDataToPolyDataFilter 
{
public:
  vtkTypeMacro(vtkLinearExtrusionFilter,vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create object with normal extrusion type, capping on, scale factor=1.0,
  // vector (0,0,1), and extrusion point (0,0,0).
  static vtkLinearExtrusionFilter *New();

  // Description:
  // Set/Get the type of extrusion.
  vtkSetClampMacro(ExtrusionType,int,VTK_VECTOR_EXTRUSION,VTK_POINT_EXTRUSION);
  vtkGetMacro(ExtrusionType,int);
  void SetExtrusionTypeToVectorExtrusion()
    {this->SetExtrusionType(VTK_VECTOR_EXTRUSION);};
  void SetExtrusionTypeToNormalExtrusion()
    {this->SetExtrusionType(VTK_NORMAL_EXTRUSION);};
  void SetExtrusionTypeToPointExtrusion()
    {this->SetExtrusionType(VTK_POINT_EXTRUSION);};

  // Description:
  // Turn on/off the capping of the skirt.
  vtkSetMacro(Capping,int);
  vtkGetMacro(Capping,int);
  vtkBooleanMacro(Capping,int);

  // Description:
  // Set/Get extrusion scale factor,
  vtkSetMacro(ScaleFactor,float);
  vtkGetMacro(ScaleFactor,float);

  // Description:
  // Set/Get extrusion vector. Only needs to be set if VectorExtrusion is
  // turned on.
  vtkSetVector3Macro(Vector,float);
  vtkGetVectorMacro(Vector,float,3);

  // Description:
  // Set/Get extrusion point. Only needs to be set if PointExtrusion is
  // turned on. This is the point towards which extrusion occurs.
  vtkSetVector3Macro(ExtrusionPoint,float);
  vtkGetVectorMacro(ExtrusionPoint,float,3);

protected:
  vtkLinearExtrusionFilter();
  ~vtkLinearExtrusionFilter() {};
  vtkLinearExtrusionFilter(const vtkLinearExtrusionFilter&) {};
  void operator=(const vtkLinearExtrusionFilter&) {};

  void Execute();
  int ExtrusionType;
  int Capping;
  float ScaleFactor;
  float Vector[3];
  float ExtrusionPoint[3];

  //BTX
  float *(vtkLinearExtrusionFilter::*ExtrudePoint)(float x[3], int id, vtkNormals *normals);
  float *ViaNormal(float x[3], int id, vtkNormals *normals=NULL);
  float *ViaVector(float x[3], int id, vtkNormals *normals=NULL);
  float *ViaPoint(float x[3], int id, vtkNormals *normals=NULL);
  //ETX
 
};

#endif
