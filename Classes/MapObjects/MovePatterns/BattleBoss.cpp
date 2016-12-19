//
//  BattleBoss.cpp
//  6chefs2
//
//  Created by Kohei Asami on 2016/12/11.
//
//

#include "MapObjects/MovePatterns/BattleBoss.h"

#include "MapObjects/Character.h"
#include "MapObjects/Command/AttackCommand.h"
#include "MapObjects/DetectionBox/CollisionBox.h"
#include "MapObjects/MovePatterns/Chaser.h"

// 定数
const string BattleBoss::FORWARD_ATTACK_NAME { "attack_1" };
const string BattleBoss::SPIN_ATTACK_NAME { "attack_2" };
const string BattleBoss::FLASH_ATTACK_NAME { "attack_3" };

// コンストラクタ
BattleBoss::BattleBoss() { FUNCLOG }

// デストラクタ
BattleBoss::~BattleBoss() { FUNCLOG }

// 初期化
bool BattleBoss::init(Character* character)
{
    if (!MovePattern::init(character)) return false;
    
    _subPattern = Chaser::create(character);
    CC_SAFE_RETAIN(_subPattern);
    _subPattern->setSpeedRatio(0.4f);
    
    return true;
}

#pragma mark -
#pragma mark Interface
void BattleBoss::start()
{
    MovePattern::start();
    if (_chara) _chara->setDirection(Direction::convertVec2(this->getMainCharacter()->getPosition() - _chara->getPosition()));
    if (_subPattern) _subPattern->start();
}

void BattleBoss::pause()
{
    MovePattern::pause();
    if (_subPattern) _subPattern->pause();
}

void BattleBoss::resume()
{
    MovePattern::resume();
    if (_subPattern) _subPattern->resume();
}

void BattleBoss::onPartyMoved()
{
    if (_subPattern) _subPattern->onPartyMoved();
}

bool BattleBoss::canGoToNextMap() const
{
    return false;
}

void BattleBoss::update(float delta)
{
    if (_paused) return;
    if (!_chara) return;
    if (_chara->isInAttackMotion()) return;
    if (!_chara->getBattle()) return;
    
    Point forwardGridPos1 { _chara->getGridPosition() + _chara->getDirection().getGridVec2() };
    Point forwardGridPos2 { _chara->getGridPosition() + _chara->getDirection().getGridVec2() * 2 };
    
    if (_chara->getCollision()->intersectsGrid(this->getMainCharacter()->getCollision(), forwardGridPos1) ||
        _chara->getCollision()->intersectsGrid(this->getMainCharacter()->getCollision(), forwardGridPos2)) {
        _chara->clearCommandQueue();
        AttackCommand* command { AttackCommand::create() };
        command->setName(FORWARD_ATTACK_NAME);
        command->setCallback([this](Character* character){ this->start(); });
        _chara->pushCommand(command);
    }
}
