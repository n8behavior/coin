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

#include <Inventor/manips/SoTransformBoxManip.h>
#include <coindefs.h> // COIN_STUB()

SO_NODE_SOURCE(SoTransformBoxManip);


void
SoTransformBoxManip::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoTransformBoxManip);
}

SoTransformBoxManip::SoTransformBoxManip(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoTransformBoxManip);

  // FIXME: stuff missing? 20000108 mortene.

  COIN_STUB();
}


SoTransformBoxManip::~SoTransformBoxManip()
{
  COIN_STUB();
}
