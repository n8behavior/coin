#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef HAVE_SCENE_PROFILING

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2008 by Kongsberg SIM.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Kongsberg SIM about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Kongsberg SIM, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

#include <Inventor/annex/Profiler/nodes/SoProfilerStats.h>

#include <map>

#include <Inventor/actions/SoAudioRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/lists/SbList.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/elements/SoCacheElement.h>

#include "nodes/SoSubNodeP.h"
#include "profiler/SoProfilerElement.h"
#include "profiler/SbProfilingData.h"

// decide whether to expose profiling stats by node type or by
// user-specified node names
#define BY_TYPE 1

// *************************************************************************

/*!
  \class SoProfilerStats SoProfilerStats.h Inventor/annex/Profiler/nodes/SoProfilerStats.h
  \brief The SoProfilerStats class is a node for exposing profiling results gathered by \a SoProfilerElement.
  \ingroup profiler

  TODO: Describe functionality and provide usage example.

  \sa SoProfilerElement
*/

/*!
  \var SoMFName SoProfilerStats::nodeType

  Name of type types traversed during the current render traversal.

  This field is tightly connected to the \a timePerNodeType field as
  they will contain the same number of elements, and each value
  in \a timePerNodeType matches the node type with the same index in 
  this field.

  \sa SoProfilerStats::timePerNodeType
*/

/*!
  \var SoMFTime SoProfilerStats::timePerNodeType

  Time spent, during the current render traversal, per node type.

  This field is tightly connected to the \a nodeType field as
  they will contain the same number of elements, and each value
  in this field matches the node type with the same index in 
  \a nodeType.

  \sa SoProfilerStats::nodeType
*/

// *************************************************************************

#define PUBLIC(obj) ((obj)->master)

namespace {
struct TypeTimings {
  SbTime max, total;
  int32_t count;
};
}

class SoProfilerStatsP {
public:
  SoProfilerStatsP(void) : master(NULL) {
  }
  ~SoProfilerStatsP(void) { }

  void doAction(SoAction * action);
  void clearProfilingData(void);
  void updateActionTimingMaps(SoProfilerElement * e, SoAction * action);
  void updateActionTimingFields(SoProfilerElement * e);
  void updateNodeTypeTimingMap(SoProfilerElement * e);
  void updateNodeTypeTimingFields();

  std::map<int16_t, SbProfilingData *> action_map;
  std::map<int16_t, TypeTimings> type_timings;
  std::map<const char *, TypeTimings> name_timings;

  SoProfilerStats * master;

  SbHash<SbTime, int16_t> action_timings;
}; // SoProfilerStatsP
  
void
SoProfilerStatsP::doAction(SoAction * action)
{
  // every SoGLRenderAction traversal will set a flag that causes
  // the profiling data to be cleared at the next traversal, regardless of 
  // type
  static SbBool clear_state = FALSE;
  if (clear_state) {
    this->clearProfilingData();
    clear_state = FALSE;
  }

  this->action_timings.put(SoGLRenderAction::getClassTypeId().getKey(), 
                           SbTime::zero());

  SoState * state = action->getState();
  if (state->isElementEnabled(SoCacheElement::getClassStackIndex())) {
    SoCacheElement::invalidate(state);
  }

  SoProfilerElement * e = SoProfilerElement::get(state);
  if (!e) { return; }

  
  std::map<int16_t, SbProfilingData *>::iterator it = 
    this->action_map.find(action->getTypeId().getKey());
  if (it != this->action_map.end()) {
    (*((*it).second)) += e->getProfilingData();
  } else {
    SbProfilingData * data = new SbProfilingData;
    (*data) += e->getProfilingData();
    std::pair<int16_t, SbProfilingData *> entry(action->getTypeId().getKey(), data);
    this->action_map.insert(entry);
  }

  this->updateNodeTypeTimingMap(e);
  this->updateActionTimingMaps(e, action);

  if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
    this->updateNodeTypeTimingFields();
    updateActionTimingFields(e);
    PUBLIC(this)->profilingUpdate.touch();

    clear_state = TRUE;
  }
} // doAction

void
SoProfilerStatsP::clearProfilingData(void)
{
  std::map<int16_t, SbProfilingData *>::iterator it, end;
  for (it = this->action_map.begin(), end = this->action_map.end();
       it != end; ++it) {
    delete (*it).second;
  }
  this->action_map.clear();

  this->action_timings.clear();

  this->type_timings.clear();
} // clearActionMap

void
SoProfilerStatsP::updateActionTimingMaps(SoProfilerElement * e, 
                                         SoAction * action)
{
  int16_t type = action->getTypeId().getKey();
  SbTime time = SbTime::zero();
  SbBool found = this->action_timings.get(type, time);
  if (found)
    time += e->timeSinceTraversalStart();
  else
    time = e->timeSinceTraversalStart();
  
  this->action_timings.put(type, time);
} // updateActionTimingMaps

