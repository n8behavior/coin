/**************************************************************************\
 *
 *  Copyright (C) 1998-1999 by Systems in Motion.  All rights reserved.
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
  \class SoNode SoNode.h Inventor/nodes/SoNode.h
  \brief The SoNode class ...
  \ingroup nodes

  FIXME: write class doc
*/

#include <Inventor/nodes/SoNode.h>

#include <Inventor/SbString.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoActions.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/nodes/SoNodes.h>
#include <Inventor/nodes/SoUnknownNode.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#ifndef NDEBUG
#include <GL/gl.h> // assert glGetError
#endif

/*!
  \enum SoNode::Stage
  FIXME: write documentation for enum
*/
/*!
  \var SoNode::Stage SoNode::FIRST_INSTANCE
  FIXME: write documentation for enum definition
*/
/*!
  \var SoNode::Stage SoNode::PROTO_INSTANCE
  FIXME: write documentation for enum definition
*/
/*!
  \var SoNode::Stage SoNode::OTHER_INSTANCE
  FIXME: write documentation for enum definition
*/


/*!
  \var uint32_t SoNode::uniqueId
  FIXME: write doc
*/
/*!
  \var uint32_t SoNode::nextUniqueId
  FIXME: write doc
*/
/*!
  \var int SoNode::nextActionMethodIndex
  FIXME: write doc
*/


// Syntax keywords for writing multiple references in .iv/.wrl files.
#define DEFINITION_KEYWORD "DEF"
#define REFERENCE_KEYWORD "USE"


uint32_t SoNode::nextUniqueId = 0;
int SoNode::nextActionMethodIndex = 0;
SoType SoNode::classTypeId = SoType::badType();


/*!
  FIXME: write function documentation
*/
SoType
SoNode::getClassTypeId(void)
{
  return SoNode::classTypeId;
}

/*!
  Constructor.
*/
SoNode::SoNode()
{
}

/*!
  Destructor.
*/
SoNode::~SoNode()
{
  // FIXME: detach any node sensor(s) to avoid references to free'd
  // memory. 19981027 mortene.
}

/*!
  FIXME: write function documentation
*/
SoNode *
SoNode::copy(SbBool /* copyConnections */) const
{
  SoNode * newnode = (SoNode *)(this->getTypeId().createInstance());
  assert(newnode != NULL);

#if 0 // FIXME: tmp disabled during development. 19980923 mortene.
  newnode->setName(this->getName());
  newnode->copyFieldValues(this);
#endif // disabled

  return newnode;
}


/*!
  FIXME: write function documentation
*/
void
SoNode::notify(SoNotList * list)
{
  // FIXME: should put ourselves in the list. 19990701 mortene.
  inherited::notify(list);
}

/*!
  FIXME: write function documentation
*/
int
SoNode::getActionMethodIndex(const SoType type)
{
  return type.getData();
}

/*!
  Does initialization common for all objects of the
  SoNode class. This includes setting up the
  type system, among other things.
*/
void
SoNode::initClass(void)
{
  // Make sure we only initialize once.
  assert(SoNode::classTypeId == SoType::badType());
  // Make sure parent class has been initialized.
  assert(inherited::getClassTypeId() != SoType::badType());

  SoNode::classTypeId =
    SoType::createType(inherited::getClassTypeId(), "Node", NULL,
                       SoNode::nextActionMethodIndex++);

  SoNode::initClasses();
}



