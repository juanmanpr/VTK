/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkQuadraticHexahedron.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkQuadraticHexahedron.h"
#include "vtkPolyData.h"
#include "vtkPointLocator.h"
#include "vtkMath.h"
#include "vtkQuadraticEdge.h"
#include "vtkQuadraticQuad.h"
#include "vtkHexahedron.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkQuadraticHexahedron, "1.13");
vtkStandardNewMacro(vtkQuadraticHexahedron);

// Construct the hex with 20 points + 7 extra points for internal
// computation.
vtkQuadraticHexahedron::vtkQuadraticHexahedron()
{
  int i;

  // At times the cell looks like it has 27 points (during interpolation)
  // We initially allocate for 27.
  this->Points->SetNumberOfPoints(27);
  this->PointIds->SetNumberOfIds(27);
  for (i = 0; i < 27; i++)
    {
    this->Points->SetPoint(i, 0.0, 0.0, 0.0);
    this->PointIds->SetId(i,0);
    }
  this->Points->SetNumberOfPoints(20);
  this->PointIds->SetNumberOfIds(20);

  this->Edge = vtkQuadraticEdge::New();
  this->Face = vtkQuadraticQuad::New();
  this->Hex = vtkHexahedron::New();

  this->PointData = vtkPointData::New();
  this->CellData = vtkCellData::New();
  this->Scalars = vtkFloatArray::New();
  this->Scalars->SetNumberOfTuples(8);
}

vtkQuadraticHexahedron::~vtkQuadraticHexahedron()
{
  this->Edge->Delete();
  this->Face->Delete();
  this->Hex->Delete();

  this->PointData->Delete();
  this->CellData->Delete();
  this->Scalars->Delete();
}


static int LinearHexs[8][8] = { {0,8,24,11,16,22,26,20},
                                {8,1,9,24,22,17,21,26},
                                {11,24,10,3,20,26,23,19},
                                {24,9,2,10,26,21,18,23},
                                {16,22,26,20,4,12,25,15},
                                {22,17,21,26,12,5,13,25},
                                {20,26,23,19,15,25,14,7},
                                {26,21,18,23,25,13,6,14} };

static int HexFaces[6][8] = { {0,4,7,3,16,15,19,11},
                              {1,2,6,5,9,18,13,17},
                              {0,1,5,4,8,17,12,16},
                              {3,7,6,2,19,14,18,10},
                              {0,3,2,1,11,10,9,8},
                              {4,5,6,7,12,13,14,15} };

static int HexEdges[12][3] = { {0,1,8}, {1,2,9}, {3,2,10}, {0,3,11},
                               {4,5,12}, {5,6,13}, {7,6,14}, {4,7,15},
                               {0,4,16}, {1,5,17}, {3,7,19}, {2,6,18} };

static float MidPoints[7][3] = { {0.0,0.5,0.5}, {1.0,0.5,0.5}, 
                                 {0.5,0.0,0.5}, {0.5,1.0,0.5},
                                 {0.5,0.5,0.0}, {0.5,0.5,1.0},
                                 {0.5,0.5,0.5} };


vtkCell *vtkQuadraticHexahedron::GetEdge(int edgeId)
{
  edgeId = (edgeId < 0 ? 0 : (edgeId > 11 ? 11 : edgeId ));

  for (int i=0; i<3; i++)
    {
    this->Edge->PointIds->SetId(i,this->PointIds->GetId(HexEdges[edgeId][i]));
    this->Edge->Points->SetPoint(i,this->Points->GetPoint(HexEdges[edgeId][i]));
    }

  return this->Edge;
}

vtkCell *vtkQuadraticHexahedron::GetFace(int faceId)
{
  faceId = (faceId < 0 ? 0 : (faceId > 11 ? 11 : faceId ));

  for (int i=0; i<8; i++)
    {
    this->Face->PointIds->SetId(i,this->PointIds->GetId(HexFaces[faceId][i]));
    this->Face->Points->SetPoint(i,this->Points->GetPoint(HexFaces[faceId][i]));
    }

  return this->Face;
}