void
SoProfilerStatsP::updateNodeTypeTimingMap(SoProfilerElement * e)
{
#if BY_TYPE
  SbList<int16_t> keys = e->accumulatedRenderTimeForTypeKeys();
  int keyCount = keys.getLength();
  for (int i = 0; i < keyCount; ++i) {
    int16_t k = keys[i];
    std::map<int16_t, TypeTimings>::iterator it = this->type_timings.find(k);
    if (it != this->type_timings.end()) {
      TypeTimings & timings = it->second;
      it->second.total += e->getAccumulatedRenderTimeForType(k);
      it->second.max += e->getMaxRenderTimeForType(k);
      it->second.count += e->getAccumulatedRenderCountForType(k);

    } else {
      TypeTimings timings;
      timings.total = e->getAccumulatedRenderTimeForType(k);
      timings.max = e->getMaxRenderTimeForType(k);
      timings.count = e->getAccumulatedRenderCountForType(k);
      std::pair<int16_t, TypeTimings> entry(k, timings);
      this->type_timings.insert(entry);
    }
  }
#else
  SbList<const char *> keys = e->accumulatedRenderTimeForNameKeys();
  int keyCount = keys.getLength();
  for (int i = 0; i < keyCount; ++i) {
    const char * k = keys[i];
    std::map<const char *, TypeTimings>::iterator it = this->name_timings.find(k);
    if (it != this->name_timings.end()) {
      TypeTimings & timings = it->second;
      it->second.total += e->getAccumulatedRenderTimeForName(k);
      it->second.max += e->getMaxRenderTimeForName(k);
      it->second.count += e->getAccumulatedRenderCountForName(k);
    } else {
      TypeTimings timings;
      timings.total = e->getAccumulatedRenderTimeForName(k);
      timings.max = e->getMaxRenderTimeForName(k);
      timings.count = e->getAccumulatedRenderCountForName(k);
      std::pair<const char *, TypeTimings> entry(k, timings);
      this->name_timings.insert(entry);
    }
  }
#endif
} // updateNodeTypeTimingMaps