/*!
  FIXME: write function documentation
*/
void
SoNode::initClasses(void)
{
  SoCamera::initClass();
  SoPerspectiveCamera::initClass();
  SoOrthographicCamera::initClass();
  SoShape::initClass();
  SoAnnoText3::initClass();
  SoAsciiText::initClass();
  SoCone::initClass();
  SoCube::initClass();
  SoCylinder::initClass();
  SoVertexShape::initClass();
  SoNonIndexedShape::initClass();
  SoFaceSet::initClass();
  SoLineSet::initClass();
  SoPointSet::initClass();
  SoMarkerSet::initClass();
  SoQuadMesh::initClass();
  SoTriangleStripSet::initClass();
  SoIndexedShape::initClass();
  SoIndexedFaceSet::initClass();
  SoIndexedLineSet::initClass();
  SoIndexedTriangleStripSet::initClass();
  SoImage::initClass();
  SoIndexedNurbsCurve::initClass();
  SoIndexedNurbsSurface::initClass();
  SoNurbsCurve::initClass();
  SoNurbsSurface::initClass();
  SoSphere::initClass();
  SoText2::initClass();
  SoText3::initClass();
  SoAnnoText3Property::initClass();
  SoGroup::initClass();
  SoSeparator::initClass();
  SoAnnotation::initClass();
  SoLocateHighlight::initClass();
  SoWWWAnchor::initClass();
  SoArray::initClass();
  SoSwitch::initClass();
  SoBlinker::initClass();
  SoLOD::initClass();
  SoLevelOfSimplification::initClass();
  SoLevelOfDetail::initClass();
  SoMultipleCopy::initClass();
  SoPathSwitch::initClass();
  SoTransformSeparator::initClass();
  SoTransformation::initClass();
  SoMatrixTransform::initClass();
  SoRotation::initClass();
  SoPendulum::initClass();
  SoRotor::initClass();
  SoResetTransform::initClass();
  SoRotationXYZ::initClass();
  SoScale::initClass();
  SoTranslation::initClass();
  SoShuttle::initClass();
  SoTransform::initClass();
  SoUnits::initClass();
  SoBaseColor::initClass();
  SoCallback::initClass();
  SoClipPlane::initClass();
  SoColorIndex::initClass();
  SoComplexity::initClass();
  SoCoordinate3::initClass();
  SoCoordinate4::initClass();
  SoLight::initClass();
  SoDirectionalLight::initClass();
  SoSpotLight::initClass();
  SoPointLight::initClass();
  SoDrawStyle::initClass();
  SoEnvironment::initClass();
  SoEventCallback::initClass();
  SoFile::initClass();
  SoFont::initClass();
  SoFontStyle::initClass();
  SoInfo::initClass();
  SoLabel::initClass();
  SoLightModel::initClass();
  SoProfile::initClass();
  SoLinearProfile::initClass();
  SoNurbsProfile::initClass();
  SoMaterial::initClass();
  SoMaterialBinding::initClass();
  SoNormal::initClass();
  SoNormalBinding::initClass();
  SoPackedColor::initClass();
  SoPickStyle::initClass();
  SoPolygonOffset::initClass();
  SoProfileCoordinate2::initClass();
  SoProfileCoordinate3::initClass();
  SoShapeHints::initClass();
  SoTexture2::initClass();
  SoTexture2Transform::initClass();
  SoTextureCoordinate2::initClass();
  SoTextureCoordinateBinding::initClass();
  SoTextureCoordinateFunction::initClass();
  SoTextureCoordinateDefault::initClass();
  SoTextureCoordinateEnvironment::initClass();
  SoTextureCoordinatePlane::initClass();
  SoUnknownNode::initClass();
  SoVertexProperty::initClass();
  SoWWWInline::initClass();
}

// *************************************************************************

/*!
  FIXME: write function documentation
*/
void
SoNode::setOverride(const SbBool state)
{
  this->stateflags.override = state;
}

/*!
  FIXME: write function documentation
*/
SbBool
SoNode::isOverride(void) const
{
  return this->stateflags.override;
}

// *************************************************************************
// * ACTION STUFF
// *************************************************************************

/*!
  This function performs the typical operation of a node for any action.
  Default method does nothing.
*/
void
SoNode::doAction(SoAction *)
{
}

/*!
  FIXME: write function documentation
*/
SbBool
SoNode::affectsState() const
{
  return TRUE; // default
}