void vtkQuadraticHexahedron::Subdivide(vtkPointData *inPd, vtkCellData *inCd, 
                                       vtkIdType cellId)
{
  int numMidPts, i, j;
  float weights[20];
  float x[3];

  //Copy point and cell attribute data
  this->PointData->CopyAllocate(inPd,27);
  this->CellData->CopyAllocate(inCd,8);
  for (i=0; i<20; i++)
    {
    this->PointData->CopyData(inPd,this->PointIds->GetId(i),i);
    }
  this->CellData->CopyData(inCd,cellId,0);
  
  //Interpolate new values
  this->PointIds->SetNumberOfIds(20);
  float *p = ((vtkFloatArray *)this->Points->GetData())->GetPointer(0);
  for ( numMidPts=0; numMidPts < 7; numMidPts++ )
    {
    this->InterpolationFunctions(MidPoints[numMidPts], weights);

    for (j=0; j<3; j++) 
      {
      x[j] = 0.0;
      for (i=0; i<20; i++) 
        {
        x[j] += p[3*i+j] * weights[i];
        }
      }
    this->Points->SetPoint(20+numMidPts,x);
    this->PointData->InterpolatePoint(inPd, 20+numMidPts, 
                                      this->PointIds, weights);
    }
  this->PointIds->SetNumberOfIds(27);
}

static const float VTK_DIVERGED = 1.e6;
static const int VTK_HEX_MAX_ITERATION=10;
static const float VTK_HEX_CONVERGED=1.e-03;

int vtkQuadraticHexahedron::EvaluatePosition(float* x, 
                                             float* closestPoint, 
                                             int& subId, float pcoords[3],
                                             float& dist2, float *weights)
{
  int iteration, converged;
  float  params[3];
  float  fcol[3], rcol[3], scol[3], tcol[3];
  int i, j;
  float  d, *pt;
  float derivs[60];

  //  set initial position for Newton's method
  subId = 0;
  pcoords[0] = pcoords[1] = pcoords[2] = params[0] = params[1] = params[2]=0.5;

  //  enter iteration loop
  for (iteration=converged=0;
       !converged && (iteration < VTK_HEX_MAX_ITERATION);  iteration++) 
    {
    //  calculate element interpolation functions and derivatives
    this->InterpolationFunctions(pcoords, weights);
    this->InterpolationDerivs(pcoords, derivs);

    //  calculate newton functions
    for (i=0; i<3; i++) 
      {
      fcol[i] = rcol[i] = scol[i] = tcol[i] = 0.0;
      }
    for (i=0; i<20; i++)
      {
      pt = this->Points->GetPoint(i);
      for (j=0; j<3; j++)
        {
        fcol[j] += pt[j] * weights[i];
        rcol[j] += pt[j] * derivs[i];
        scol[j] += pt[j] * derivs[i+20];
        tcol[j] += pt[j] * derivs[i+40];
        }
      }

    for (i=0; i<3; i++)
      {
      fcol[i] -= x[i];
      }

    //  compute determinants and generate improvements
    d=vtkMath::Determinant3x3(rcol,scol,tcol);
    if ( fabs(d) < 1.e-20) 
      {
      return -1;
      }

    pcoords[0] = params[0] - vtkMath::Determinant3x3 (fcol,scol,tcol) / d;
    pcoords[1] = params[1] - vtkMath::Determinant3x3 (rcol,fcol,tcol) / d;
    pcoords[2] = params[2] - vtkMath::Determinant3x3 (rcol,scol,fcol) / d;

    //  check for convergence
    if ( ((fabs(pcoords[0]-params[0])) < VTK_HEX_CONVERGED) &&
         ((fabs(pcoords[1]-params[1])) < VTK_HEX_CONVERGED) &&
         ((fabs(pcoords[2]-params[2])) < VTK_HEX_CONVERGED) )
      {
      converged = 1;
      }

    // Test for bad divergence (S.Hirschberg 11.12.2001)
    else if ((fabs(pcoords[0]) > VTK_DIVERGED) || 
             (fabs(pcoords[1]) > VTK_DIVERGED) || 
             (fabs(pcoords[2]) > VTK_DIVERGED))
      {
      return -1;
      }

    //  if not converged, repeat
    else 
      {
      params[0] = pcoords[0];
      params[1] = pcoords[1];
      params[2] = pcoords[2];
      }
    }

  //  if not converged, set the parametric coordinates to arbitrary values
  //  outside of element
  if ( !converged )
    {
    return -1;
    }

  this->InterpolationFunctions(pcoords, weights);

  if ( pcoords[0] >= -0.001 && pcoords[0] <= 1.001 &&
  pcoords[1] >= -0.001 && pcoords[1] <= 1.001 &&
  pcoords[2] >= -0.001 && pcoords[2] <= 1.001 )
    {
    if (closestPoint)
      {
      closestPoint[0] = x[0]; closestPoint[1] = x[1]; closestPoint[2] = x[2];
      dist2 = 0.0; //inside hexahedron
      }
    return 1;
    }
  else
    {
    float pc[3], w[20];
    if (closestPoint)
      {
      for (i=0; i<3; i++) //only approximate, not really true for warped hexa
        {
        if (pcoords[i] < 0.0)
          {
          pc[i] = 0.0;
          }
        else if (pcoords[i] > 1.0)
          {
          pc[i] = 1.0;
          }
        else
          {
          pc[i] = pcoords[i];
          }
        }
      this->EvaluateLocation(subId, pc, closestPoint, (float *)w);
      dist2 = vtkMath::Distance2BetweenPoints(closestPoint,x);
      }
    return 0;
    }
}

