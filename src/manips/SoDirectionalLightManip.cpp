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
  \class SoDirectionalLightManip SoDirectionalLightManip.h Inventor/manips/SoDirectionalLightManip.h
  \brief The SoDirectionalLightManip class is used to manipulate directional light nodes.
  \ingroup manips

  A manipulator is used by replacing the node you want to edit in the graph with
  the manipulator. Draggers are used to manipulate the node. When manipulation
  is finished, the node is put back into the graph, replacing the manipulator.
*/

#include <Inventor/manips/SoDirectionalLightManip.h>
#include <Inventor/draggers/SoDirectionalLightDragger.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/SbMatrix.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SoFullPath.h>
#include <Inventor/SoNodeKitPath.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

SO_NODE_SOURCE(SoDirectionalLightManip);


void
SoDirectionalLightManip::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoDirectionalLightManip);
}

SoDirectionalLightManip::SoDirectionalLightManip(void)
{
  SO_NODE_INTERNAL_CONSTRUCTOR(SoDirectionalLightManip);

  this->children = new SoChildList(this);
  this->directionFieldSensor = new SoFieldSensor(SoDirectionalLightManip::fieldSensorCB, this);
  this->directionFieldSensor->setPriority(0);

  this->colorFieldSensor = new SoFieldSensor(SoDirectionalLightManip::fieldSensorCB, this);
  this->colorFieldSensor->setPriority(0);

  this->attachSensors(TRUE);
  this->setDragger(new SoDirectionalLightDragger);
}


SoDirectionalLightManip::~SoDirectionalLightManip()
{
  this->setDragger(NULL);

  delete this->colorFieldSensor;
  delete this->directionFieldSensor;

  delete this->children;
}

void
SoDirectionalLightManip::setDragger(SoDragger * newdragger)
{
  SoDragger *olddragger = this->getDragger();
  if (olddragger) {
    olddragger->removeValueChangedCallback(SoDirectionalLightManip::valueChangedCB, this);
    this->children->remove(0);
  }
  if (newdragger != NULL) {
    if (this->children->getLength() > 0) {
      this->children->set(0, newdragger);
    }
    else {
      this->children->append(newdragger);
      SoDirectionalLightManip::fieldSensorCB(this, NULL);
      newdragger->addValueChangedCallback(SoDirectionalLightManip::valueChangedCB, this);
    }
  }
}

SoDragger *
SoDirectionalLightManip::getDragger(void)
{
  if (this->children->getLength() > 0) {
    SoNode *node = (*children)[0];
    if (node->isOfType(SoDragger::getClassTypeId()))
      return (SoDragger*)node;
    else {
#if COIN_DEBUG
      SoDebugError::post("SoDirectionalLightManip::getDragger",
                         "Child is not a dragger!");
#endif // debug
    }
  }
  return NULL;
}

/*!
  Replaces the node specified by \a path with this manipulator.
  The manipulator will copy the field data from the node, to make
  it affect the state in the same way as the node.
*/
SbBool
SoDirectionalLightManip::replaceNode(SoPath * path)
{
  SoFullPath *fullpath = (SoFullPath*)path;
  SoNode *fulltail = fullpath->getTail();
  if (!fulltail->isOfType(SoDirectionalLight::getClassTypeId())) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceNode",
                       "End of path is not a SoDirectionalLight");
#endif // debug
    return FALSE;
  }
  SoNode *tail = path->getTail();
  if (tail->isOfType(SoBaseKit::getClassTypeId())) {
    SoBaseKit *kit = (SoBaseKit*) ((SoNodeKitPath*)path)->getTail();
    SbString partname = kit->getPartString(path);
    if (partname != "") {
      SoDirectionalLight *oldpart = (SoDirectionalLight*) kit->getPart(partname, TRUE);
      if (oldpart != NULL) {
        this->attachSensors(FALSE);
        this->transferFieldValues(oldpart, this);
        this->attachSensors(TRUE);
        SoDirectionalLightManip::fieldSensorCB(this, NULL);
        kit->setPart(partname, this);
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
  }
  if (fullpath->getLength() < 2) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceNode",
                       "Path is too short");
#endif // debug
    return FALSE;
  }
  SoNode *parent = fullpath->getNodeFromTail(1);
  if (!parent->isOfType(SoGroup::getClassTypeId())) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceNode",
                       "Parent node is not a group");
