//
//  SceneEvent.cpp
//  LastSupper
//
//  Created by Kohei Asami on 2015/10/25.
//
//

#include "Event/EventFactory.h"
#include "Event/SceneEvent.h"

#include "Event/GameEventHelper.h"
#include "Event/EventScriptMember.h"

#include "Datas/Scene/DungeonCameraSceneData.h"
#include "Datas/Scene/DungeonSceneData.h"

#include "MapObjects/MapObjectList.h"
#include "MapObjects/Character.h"
#include "MapObjects/Party.h"
#include "Managers/DungeonSceneManager.h"
#include "Models/StopWatch.h"

#include "Scenes/EndingScene.h"
#include "Scenes/DungeonCameraScene.h"
#include "Scenes/DungeonScene.h"
#include "Scenes/GameOverScene.h"
#include "Scenes/TitleScene.h"

#include "Utils/AssertUtils.h"

#pragma mark ChangeMapEvent

bool ChangeMapEvent::init(rapidjson::Value& json)
{
    if(!GameEvent::init()) return false;
    
    Direction direction { Direction::NONE };

    // directionの指定がされている時
    if(_eventHelper->hasMember(json, member::DIRECTION))
    {
        direction = _eventHelper->getDirection(json);
    }
    // directionが指定されていない時
    else
    {
        direction = DungeonSceneManager::getInstance()->getParty()->getMainCharacter()->getDirection();
    }
    
    this->destLocation = Location(stoi(json[member::MAP_ID].GetString()), json[member::X].GetInt(), json[member::Y].GetInt(), direction);
    this->currentLocation = DungeonSceneManager::getInstance()->getParty()->getMainCharacter()->getLocation();
    
    // 移動後に実行するイベントID
    if(_eventHelper->hasMember(json, member::EVENT_ID)) this->initEventId = stoi(json[member::EVENT_ID].GetString());
    
    return true;
}

void ChangeMapEvent::run()
{
    this->setDone();
    DungeonSceneManager::getInstance()->changeMap(this->destLocation, this->currentLocation, this->initEventId);
}

#pragma mark -
#pragma mark WaitEvent

bool WaitEvent::init(rapidjson::Value& json)
{
    if(!GameEvent::init()) return false;
    
    this->duration = {static_cast<float>(json[member::TIME].GetDouble())};;
    
    return true;
}

void WaitEvent::run() {}

void WaitEvent::update(float delta)
{
    this->duration -= delta;
    
    if(this->duration <= 0) this->setDone();
}

#pragma mark -
#pragma mark FadeOutEvent

bool FadeOutEvent::init(rapidjson::Value& json)
{
    if(!GameEvent::init()) return false;
    
    if(_eventHelper->hasMember(json, member::TIME)) this->duration = json[member::TIME].GetDouble();
    
    this->color = _eventHelper->getColor(json);
    
    return true;
}

void FadeOutEvent::run()
{
    DungeonSceneManager::getInstance()->fadeOut(this->color, this->duration, [this]{this->setDone();});
}

#pragma mark -
#pragma mark FadeInEvent

bool FadeInEvent::init(rapidjson::Value& json)
{
    if(!GameEvent::init()) return false;
    
    if(_eventHelper->hasMember(json, member::TIME)) this->duration = json[member::TIME].GetDouble();
    
    return true;
}

void FadeInEvent::run()
{
    DungeonSceneManager::getInstance()->fadeIn(this->duration, [this]{this->setDone();});
}

#pragma mark -
#pragma mark GameOverEvent

bool GameOverEvent::init(rapidjson::Value& json)
{
    if(!GameEvent::init()) return false;
    
    // ゲームオーバーのID
    if(_eventHelper->hasMember(json, member::ID)) this->gameOverId = stoi(json[member::ID].GetString());
    
    return true;
}

void GameOverEvent::run()
{
    this->setDone();
    DungeonSceneManager::getInstance()->exitDungeon(GameOverScene::create(static_cast<GameOverScene::Type>(this->gameOverId)));
}

#pragma mark -
#pragma mark EndingEvent

bool EndingEvent::init(rapidjson::Value& json)
{
    if(!GameEvent::init()) return false;
    
    // エンディングID
    if(_eventHelper->hasMember(json, member::ID)) this->endingId = stoi(json[member::ID].GetString());
    
    return true;
}

void EndingEvent::run()
{
    this->setDone();
    DungeonSceneManager::getInstance()->exitDungeon(EndingScene::create(this->endingId));
}

#pragma mark -
#pragma mark BackToTitleEvent

bool BackToTitleEvent::init(rapidjson::Value& json)
{
    if(!GameEvent::init()) return false;
    return true;
}

void BackToTitleEvent::run()
{
    this->setDone();
    DungeonSceneManager::getInstance()->exitDungeon(TitleScene::create());
}

#pragma mark -
#pragma mark InfoAssertEvent

bool InfoAssertEvent::init(rapidjson::Value& json)
{
    if (!GameEvent::init()) return false;
    this->text = (_eventHelper->hasMember(json, member::TEXT)) ? json[member::TEXT].GetString() : "";
    return true;
}

void InfoAssertEvent::run()
{
    this->setDone();
    LastSupper::AssertUtils::infoAssert(this->text);
}