void vtkQuadraticHexahedron::EvaluateLocation(int& vtkNotUsed(subId), 
                                              float pcoords[3], 
                                              float x[3], float *weights)
{
  int i, j;
  float *pt;

  this->InterpolationFunctions(pcoords, weights);

  x[0] = x[1] = x[2] = 0.0;
  for (i=0; i<20; i++)
    {
    pt = this->Points->GetPoint(i);
    for (j=0; j<3; j++)
      {
      x[j] += pt[j] * weights[i];
      }
    }
}

int vtkQuadraticHexahedron::CellBoundary(int subId, float pcoords[3], 
                                         vtkIdList *pts)
{
  return this->Hex->CellBoundary(subId, pcoords, pts);
}

void vtkQuadraticHexahedron::Contour(float value, 
                                     vtkDataArray* vtkNotUsed(cellScalars), 
                                     vtkPointLocator* locator, 
                                     vtkCellArray *verts, 
                                     vtkCellArray* lines, 
                                     vtkCellArray* polys, 
                                     vtkPointData* inPd, 
                                     vtkPointData* outPd,
                                     vtkCellData* inCd, 
                                     vtkIdType cellId, 
                                     vtkCellData* outCd)
{
  //subdivide into 8 linear hexes
  this->Subdivide(inPd,inCd,cellId);
  
  //contour each linear quad separately
  vtkDataArray *localScalars = this->PointData->GetScalars();
  for (int i=0; i<8; i++)
    {
    for (int j=0; j<8; j++)
      {
      this->Hex->Points->SetPoint(j,this->Points->GetPoint(LinearHexs[i][j]));
      this->Hex->PointIds->SetId(j,this->PointIds->GetId(LinearHexs[i][j]));
      this->Scalars->SetValue(j,localScalars->GetTuple1(LinearHexs[i][j]));
      }
    this->Hex->Contour(value,this->Scalars,locator,verts,lines,polys,
                       this->PointData,outPd,this->CellData,0,outCd);
    }
}

// Line-hex intersection. Intersection has to occur within [0,1] parametric
// coordinates and with specified tolerance.
int vtkQuadraticHexahedron::IntersectWithLine(float* p1, float* p2, 
                                              float tol, float& t,
                                              float* x, float* pcoords, 
                                              int& subId)
{
  int intersection=0;
  float tTemp;
  float pc[3], xTemp[3];
  int faceNum;

  t = VTK_LARGE_FLOAT;
  for (faceNum=0; faceNum<6; faceNum++)
    {
    for (int i=0; i<8; i++)
      {
      this->Face->Points->SetPoint(i,
            this->Points->GetPoint(HexFaces[faceNum][i]));
      }

    if ( this->Face->IntersectWithLine(p1, p2, tol, tTemp, 
                                       xTemp, pc, subId) )
      {
      intersection = 1;
      if ( tTemp < t )
        {
        t = tTemp;
        x[0] = xTemp[0]; x[1] = xTemp[1]; x[2] = xTemp[2]; 
        switch (faceNum)
          {
          case 0:
            pcoords[0] = 0.0; pcoords[1] = pc[1]; pcoords[2] = pc[0];
            break;

          case 1:
            pcoords[0] = 1.0; pcoords[1] = pc[0]; pcoords[2] = pc[1];
            break;

          case 2:
            pcoords[0] = pc[0]; pcoords[1] = 0.0; pcoords[2] = pc[1];
            break;

          case 3:
            pcoords[0] = pc[1]; pcoords[1] = 1.0; pcoords[2] = pc[0];
            break;

          case 4:
            pcoords[0] = pc[1]; pcoords[1] = pc[0]; pcoords[2] = 0.0;
            break;

          case 5:
            pcoords[0] = pc[0]; pcoords[1] = pc[1]; pcoords[2] = 1.0;
            break;
          }
        }
      }
    }
  return intersection;
}