/*!
  Returns the last node that was give \a name in SoBase::setName()
*/
SoNode *
SoNode::getByName(const SbName & /* name */)
{
  // got to search through dictionary to find the last node with
  // 'name'
  assert(0);
  return NULL;
}

/*!
  Finds all nodes with name set the \a name. Returns the number
  of nodes with the specified name.
*/
int
SoNode::getByName(const SbName & /* name */, SoNodeList & /* list */)
{
  // FIXME
  assert(0);
  return 0;
}

/*!
  FIXME: write function documentation
*/
void
SoNode::getBoundingBoxS(SoAction * const action, SoNode * const node)
{
  assert(action && node);
  //  assert(action->getTypeId().isDerivedFrom(SoGetBoundingBoxAction::getClassTypeId()));
  SoGetBoundingBoxAction * bboxaction = (SoGetBoundingBoxAction *)action;
  bboxaction->checkResetBefore();
  node->getBoundingBox(bboxaction);
  bboxaction->checkResetAfter();
}

/*!
  FIXME: write function documentation
*/
void
SoNode::getBoundingBox(SoGetBoundingBoxAction *)
{
}

/*!
  FIXME: write function documentation
*/
void
SoNode::getPrimitiveCountS(SoAction * const action, SoNode * const node)
{
  assert(action && node);
  //  assert(action->getTypeId().isDerivedFrom(SoGetPrimitiveCountAction::getClassTypeId()));
  node->getPrimitiveCount((SoGetPrimitiveCountAction *)action);
}

/*!
  FIXME: write function documentation
*/
void
SoNode::getPrimitiveCount(SoGetPrimitiveCountAction *)
{
}

/*!
  This static method is used for calling the respective virtual method on the
  node.
*/
void
SoNode::GLRenderS(SoAction * const action,
                  SoNode * const node)
{
  assert(action && node);
  assert(action->getTypeId().isDerivedFrom(SoGLRenderAction::getClassTypeId()));
  SoGLRenderAction * const renderAction =
    (SoGLRenderAction *)(action);

  switch (action->getCurPathCode()) {
  case SoAction::NO_PATH:
    node->GLRender(renderAction);
    break;
  case SoAction::IN_PATH:
    node->GLRenderInPath(renderAction);
    break;
  case SoAction::OFF_PATH:
    if (node->affectsState()) node->GLRenderOffPath(renderAction);
    break;
  case SoAction::BELOW_PATH:
    node->GLRenderBelowPath(renderAction);
    break;
  default:
    assert(0 && "Unknown path code");
    break;
  }
#if COIN_DEBUG
  int err = glGetError();
  if (err != GL_NO_ERROR) {
    SoDebugError::postInfo("SoNode::GLRenderS",
                           "error: 0x%x, node: %s",
                           err, node->getTypeId().getName().getString());
  }
#endif // COIN_DEBUG
}

/*!
  This method is used by SoGLRenderAction render traversals. The default
  method does nothing, of course, so all classes which does rendering
  or influences rendering state need to reimplement this.
*/
void
SoNode::GLRender(SoGLRenderAction * /* action */)
{
}

/*!
  FIXME: write doc
*/
void
SoNode::GLRenderBelowPath(SoGLRenderAction * action)
{
  this->GLRender(action);
}

/*!
  FIXME: write doc
*/
void
SoNode::GLRenderInPath(SoGLRenderAction * action)
{
  this->GLRender(action);
}

/*!
  FIXME: write doc
*/
void
SoNode::GLRenderOffPath(SoGLRenderAction * action)
{
  this->GLRender(action);
}

/*!
  FIXME: write function documentation
*/
void
SoNode::callbackS(SoAction * const action,
                  SoNode * const node)
{
  assert(action && node);
  SoCallbackAction * const cbAction =
    (SoCallbackAction *)(action);
  cbAction->setCurrentNode(node);

  if (node->getChildren() == NULL) {
    cbAction->invokePreCallbacks(node);
    if (cbAction->getCurrentResponse() == SoCallbackAction::CONTINUE) {
      node->callback(cbAction);
      cbAction->invokePostCallbacks(node);
    }
  }
  else {
    // group node handles callbacks themselves
    // Separator needs to push state before calling the callbacks
    node->callback(cbAction);
  }
}