void
SoProfilerStatsP::updateNodeTypeTimingFields()
{
#if BY_TYPE
  int keyCount = (int) this->type_timings.size();
#else
  int keyCount = (int) this->name_timings.size();
#endif

  PUBLIC(this)->renderedNodeType.setNum(keyCount);
  SbName * typeNameArr = PUBLIC(this)->renderedNodeType.startEditing();

  PUBLIC(this)->renderingTimePerNodeType.setNum(keyCount);
  SbTime * renderingTimePerNodeTypeArr = PUBLIC(this)->renderingTimePerNodeType.startEditing();

  PUBLIC(this)->renderingTimeMaxPerNodeType.setNum(keyCount);
  SbTime * renderingTimeMaxPerNodeTypeArr = PUBLIC(this)->renderingTimeMaxPerNodeType.startEditing();

  PUBLIC(this)->renderedNodeTypeCount.setNum(keyCount);
  uint32_t * nodeTypeCountArr = PUBLIC(this)->renderedNodeTypeCount.startEditing();

  int index = 0;
#if BY_TYPE
  std::map<int16_t, TypeTimings>::iterator it, end;
  for (it = this->type_timings.begin(), end = this->type_timings.end();
       it != end; ++it, ++index) {
    typeNameArr[index] = SoType::fromKey(it->first).getName();
#else
  std::map<const char *, TypeTimings>::iterator it, end;
  for (it = this->name_timings.begin(), end = this->name_timings.end();
       it != end; ++it, ++index) {
    typeNameArr[index] = SbName(it->first);
#endif
    renderingTimePerNodeTypeArr[index] = it->second.total;
    renderingTimeMaxPerNodeTypeArr[index] = it->second.max;
    nodeTypeCountArr[index] = it->second.count;
  }

  PUBLIC(this)->renderedNodeType.finishEditing();
  PUBLIC(this)->renderingTimePerNodeType.finishEditing();
  PUBLIC(this)->renderingTimeMaxPerNodeType.finishEditing();
  PUBLIC(this)->renderedNodeTypeCount.finishEditing();
} // updateNodeTypeTimingFields

void
SoProfilerStatsP::updateActionTimingFields(SoProfilerElement * e)
{
  SbList<int16_t> actions;
  this->action_timings.makeKeyList(actions);
  int actionCount = actions.getLength();

  PUBLIC(this)->profiledAction.setNum(actionCount);
  SbName * profiledActionArr = PUBLIC(this)->profiledAction.startEditing();

  PUBLIC(this)->profiledActionTime.setNum(actionCount);
  SbTime * profiledActionTimeArr = PUBLIC(this)->profiledActionTime.startEditing();

  for (int i = 0; i < actionCount; ++i) {
    int16_t type = actions[i];
    profiledActionArr[i] = SoType::fromKey(type).getName();
      
    SbTime time = SbTime::zero();
    this->action_timings.get(type, time);
    profiledActionTimeArr[i] = time;
  }

  PUBLIC(this)->profiledAction.finishEditing();
  PUBLIC(this)->profiledActionTime.finishEditing();
} // updateActionTimingFields


#undef PUBLIC


#define PRIVATE(obj) ((obj)->pimpl)

// *************************************************************************

SO_NODE_SOURCE(SoProfilerStats);

// *************************************************************************

// Doc from superclass.
void
SoProfilerStats::initClass(void)
{
  SO_NODE_INTERNAL_INIT_CLASS(SoProfilerStats, SO_FROM_COIN_3_0);

  SO_ENABLE(SoGLRenderAction, SoProfilerElement);
  SO_ENABLE(SoHandleEventAction, SoProfilerElement);
}

/*!
  Constructor
 */
SoProfilerStats::SoProfilerStats(void)
{
  PRIVATE(this)->master = this;

  SO_NODE_INTERNAL_CONSTRUCTOR(SoProfilerStats);
  
  SO_NODE_ADD_FIELD(renderedNodeType, (""));
  SO_NODE_ADD_FIELD(renderingTimePerNodeType, (0.0f));
  SO_NODE_ADD_FIELD(renderingTimeMaxPerNodeType, (0.0f));
  SO_NODE_ADD_FIELD(renderedNodeTypeCount, (0));
  SO_NODE_ADD_FIELD(profiledAction, (""));
  SO_NODE_ADD_FIELD(profiledActionTime, (0.0f));
  SO_NODE_ADD_FIELD(profilingUpdate, ());

  this->renderedNodeType.setNum(0);
  this->renderedNodeType.setDefault(TRUE);
  this->renderingTimePerNodeType.setNum(0);
  this->renderingTimePerNodeType.setDefault(TRUE);
  this->renderingTimeMaxPerNodeType.setNum(0);
  this->renderingTimeMaxPerNodeType.setDefault(TRUE);
  this->renderedNodeTypeCount.setNum(0);
  this->renderedNodeTypeCount.setDefault(TRUE);
  this->profiledAction.setNum(0);
  this->profiledAction.setDefault(TRUE);
  this->profiledActionTime.setNum(0);
  this->profiledActionTime.setDefault(TRUE);

}

/*!
  Destructor
 */
SoProfilerStats::~SoProfilerStats()
{
}

// *************************************************************************

/*!
  \brief 
 */
SbTime
SoProfilerStats::getProfilingTime(SoType action, SoNode * parent, 
                                  SoNode * child)
{
  std::map<int16_t, SbProfilingData *>::iterator it =
    PRIVATE(this)->action_map.find(action.getKey());
  if (it != PRIVATE(this)->action_map.end()) {
    return it->second->getChildTiming(parent, child);
  }
  return SbTime::zero();;
} // getProfilingTime

SbTime
SoProfilerStats::getTotalProfilingTime(SoNode * parent,
                                       SoNode * child)
{
  SbTime total(SbTime::zero());

  std::map<int16_t, SbProfilingData *>::iterator it, end;
  for (it = PRIVATE(this)->action_map.begin(),
         end = PRIVATE(this)->action_map.end();
       it != end; ++it) {
    total += it->second->getChildTiming(parent, child);
  }
  return total;
}

/*!
  \brief TODO
*/
SbBool
SoProfilerStats::hasGLCache(SoSeparator * sep) 
{
  int16_t key = SoGLRenderAction::getClassTypeId().getKey();
  std::map<int16_t, SbProfilingData *>::iterator it =
    PRIVATE(this)->action_map.find(key);
  if (it != PRIVATE(this)->action_map.end()) {
    return it->second->hasGLCache(sep);
  }
  return FALSE;
}

// *************************************************************************

// Doc from superclass.
void
SoProfilerStats::GLRender(SoGLRenderAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void
SoProfilerStats::handleEvent(SoHandleEventAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void
SoProfilerStats::rayPick(SoRayPickAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void
SoProfilerStats::getBoundingBox(SoGetBoundingBoxAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void
SoProfilerStats::audioRender(SoAudioRenderAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void
SoProfilerStats::callback(SoCallbackAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void 
SoProfilerStats::getMatrix(SoGetMatrixAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void 
SoProfilerStats::pick(SoPickAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void 
SoProfilerStats::search(SoSearchAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void 
SoProfilerStats::write(SoWriteAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void 
SoProfilerStats::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  PRIVATE(this)->doAction(action);
}

// Doc from superclass.
void
SoProfilerStats::notify(SoNotList *l)
{
  // we disable notifications from this node, to avoid constant
  // redraws of the scene graph.
  //
  // and it doesn't expose any state relevant for any other nodes that
  // do "real work" in the scenegraph anyway, so this is the correct
  // thing to do.
}

// *************************************************************************

#undef PRIVATE

#endif // HAVE_SCENE_PROFILING