int vtkQuadraticHexahedron::Triangulate(int vtkNotUsed(index), 
                                        vtkIdList *ptIds, vtkPoints *pts)
{
  pts->Reset();
  ptIds->Reset();

  ptIds->InsertId(0,this->PointIds->GetId(0));
  pts->InsertPoint(0,this->Points->GetPoint(0));

  ptIds->InsertId(1,this->PointIds->GetId(1));
  pts->InsertPoint(1,this->Points->GetPoint(1));

  return 1;
}

// Given parametric coordinates compute inverse Jacobian transformation
// matrix. Returns 9 elements of 3x3 inverse Jacobian plus interpolation
// function derivatives.
void vtkQuadraticHexahedron::JacobianInverse(float pcoords[3], 
                                             double **inverse, 
                                             float derivs[60])
{
  int i, j;
  double *m[3], m0[3], m1[3], m2[3];
  float *x;

  // compute interpolation function derivatives
  this->InterpolationDerivs(pcoords, derivs);

  // create Jacobian matrix
  m[0] = m0; m[1] = m1; m[2] = m2;
  for (i=0; i < 3; i++) //initialize matrix
    {
    m0[i] = m1[i] = m2[i] = 0.0;
    }

  for ( j=0; j < 20; j++ )
    {
    x = this->Points->GetPoint(j);
    for ( i=0; i < 3; i++ )
      {
      m0[i] += x[i] * derivs[j];
      m1[i] += x[i] * derivs[20 + j];
      m2[i] += x[i] * derivs[40 + j];
      }
    }

  // now find the inverse
  if ( vtkMath::InvertMatrix(m,inverse,3) == 0 )
    {
    vtkErrorMacro(<<"Jacobian inverse not found");
    return;
    }
}

void vtkQuadraticHexahedron::Derivatives(int vtkNotUsed(subId), 
                                         float pcoords[3], float *values, 
                                         int dim, float *derivs)
{
  double *jI[3], j0[3], j1[3], j2[3];
  float functionDerivs[60], sum[3];
  int i, j, k;

  // compute inverse Jacobian and interpolation function derivatives
  jI[0] = j0; jI[1] = j1; jI[2] = j2;
  this->JacobianInverse(pcoords, jI, functionDerivs);

  // now compute derivates of values provided
  for (k=0; k < dim; k++) //loop over values per vertex
    {
    sum[0] = sum[1] = sum[2] = 0.0;
    for ( i=0; i < 20; i++) //loop over interp. function derivatives
      {
      sum[0] += functionDerivs[i] * values[dim*i + k]; 
      sum[1] += functionDerivs[20 + i] * values[dim*i + k];
      sum[2] += functionDerivs[40 + i] * values[dim*i + k];
      }
    for (j=0; j < 3; j++) //loop over derivative directions
      {
      derivs[3*k + j] = sum[0]*jI[j][0] + sum[1]*jI[j][1] + sum[2]*jI[j][2];
      }
    }
}


// Clip this quadratic hex using scalar value provided. Like contouring, 
// except that it cuts the hex to produce tetrahedra.
void vtkQuadraticHexahedron::Clip(float value, 
                                  vtkDataArray* vtkNotUsed(cellScalars), 
                                  vtkPointLocator* locator, vtkCellArray* tets,
                                  vtkPointData* inPd, vtkPointData* outPd,
                                  vtkCellData* inCd, vtkIdType cellId, 
                                  vtkCellData* outCd, int insideOut)
{
  //create eight linear hexes
  this->Subdivide(inPd,inCd,cellId);
  
  //contour each linear hex separately
  vtkDataArray *localScalars = this->PointData->GetScalars();
  for (int i=0; i<8; i++)
    {
    for (int j=0; j<8; j++)
      {
      this->Hex->Points->SetPoint(j,this->Points->GetPoint(LinearHexs[i][j]));
      this->Hex->PointIds->SetId(j,this->PointIds->GetId(LinearHexs[i][j]));
      this->Scalars->SetValue(j,localScalars->GetTuple1(LinearHexs[i][j]));
      }
    this->Hex->Clip(value,this->Scalars,locator,tets,this->PointData,outPd,
                    this->CellData,0,outCd,insideOut);
    }
}