/*!
  FIXME: write function documentation
*/
void
SoNode::callback(SoCallbackAction * /* action */)
{
}

/*!
  FIXME: write function documentation
*/
void
SoNode::getMatrixS(SoAction * const action, SoNode * const node)
{
#if 0 // debug
  SoDebugError::postInfo("SoNode::getMatrixS",
                         "%s",
                         node->getTypeId().getName().getString());
#endif // debug

  assert(action && node);
  assert(action->getTypeId() == SoGetMatrixAction::getClassTypeId());
  SoGetMatrixAction * const getMatrixAction =
    (SoGetMatrixAction *)(action);
  node->getMatrix(getMatrixAction);
}

/*!
//FIXME: How does doxygen manage links from classnames in the documentation?
//       (kintel 19990616)
  This is the default virtual function used by SoGetMatrixAction.
  It may be overloaded in subclasses (usually SoTransform subclasses)
*/
void
SoNode::getMatrix(SoGetMatrixAction * /* action */)
{
  //FIXME: This should return identity as default (kintel 19990615)
}

/*!
  FIXME: write function documentation
*/
void
SoNode::handleEventS(SoAction * const action, SoNode * const node)
{
  assert(action && node);
  assert(action->getTypeId().isDerivedFrom(SoHandleEventAction::getClassTypeId()));
  SoHandleEventAction * const handleEventAction =
    (SoHandleEventAction *)(action);
  node->handleEvent(handleEventAction);
}

/*!
  FIXME: write function documentation
*/
void
SoNode::handleEvent(SoHandleEventAction * /* action */)
{
}

/*!
  FIXME: write function documentation
*/
void
SoNode::pickS(SoAction * const action, SoNode * const node)
{
  assert(action && node);
  assert(action->getTypeId().isDerivedFrom(SoPickAction::getClassTypeId()));
  SoPickAction * const pickAction =
    (SoPickAction *)(action);
  node->pick(pickAction);
}

/*!
  FIXME: write function documentation
*/
void
SoNode::pick(SoPickAction * /* action */)
{
}

/*!
  FIXME: write function documentation
*/
void
SoNode::rayPickS(SoAction * const action, SoNode * const node)
{
  assert(action && node);
  assert(action->getTypeId().isDerivedFrom(SoRayPickAction::getClassTypeId()));

#if 0 // debug
  SoDebugError::postInfo("SoNode::rayPickS",
                         "%s", node->getTypeId().getName().getString());
#endif // debug

  SoRayPickAction * const rayPickAction =
    (SoRayPickAction *)(action);
  node->rayPick(rayPickAction);
}

/*!
  FIXME: write function documentation
*/
void
SoNode::rayPick(SoRayPickAction * /* action */)
{
}

/*!
  FIXME: write function documentation
*/
void
SoNode::searchS(SoAction * const action, SoNode * const node)
{
  assert(action && node);
  assert(action->getTypeId().isDerivedFrom(SoSearchAction::getClassTypeId()));
  SoSearchAction * const searchAction =
    (SoSearchAction *)(action);
  node->search(searchAction);
}

