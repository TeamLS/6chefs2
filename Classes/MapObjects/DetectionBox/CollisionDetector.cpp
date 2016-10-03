//
//  CollisionDetector.cpp
//  6chefs2
//
//  Created by Kohei Asami on 2016/09/11.
//
//

#include "MapObjects/DetectionBox/CollisionDetector.h"

#include "MapObjects/MapObject.h"

// コンストラクタ
CollisionDetector::CollisionDetector() { FUNCLOG }

// デストラクタ
CollisionDetector::~CollisionDetector()
{
    FUNCLOG
    
    _collistionBoxes.clear();
    _ignorableCollisionBoxes.clear();
}

// 初期化
bool CollisionDetector::init()
{
    if (!Node::init()) return false;
    
    return true;
}

CollisionBox* CollisionDetector::getIntersectsCollision(const CollisionBox* collision, const vector<Direction>& directions) const
{
    if (!collision) return nullptr;
    
    for (CollisionBox* other : _collistionBoxes) {
        if (collision->intersectsGrid(other, directions)) return other;
    }
    
    return nullptr;
}

CollisionBox* CollisionDetector::getIntersentsCollisionIncludeIngrable(const CollisionBox* collision, const vector<Direction>& directions) const
{
    if (!collision) return nullptr;
    
    for (CollisionBox* other : _collistionBoxes) {
        if (collision->intersectsGrid(other, directions)) return other;
    }
    
    for (CollisionBox* other : _ignorableCollisionBoxes) {
        if (collision->intersectsGrid(other, directions)) return other;
    }
    
    return nullptr;
}

#pragma mark -
#pragma mark Collection

void CollisionDetector::addCollision(CollisionBox* collision)
{
    if (!collision) return;
    
    _collistionBoxes.pushBack(collision);
}

void CollisionDetector::removeCollision(CollisionBox* collision)
{
    if (!collision) return;
    
    _collistionBoxes.eraseObject(collision);
}

void CollisionDetector::addIgnorableCollision(CollisionBox* collision)
{
    if (!collision) return;
    
    _ignorableCollisionBoxes.pushBack(collision);
}

void CollisionDetector::removeIgnorableCollision(CollisionBox* collision)
{
    if (!collision) return;
    
    _ignorableCollisionBoxes.eraseObject(collision);
}

#pragma mark -
#pragma mark Detection

bool CollisionDetector::intersects(const CollisionBox* collision) const
{
    if (!collision) return false;
    
    return this->getIntersentsCollisionIncludeIngrable(collision, {});
}

bool CollisionDetector::intersects(const MapObject* mapObject, const vector<Direction>& directions) const
{
    if (!mapObject->getCollision()) return false;
    
    return this->getIntersentsCollisionIncludeIngrable(mapObject->getCollision(), directions);
}

bool CollisionDetector::intersectsExceptIgnorable(const CollisionBox* collision) const
{
    if (!collision) return false;
    
    return this->getIntersectsCollision(collision, {});
}

bool CollisionDetector::intersectsExceptIgnorable(const MapObject* mapObject, const vector<Direction>& directions) const
{
    if (!mapObject->getCollision()) return false;
    
    return this->getIntersectsCollision(mapObject->getCollision(), directions);
}

bool CollisionDetector::existsCollisionBetween(const MapObject* obj1, const MapObject* obj2) const
{
    for (CollisionBox* collision : _ignorableCollisionBoxes) {
        if (collision->isBetween(obj1, obj2)) return true;
    }
    
    for (CollisionBox* collision : _collistionBoxes) {
        if (collision->isBetween(obj1, obj2)) return true;
    }
    
    return false;
}