// Compute interpolation functions for the twenty nodes.
void vtkQuadraticHexahedron::InterpolationFunctions(float pcoords[3], 
                                                    float weights[20])
{
  //VTK needs parametric coordinates to be between (0,1). Isoparametric
  //shape functions are formulated between (-1,1). Here we do a 
  //coordinate system conversion from (0,1) to (-1,1).
  float r = 2.0*(pcoords[0]-0.5);
  float s = 2.0*(pcoords[1]-0.5);
  float t = 2.0*(pcoords[2]-0.5);

  float rm = 1.0 - r;
  float rp = 1.0 + r;
  float sm = 1.0 - s;
  float sp = 1.0 + s;
  float tm = 1.0 - t;
  float tp = 1.0 + t;
  float r2 = 1.0 - r*r;
  float s2 = 1.0 - s*s;
  float t2 = 1.0 - t*t;

  //The eight corner points
  weights[0] = 0.125 * rm * sm * tm * (-r - s - t - 2.0);
  weights[1] = 0.125 * rp * sm * tm * ( r - s - t - 2.0);
  weights[2] = 0.125 * rp * sp * tm * ( r + s - t - 2.0);
  weights[3] = 0.125 * rm * sp * tm * (-r + s - t - 2.0);
  weights[4] = 0.125 * rm * sm * tp * (-r - s + t - 2.0);
  weights[5] = 0.125 * rp * sm * tp * ( r - s + t - 2.0);
  weights[6] = 0.125 * rp * sp * tp * ( r + s + t - 2.0);
  weights[7] = 0.125 * rm * sp * tp * (-r + s + t - 2.0);
  
  //The mid-edge nodes
  weights[8] =  0.25 * r2 * sm * tm;
  weights[9] =  0.25 * s2 * rp * tm;
  weights[10] = 0.25 * r2 * sp * tm;
  weights[11] = 0.25 * s2 * rm * tm;
  weights[12] = 0.25 * r2 * sm * tp;
  weights[13] = 0.25 * s2 * rp * tp;
  weights[14] = 0.25 * r2 * sp * tp;
  weights[15] = 0.25 * s2 * rm * tp;
  weights[16] = 0.25 * t2 * rm * sm;
  weights[17] = 0.25 * t2 * rp * sm;
  weights[18] = 0.25 * t2 * rp * sp;
  weights[19] = 0.25 * t2 * rm * sp;
}