/*!
  This method is used by SoSearchAction instances to traverse and search the
  scene graph.  It must be overridden in SoSwitch nodes to take into account
  if the search involves every single node, or just the nodes involved in
  normal graph traversal.
*/
void
SoNode::search(SoSearchAction * action)
{
  if (action->isFound())
    return;

  int lookFor = action->getFind();
  SbBool flag;

  if ((lookFor & SoSearchAction::NODE) &&
       (this == action->getNode())) {
    action->addPath(action->getCurPath()->copy());
#if 0 // debug
    SoDebugError::postInfo("SoNode::search", "NODE match found.\n");
#endif // debug
  } else if ((lookFor & SoSearchAction::NAME) &&
      (this->getName() == action->getName())) {
    action->addPath(action->getCurPath()->copy());
#if 0 // debug
    SoDebugError::postInfo("SoNode::search", "NAME match found.\n");
#endif // debug
  } else if ((lookFor & SoSearchAction::TYPE) &&
      ((this->getTypeId() == action->getType(flag)) ||
      (flag && this->getTypeId().isDerivedFrom(action->getType(flag))))) {
    action->addPath(action->getCurPath()->copy());
#if 0 // debug
    SoDebugError::postInfo("SoNode::search", "TYPE match found.\n");
#endif // debug
  }
#if 0 // debug
  else {
    SoDebugError::postInfo("SoNode::search", "no match for %p (type '%s').",
                           this, this->getTypeId().getName().getString());
  }
#endif // debug


  //doAction(action);  // this is correct, right? I don't think so: pederb
}

/*!
  \internal

  Static method registered with SoWriteAction. Just calls the virtual
  write() method.
*/
void
SoNode::writeS(SoAction * const action, SoNode * const node)
{
  assert(action && node);
  assert(action->getTypeId().isDerivedFrom(SoWriteAction::getClassTypeId()));
  SoWriteAction * const writeAction = (SoWriteAction *)(action);
  node->write(writeAction);
}

/*!
  This is the default method for writing out a node object. It is used
  by SoWriteAction.
*/
void
SoNode::write(SoWriteAction * action)
{
  // Note: make sure this code is in sync with the code in
  // SoSFNode::writeValue(). Any changes here need to be propagated to
  // that method.

  SoOutput * out = action->getOutput();
  if (out->getStage() == SoOutput::COUNT_REFS) {
    this->addWriteReference(out, FALSE);
  }
  else if (out->getStage() == SoOutput::WRITE) {
    if (this->writeHeader(out, FALSE, FALSE)) return;
    this->writeInstance(out);
    this->writeFooter(out);
  }
  else assert(0 && "unknown stage");
}

/*!
  FIXME: write function documentation
*/
SoChildList *
SoNode::getChildren(void) const
{
  return NULL;
}

/*!
  FIXME: write doc
*/
void
SoNode::grabEventsSetup(void)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
*/
void
SoNode::grabEventsCleanup(void)
{
  COIN_STUB();
}

/*!
  FIXME: write doc
*/
void
SoNode::startNotify(void)
{
  SoNotList list; // FIXME: what should be added to this list? pederb, 19991214
  this->notify(&list);
}

/*!
  FIXME: write function documentation
*/
uint32_t
SoNode::getNodeId(void) const
{
  return this->uniqueId;
}

/*!
  This method is called from write() if we're in the actual writing pass
  of the write action taking place. It dumps the node to the given output
  stream.
*/
void
SoNode::writeInstance(SoOutput * out)
{
  // Fields are written from SoFieldContainer.
  inherited::writeInstance(out);
}

/*!
  FIXME: write function documentation
*/
SoNode *
SoNode::addToCopyDict(void) const
{
  COIN_STUB();
  return NULL;
}

/*!
  FIXME: write function documentation
*/
void
SoNode::copyContents(const SoFieldContainer * /* fromFC */,
                     SbBool /* copyConnections */)
{
  COIN_STUB();
}

/*!
  FIXME: write function documentation
*/
SoFieldContainer *
SoNode::copyThroughConnection(void) const
{
  COIN_STUB();
  return NULL;
}

/*!
  FIXME: write function documentation
*/
uint32_t
SoNode::getNextNodeId(void)
{
  return SoNode::nextUniqueId;
}

/*!
  FIXME: write function documentation
*/
const SoFieldData **
SoNode::getFieldDataPtr(void)
{
  COIN_STUB();
  return NULL;
}
