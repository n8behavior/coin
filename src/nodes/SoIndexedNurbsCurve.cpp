/**************************************************************************\
 *
 *  Copyright (C) 1998-2000 by Systems in Motion.  All rights reserved.
 *
 *  This file is part of the Coin library.
 *
 *  This file may be distributed under the terms of the Q Public License
 *  as defined by Troll Tech AS of Norway and appearing in the file
 *  LICENSE.QPL included in the packaging of this file.
 *
 *  If you want to use Coin in applications not covered by licenses
 *  compatible with the QPL, you can contact SIM to aquire a
 *  Professional Edition license for Coin.
 *
 *  Systems in Motion AS, Prof. Brochs gate 6, N-7030 Trondheim, NORWAY
 *  http://www.sim.no/ sales@sim.no Voice: +47 22114160 Fax: +47 67172912
 *
\**************************************************************************/

/*!
  \class SoIndexedNurbsCurve SoIndexedNurbsCurve.h Inventor/nodes/SoIndexedNurbsCurve.h
  \brief The SoIndexedNurbsCurve class ...
  \ingroup nodes

  FIXME: write class doc
*/

#include <Inventor/nodes/SoIndexedNurbsCurve.h>
#include <coindefs.h> // COIN_STUB()



/*!
  \var SoSFInt32 SoIndexedNurbsCurve::numControlPoints
  FIXME: write documentation for field
*/
/*!
  \var SoMFInt32 SoIndexedNurbsCurve::coordIndex
  FIXME: write documentation for field
*/
/*!
  \var SoMFFloat SoIndexedNurbsCurve::knotVector
  FIXME: write documentation for field
*/


// *************************************************************************

SO_NODE_SOURCE(SoIndexedNurbsCurve);

/*!
  Constructor.
*/
SoIndexedNurbsCurve::SoIndexedNurbsCurve()
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoIndexedNurbsCurve);

  SO_NODE_ADD_FIELD(numControlPoints, (0));
  SO_NODE_ADD_FIELD(coordIndex, (0));
  SO_NODE_ADD_FIELD(knotVector, (0));
}

/*!
  Destructor.
*/
SoIndexedNurbsCurve::~SoIndexedNurbsCurve()
{
}

/*!
  Does initialization common for all objects of the
  SoIndexedNurbsCurve class. This includes setting up the
  type system, among other things.
*/
void
SoIndexedNurbsCurve::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoIndexedNurbsCurve);
}

/*!
  FIXME: write function documentation
*/
void
SoIndexedNurbsCurve::computeBBox(SoAction * /* action */,
                                 SbBox3f & /* box */, SbVec3f & /* center */)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
*/
void
SoIndexedNurbsCurve::GLRender(SoGLRenderAction * /* action */)
{
  COIN_STUB();

  //
  // See comment in SoNurbsSurface::GLRender
  //
}

/*!
  FIXME: write doc
*/
void
SoIndexedNurbsCurve::rayPick(SoRayPickAction * /* action */)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
*/
void
SoIndexedNurbsCurve::getPrimitiveCount(SoGetPrimitiveCountAction * /* action */)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
*/
void
SoIndexedNurbsCurve::getBoundingBox(SoGetBoundingBoxAction * /* action */)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
*/
void
SoIndexedNurbsCurve::sendPrimitive(SoAction * ,  SoPrimitiveVertex *)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
*/
void
SoIndexedNurbsCurve::generatePrimitives(SoAction * /* action */)
{
  COIN_STUB();
}