// Derivatives in parametric space.
void vtkQuadraticHexahedron::InterpolationDerivs(float pcoords[3], 
                                                 float derivs[60])
{
  //VTK needs parametric coordinates to be between (0,1). Isoparametric
  //shape functions are formulated between (-1,1). Here we do a 
  //coordinate system conversion from (0,1) to (-1,1).
  float r = 2.0*(pcoords[0]-0.5);
  float s = 2.0*(pcoords[1]-0.5);
  float t = 2.0*(pcoords[2]-0.5);

  float rm = 1.0 - r;
  float rp = 1.0 + r;
  float sm = 1.0 - s;
  float sp = 1.0 + s;
  float tm = 1.0 - t;
  float tp = 1.0 + t;

  //r-derivatives
  derivs[0] = -0.125*(sm*tm - 2.0*r*sm*tm - s*sm*tm - t*sm*tm - 2.0*sm*tm);
  derivs[1] =  0.125*(sm*tm + 2.0*r*sm*tm - s*sm*tm - t*sm*tm - 2.0*sm*tm);
  derivs[2] =  0.125*(sp*tm + 2.0*r*sp*tm + s*sp*tm - t*sp*tm - 2.0*sp*tm);
  derivs[3] = -0.125*(sp*tm - 2.0*r*sp*tm + s*sp*tm - t*sp*tm - 2.0*sp*tm);
  derivs[4] = -0.125*(sm*tp - 2.0*r*sm*tp - s*sm*tp + t*sm*tp - 2.0*sm*tp);
  derivs[5] =  0.125*(sm*tp + 2.0*r*sm*tp - s*sm*tp + t*sm*tp - 2.0*sm*tp);
  derivs[6] =  0.125*(sp*tp + 2.0*r*sp*tp + s*sp*tp + t*sp*tp - 2.0*sp*tp);
  derivs[7] = -0.125*(sp*tp - 2.0*r*sp*tp + s*sp*tp + t*sp*tp - 2.0*sp*tp);
  derivs[8] = -0.5*r*sm*tm;
  derivs[9] =  0.25*(tm - s*s*tm);
  derivs[10] = -0.5*r*sp*tm;
  derivs[11] = -0.25*(tm - s*s*tm);
  derivs[12] = -0.5*r*sm*tp;
  derivs[13] =  0.25*(tp - s*s*tp);
  derivs[14] = -0.5*r*sp*tp;
  derivs[15] = -0.25*(tp - s*s*tp);
  derivs[16] = -0.25*(sm - t*t*sm);
  derivs[17] =  0.25*(sm - t*t*sm);
  derivs[18] =  0.25*(sp - t*t*sp);
  derivs[19] = -0.25*(sp - t*t*sp);

  //s-derivatives
  derivs[20] = -0.125*(rm*tm - 2.0*s*rm*tm - r*rm*tm - t*rm*tm - 2.0*rm*tm);
  derivs[21] = -0.125*(rp*tm - 2.0*s*rp*tm + r*rp*tm - t*rp*tm - 2.0*rp*tm); 
  derivs[22] =  0.125*(rp*tm + 2.0*s*rp*tm + r*rp*tm - t*rp*tm - 2.0*rp*tm); 
  derivs[23] =  0.125*(rm*tm + 2.0*s*rm*tm - r*rm*tm - t*rm*tm - 2.0*rm*tm); 
  derivs[24] = -0.125*(rm*tp - 2.0*s*rm*tp - r*rm*tp + t*rm*tp - 2.0*rm*tp); 
  derivs[25] = -0.125*(rp*tp - 2.0*s*rp*tp + r*rp*tp + t*rp*tp - 2.0*rp*tp); 
  derivs[26] =  0.125*(rp*tp + 2.0*s*rp*tp + r*rp*tp + t*rp*tp - 2.0*rp*tp); 
  derivs[27] =  0.125*(rm*tp + 2.0*s*rm*tp - r*rm*tp + t*rm*tp - 2.0*rm*tp); 
  derivs[28] = -0.25*(tm - r*r*tm);
  derivs[29] = -0.5*s*rp*tm;
  derivs[20] =  0.25*(tm - r*r*tm);
  derivs[31] = -0.5*s*rm*tm;
  derivs[32] = -0.25*(tp - r*r*tp);
  derivs[33] = -0.5*s*rp*tp;
  derivs[34] =  0.25*(tp - r*r*tp);
  derivs[35] = -0.5*s*rm*tp;
  derivs[36] = -0.25*(rm - t*t*rm);
  derivs[37] = -0.25*(rp - t*t*rp); 
  derivs[38] =  0.25*(rp - t*t*rp);
  derivs[39] =  0.25*(rm - t*t*rm);

  //t-derivatives
  derivs[40] = -0.125*(rm*sm - 2.0*t*rm*sm - r*rm*sm - s*rm*sm - 2.0*rm*sm); 
  derivs[41] = -0.125*(rp*sm - 2.0*t*rp*sm + r*rp*sm - s*rp*sm - 2.0*rp*sm);  
  derivs[42] = -0.125*(rp*sp - 2.0*t*rp*sp + r*rp*sp + s*rp*sp - 2.0*rp*sp);  
  derivs[43] = -0.125*(rm*sp - 2.0*t*rm*sp - r*rm*sp + s*rm*sp - 2.0*rm*sp);  
  derivs[44] =  0.125*(rm*sm + 2.0*t*rm*sm - r*rm*sm - s*rm*sm - 2.0*rm*sm);  
  derivs[45] =  0.125*(rp*sm + 2.0*t*rp*sm + r*rp*sm - s*rp*sm - 2.0*rp*sm);  
  derivs[46] =  0.125*(rp*sp + 2.0*t*rp*sp + r*rp*sp + s*rp*sp - 2.0*rp*sp);  
  derivs[47] =  0.125*(rm*sp + 2.0*t*rm*sp - r*rm*sp + s*rm*sp - 2.0*rm*sp);  
  derivs[48] = -0.25*(sm - r*r*sm);
  derivs[49] = -0.25*(rp - s*s*rp);
  derivs[50] = -0.25*(sp - r*r*sp);
  derivs[51] = -0.25*(rm - s*s*rm);
  derivs[52] =  0.25*(sm - r*r*sm);
  derivs[53] =  0.25*(rp - s*s*rp);
  derivs[54] =  0.25*(sp - r*r*sp);
  derivs[55] =  0.25*(rm - s*s*rm);
  derivs[56] = -0.5*t*rm*sm;
  derivs[57] = -0.5*t*rp*sm;
  derivs[58] = -0.5*t*rp*sp;
  derivs[59] = -0.5*t*rm*sp;
}