#endif // debug
    return FALSE;
  }
  this->ref();
  this->attachSensors(FALSE);
  this->transferFieldValues((SoDirectionalLight*)fulltail, this);
  this->attachSensors(TRUE);
  SoDirectionalLightManip::fieldSensorCB(this, NULL);

  ((SoGroup*)parent)->replaceChild(fulltail, this);
  this->unrefNoDelete();
  return TRUE;
}

/*!
  Replaces this manipulator specified by \a path with \a newnode. If
  \a newnode is \e NULL, a SoDirectionalLight will be created for you.
*/
SbBool
SoDirectionalLightManip::replaceManip(SoPath * path, SoDirectionalLight * newone) const
{
  SoFullPath *fullpath = (SoFullPath*) path;
  SoNode *fulltail = fullpath->getTail();
  if (fulltail != (SoNode*)this) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceManip",
                       "Child to replace is not this manip");
#endif // debug
  }
  SoNode *tail = path->getTail();
  if (tail->isOfType(SoBaseKit::getClassTypeId())) {
    SoBaseKit *kit = (SoBaseKit*) ((SoNodeKitPath*)path)->getTail();
    SbString partname = kit->getPartString(path);
    if (partname != "") {
      if (newone != NULL) {
        this->transferFieldValues(this, newone);
      }
      kit->setPart(partname, newone);
      return TRUE;
    }
  }
  if (fullpath->getLength() < 2) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceManip",
                       "Path is too short");
#endif // debug
    return FALSE;
  }
  SoNode *parent = fullpath->getNodeFromTail(1);
  if (!parent->isOfType(SoGroup::getClassTypeId())) {
#if COIN_DEBUG
    SoDebugError::post("SoDirectionalLightManip::replaceManip",
                       "Parent node is not a group");
#endif // debug
    return FALSE;
  }
  if (newone == NULL) newone = new SoDirectionalLight;
  newone->ref();
  this->transferFieldValues(this, newone);
  ((SoGroup*)parent)->replaceChild((SoNode*)this, newone);
  newone->unrefNoDelete();
  return TRUE;
}

void
SoDirectionalLightManip::doAction(SoAction * action)
{
  int numindices;
  const int *indices;
  if (action->getPathCode(numindices, indices) == SoAction::IN_PATH) {
    this->children->traverse(action, 0, indices[numindices-1]);
  }
  else {
    this->children->traverse(action);
  }
}

void
SoDirectionalLightManip::callback(SoCallbackAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::callback(action);
}

/*!
 */
void
SoDirectionalLightManip::GLRender(SoGLRenderAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::GLRender(action);
}

/*!
 */
void
SoDirectionalLightManip::getBoundingBox(SoGetBoundingBoxAction * action)
{
  int numindices;
  const int *indices;
  int lastchild;
  SbVec3f center(0.0f, 0.0f, 0.0f);
  int numcenters = 0;

  if (action->getPathCode(numindices, indices) == SoAction::IN_PATH) {
    lastchild  = indices[numindices-1];
  }
  else {
    lastchild = this->children->getLength() - 1;
  }
  for (int i = 0; i <= lastchild; i++) {
    this->children->traverse(action, i, i);
    if (action->isCenterSet()) {
      center += action->getCenter();
      numcenters++;
      action->resetCenter();
    }
  }
  SoDirectionalLight::getBoundingBox(action);
  if (action->isCenterSet()) {
    center += action->getCenter();
    numcenters++;
    action->resetCenter();
  }
  if (numcenters != 0) {
    action->setCenter(center / numcenters, FALSE);
  }
}

/*!
 */
void
SoDirectionalLightManip::getMatrix(SoGetMatrixAction * action)
{
  int numindices;
  const int *indices;
  switch (action->getPathCode(numindices, indices)) {
  case SoAction::NO_PATH:
  case SoAction::BELOW_PATH:
    break;
  case SoAction::IN_PATH:
    this->children->traverse(action, 0, indices[numindices-1]);
    break;
  case SoAction::OFF_PATH:
    this->children->traverse(action);
    break;
  default:
    assert(0 && "unknown path code");
    break;
  }
}

/*!
 */
void
SoDirectionalLightManip::handleEvent(SoHandleEventAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::handleEvent(action);
}

/*!
 */
void
SoDirectionalLightManip::pick(SoPickAction * action)
{
  SoDirectionalLightManip::doAction(action);
  SoDirectionalLight::pick(action);
}

/*!
 */
void
SoDirectionalLightManip::search(SoSearchAction * action)
{
  inherited::search(action);
  if (action->isFound()) return;
  SoDirectionalLightManip::doAction(action);
}

/*!
  Returns the children of this node. This node only has the dragger
  as a child.
*/
SoChildList *
SoDirectionalLightManip::getChildren(void) const
{
  return this->children;
}

//
// convert from matrix to direction
//
void
SoDirectionalLightManip::valueChangedCB(void * m, SoDragger * dragger)
{
  SoDirectionalLightManip * thisp = (SoDirectionalLightManip*)m;

  SbMatrix matrix = dragger->getMotionMatrix();
  SbVec3f direction(0.0f, 0.0f, -1.0f);
  matrix.multDirMatrix(direction, direction);
  direction.normalize();

  thisp->attachSensors(FALSE);
  if (thisp->direction.getValue() != direction) {
    thisp->direction = direction;
  }
  thisp->attachSensors(TRUE);
}

void
SoDirectionalLightManip::fieldSensorCB(void * m, SoSensor *)
{
  SoDirectionalLightManip *thisp = (SoDirectionalLightManip*)m;
  SoDragger *dragger = thisp->getDragger();
  if (dragger != NULL) {
    SbVec3f direction = thisp->direction.getValue();
    SbMatrix matrix = dragger->getMotionMatrix();
    SbVec3f t, s;
    SbRotation r, so;
    matrix.getTransform(t, r, s, so);
    r.setValue(SbVec3f(0.0f, 0.0f, -1.0f), direction);
    matrix.setTransform(t, r, s, so);
    dragger->setMotionMatrix(matrix);

    SoMaterial *material = (SoMaterial*)dragger->getPart("material", TRUE);
    material->diffuseColor = thisp->color.getValue();
    // FIXME: should I edit the emissive field?
    // pederb, 2000-02-04
  }
}

/*!
  Overloaded to copy the dragger.
*/
void
SoDirectionalLightManip::copyContents(const SoFieldContainer * fromfc, SbBool copyconnections)
{
  assert(fromfc->isOfType(SoDirectionalLightManip::getClassTypeId()));
  this->setDragger(((SoDirectionalLightManip*)fromfc)->getDragger());
  inherited::copyContents(fromfc, copyconnections);
}

/*!
  Copies field values.
*/
void
SoDirectionalLightManip::transferFieldValues(const SoDirectionalLight * from, SoDirectionalLight * to)
{
  to->direction = from->direction;
  to->color = from->color;
}

void
SoDirectionalLightManip::attachSensors(const SbBool onoff)
{
  if (onoff) {
    this->directionFieldSensor->attach(&this->direction);
    this->colorFieldSensor->attach(&this->color);
  }
  else {
    this->directionFieldSensor->detach();
    this->colorFieldSensor->detach();
  }
}
