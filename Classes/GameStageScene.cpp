#include "GameStageScene.h"
#include "SimpleAudioEngine.h"
#include "StageClearScene.h"
USING_NS_CC;

#define IMAGE_TILE_NORAML "YELLOW.png"
#define IMAGE_TILE_SELECTED "MAGENTA.png"
#define IMAGE_BG_CURRENT_STAGE "GREEN_CIRCLE.png"
#define SPEED_FOR_FLIP 0.05
#define SPEED_FOR_FLIP_DELAY 0.3
#define MSG_PLAY_AGAIN "play-again"
#define TAG_BUTTON_CURRENT_STAGE_BG 500

Scene* GameStageScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameStageScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


bool GameStageScene::init()
{
    if (!LayerColor::initWithColor(Color4B(120,180,180,255))) {
        return false;
    }
    
    winSize = Director::getInstance()->getWinSize();
	
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("firework.mp3");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("es042.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("ticktock.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("pageFlip.mp3");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("applause.mp3");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("gameFail.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("stageBtClick.wav");
    
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("bgMusic.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("bgMusic.wav", true);
    
	mCurrentLevel = 1;
    tileTouchEnable = false;
	gameStart(0);

    return true;
}

void GameStageScene::gameStart(float dt) {

	this->removeAllChildren();

    bgImage = Sprite::create("stage_bg.png");
    bgImage->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(bgImage);
    
	mTiles.clear();
	mTilesSelected.clear();
	getStageInfo();
    drawBoard();
    drawCurrentStageInfo();
	addEventListener(_eventDispatcher);
    
}

void GameStageScene::drawCurrentStageInfo() {
    
    bgCurrentStage = Sprite::create(IMAGE_BG_CURRENT_STAGE);
    bgCurrentStage->setPosition(Point(winSize.width/2, winSize.height * 0.2));
    this->addChild(bgCurrentStage);
    
    char stageInfo[3];
    std::sprintf(stageInfo, "%d", mCurrentLevel);
    currentStage = LabelTTF::create(stageInfo, "arial", 80);
    currentStage->setPosition(Point(winSize.width/2, winSize.height * 0.2));
    bgCurrentStage->setTag(TAG_BUTTON_CURRENT_STAGE_BG);
    this->addChild(currentStage);
    
}

void GameStageScene::getStageInfo() {
    info = matrix.getStageInfo(mCurrentLevel);
    log("action Num: %d, level: %d, matrixSize: %d",info.actionNum, info.level, info.matrixSize);
}

void GameStageScene::drawBoard() {
	
	drawInitBoard();
	scheduleOnce(schedule_selector(GameStageScene::showTiles), 0.05);

	float delayTime = 2.0f * (float)info.matrixSize / 3.0f;
    float hideTime = 1.0f * (float)info.matrixSize / 3.0f;
    const float delayForOneAnimation = 0.8;
    const float animationTime = delayForOneAnimation + 0.2f * info.actionNum;
    
	scheduleOnce(schedule_selector(GameStageScene::hideTiles), delayTime);
    scheduleOnce(schedule_selector(GameStageScene::makeTimer), delayTime + animationTime + hideTime);
    
    FiniteTimeAction* action1 = NULL;
    FiniteTimeAction* action2 = NULL;
    FiniteTimeAction* action3 = NULL;
    
    auto delay_interval = DelayTime::create(0.1);
    
	for (int t=0; t<info.actionNum; t++) {

        ActionInterval* action;
        
		switch(info.actions[t]) {
		case TRANSFORM_FLIP_X:
			action = OrbitCamera::create(delayForOneAnimation, 1, 0, 0, 180, 0, 0);
            //action = FlipX3D::create(0.5);
            log("flip x");
			break;
		case TRANSFORM_FLIP_Y:
			action = OrbitCamera::create(delayForOneAnimation, 1, 0, 0, 180, 0, 0);
            //action = FlipY3D::create(0.5);
            log("flip y");
			break;
		case TRANSFORM_ROT_CW:
			action = RotateBy::create(delayForOneAnimation, 90);
			log("rotate 90");
			break;
		case TRANSFORM_ROT_CCW:
			action = RotateBy::create(delayForOneAnimation, -90);
			log("rotate -90");
			break;
		case TRANSFORM_TRANS1:
			action = RotateBy::create(delayForOneAnimation, 90);
			log("trans 1");
			break;
		case TRANSFORM_TRANS2:
			action = RotateBy::create(delayForOneAnimation, -90);
			log("trans 2");
			break;
		default:
			break;
		}
        
        //action = EaseExponentialIn::create(action);
        action = EaseBackOut::create(action);
        
        if (t==0) {
            action1 = action;
        } else if(t==1) {
            action2 = action;
        } else if(t==2) {
            action3 = action;
        }
	}
    
    auto delay = DelayTime::create(delayTime + hideTime);
    boardLayer->runAction(Sequence::create(delay, action1, delay_interval, action2, delay_interval, action3, NULL));
}


void GameStageScene::drawInitBoard() {

	const int BOARD_SIZE = info.matrixSize;
	
    int MARGIN_BOTTOM;
    if (winSize.height > 1000) {
        // 3.5 inch
        MARGIN_BOTTOM = winSize.height * 0.3;
    } else {
        // 4 inch
        MARGIN_BOTTOM = winSize.height * 0.2;
    }
    
    //nodeGrid = NodeGrid::create();
    
	boardLayer = Layer::create();
	boardLayer->setPosition(0, MARGIN_BOTTOM);
    boardLayer->setContentSize(Size(winSize.width, winSize.width));
	boardLayer->setColor(Color3B(255, 0,0));

	for (int n=0; n<BOARD_SIZE; n++) {
		for (int m=0; m<BOARD_SIZE; m++) {

			int idx = n * BOARD_SIZE + m;

			auto tile = Sprite::create(IMAGE_TILE_NORAML);
			tile->setPosition(winSize.width/(BOARD_SIZE+1)*(m+1), winSize.width/(BOARD_SIZE+1)*(n+1));
            float tileScale = 4.0f / (float)info.matrixSize;
			tile->setScale(tileScale);
			tile->setTag(idx);
			boardLayer->addChild(tile,10);
            
			mTiles.push_back(tile);
			mTilesSelected.push_back(0);
		}
	}

	this->addChild(boardLayer);
}

void GameStageScene::showTiles(float dt) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pageFlip.mp3");
	for (int n=0; n<mTiles.size(); n++) {

		float delayTime = n * SPEED_FOR_FLIP;
		float flipTime = SPEED_FOR_FLIP_DELAY;

		auto delay1 = DelayTime::create(delayTime);
		auto delay2 = DelayTime::create(flipTime / 2);

		auto flipActions = Sequence::create(delay1, OrbitCamera::create(flipTime, 1, 0, 0, -180, 0, 0), NULL);
		auto changeTile = CallFunc::create( CC_CALLBACK_0(GameStageScene::drawSolutionTiles, this, n) );

		mTiles.at(n)->runAction(flipActions);
		if (info.before[n] == 1) {
			mTiles.at(n)->runAction(Sequence::create(delay1, delay2, changeTile, NULL));
		}	
	}
}

void GameStageScene::hideTiles(float dt) {
	for (int n=0; n<mTiles.size(); n++) {

		float delayTime = n * SPEED_FOR_FLIP;
		float flipTime = SPEED_FOR_FLIP_DELAY;


		auto delay1 = DelayTime::create(delayTime);
		auto delay2 = DelayTime::create(flipTime / 2);

		auto flipActions = Sequence::create(delay1, OrbitCamera::create(flipTime, 1, 0, 0, 180, 0, 0), NULL);
		auto changeTile = CallFunc::create( CC_CALLBACK_0(GameStageScene::drawBoardTiles, this, n) );

		mTiles.at(n)->runAction(flipActions);
		if (info.before[n] == 1) {
			mTiles.at(n)->runAction(Sequence::create(delay1, delay2, changeTile, NULL));	
		}
	}
}

void GameStageScene::drawSolutionTiles(int n) {
	mTiles.at(n)->setTexture(IMAGE_TILE_SELECTED);
}

void GameStageScene::drawBoardTiles(int n) {
	mTiles.at(n)->setTexture(IMAGE_TILE_NORAML);
}

void GameStageScene::addEventListener(EventDispatcher* e) {

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    
    listener->onTouchBegan = [=](Touch* touch, Event* event){
        
        auto target = event->getCurrentTarget();
        int tagNum = target->getTag();
        
        Point locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        const float MARGIN_TOUCH_AREA = target->getContentSize().width * 0.1;
        Rect rect = Rect(-MARGIN_TOUCH_AREA, -MARGIN_TOUCH_AREA, s.width + MARGIN_TOUCH_AREA, s.height + MARGIN_TOUCH_AREA);
        
        if (rect.containsPoint(locationInNode)) {
            
            if (tagNum > mTiles.size()) {
                switch (tagNum) {
                    case TAG_BUTTON_CURRENT_STAGE_BG:
                        //auto scaleInitup = ScaleBy::create(0.2f, 1.3f);
                        auto scaledown = ScaleBy::create(0.1f, 0.9f);
                        auto scaleup = ScaleBy::create(0.1f, 1.1f);
                        auto scaleupdowon = RepeatForever::create(Sequence::create(scaleup, scaledown, NULL));
                        bgCurrentStage->setScale(1.3f);
                        bgCurrentStage->runAction(scaleupdowon);
                        break;
                }
                return true;
            } else if (tileTouchEnable == true) {
                float currentScale = mTiles[tagNum]->getScale();
                mTiles[tagNum]->setScale(currentScale * 1.1);
                return true;
            }
            
        }
        return false;
    };
    
    listener->onTouchMoved = [](Touch* touch, Event* event){
        // nothing..
    };
    
    listener->onTouchEnded = [=](Touch* touch, Event* event){
        auto target = event->getCurrentTarget();
		int tagNum = target->getTag();
 
        if (tagNum > mTiles.size()) {
            switch (tagNum) {
                case TAG_BUTTON_CURRENT_STAGE_BG:
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("stageBtClick.wav");
                    bgCurrentStage->stopAllActions();
                    bgCurrentStage->setScale(1.0f);
                    break;
                default:
                    break;
            }
            return;
        }
        
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("es042.wav");
        
		mTilesSelected[tagNum] = (mTilesSelected[tagNum] + 1) % 2;
		std::string image = mTilesSelected[tagNum] == 0 ? IMAGE_TILE_NORAML : IMAGE_TILE_SELECTED;
		mTiles[tagNum]->setTexture(image);
        float currentScale = mTiles[tagNum]->getScale();
        mTiles[tagNum]->setScale(currentScale - currentScale * 0.1);
        flower(touch->getLocation());
        
		int count = 0;
		for (int i=0; i<mTiles.size(); i++) {
			if (info.before[i] == mTilesSelected.at(i)) {
				count++;
			}
		}
        if (count == mTilesSelected.size()) {
			log("stage clear!");
            stageClear();
		}
    };
    
    e->addEventListenerWithSceneGraphPriority(listener->clone(), bgCurrentStage);
    e->addEventListenerWithSceneGraphPriority(listener->clone(), currentStage);
    
	for (int n=0; n<mTiles.size(); n++) {
		e->addEventListenerWithSceneGraphPriority(listener->clone(), mTiles.at(n));
	}
    
}

void GameStageScene::stageClear() {
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("applause.mp3");
    unschedule(schedule_selector(GameStageScene::drawTimerLabel));
    tileTouchEnable = false;
    mCurrentLevel += 1;
    this->removeChild(timerLabel);
    
    auto scaleUp = ScaleBy::create(0.4, 1.5);
    auto scaleDown = scaleUp->reverse();
    bgCurrentStage->runAction(Sequence::create(scaleUp, scaleDown, scaleUp, scaleDown, scaleUp, scaleDown, NULL));
    
    auto delay1_exp = DelayTime::create(0.2);
    auto delay2_exp = DelayTime::create(0.5);
    auto exp1 = CallFunc::create( CC_CALLBACK_0(GameStageScene::explosion, this, Point(128,672)) );
    auto exp2 = CallFunc::create( CC_CALLBACK_0(GameStageScene::explosion, this, Point(530,622)) );
    auto exp3 = CallFunc::create( CC_CALLBACK_0(GameStageScene::explosion, this, Point(350,912)) );
    this->runAction(Sequence::create(exp1, delay1_exp, exp2, delay2_exp, exp3, NULL));
    
    scheduleOnce(schedule_selector(GameStageScene::gameStart), 3);
}

void GameStageScene::explosion(Point s) {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("firework.mp3");
	ParticleSystem *particle = ParticleExplosion::createWithTotalParticles(150);
	particle->setTexture(Director::getInstance()->getTextureCache()->addImage("fire.png"));
	particle->setPosition(s);
	particle->setGravity(Point(0, -70));
    particle->setLife(1.8);
    particle->setSpeed(120);
    particle->setEmissionRate(1300);
	particle->setEndColor(Color4F(0,0,0,1));
    particle->setEndSize(0.0);
    particle->setStartColor(Color4F(255, 0, 0, 255));
	
    this->addChild(particle,100);
}

void GameStageScene::flower(Point s)
{
    ParticleSystem *emitter = ParticleFlower::create();
    emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));
    emitter->setPosition(s);
    emitter->setLife(0.1);
    emitter->setDuration(0.5);
    emitter->setEmissionRate(100);
    emitter->setSpeed(100);
    
    this->addChild(emitter,101);
}

void GameStageScene::effectShowSolution(Point s)
{
    ParticleSystem *emitter = ParticleFlower::create();
    emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("stars.png"));
    emitter->setPosition(s);
    boardLayer->addChild(emitter,0);
    
}

void GameStageScene::makeTimer(float dt) {

    timerLabel = LabelTTF::create("Ready!", "arial", 90.0f);
    timerLabel->setPosition(Point(winSize.width/2, winSize.height * 0.9));
    timerLabel->runAction(ScaleBy::create(0.3, 1.5f));
    this->addChild(timerLabel);

    timerCount = info.TimeLimit;
    schedule(schedule_selector(GameStageScene::drawTimerLabel), 1.0f);
    
}

void GameStageScene::drawTimerLabel(float dt) {
    
    if (timerCount < 0) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gameFail.wav");
        unschedule(schedule_selector(GameStageScene::drawTimerLabel));
        mCurrentLevel = 1;
        timerLabel->setString("Game Over");
        timerLabel->setScale(1.0f);
        timerLabel->runAction(ScaleBy::create(0.5, 1.2f));
        tileTouchEnable = false;
        
        for (int i=0; i<mTiles.size(); i++) {
            Point pt;
            if (info.before[i] == 1) {
                pt = mTiles.at(i)->getPosition();
                effectShowSolution(pt);
            }
        }
        
        scheduleOnce(schedule_selector(GameStageScene::gameStart), 3.0f);
        
    } else {
        if (timerCount < 3) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ticktock.wav");
            auto jump = JumpBy::create(0.25, Point(0, 0), 20, 1);
            boardLayer->runAction(Sequence::create(jump, jump, NULL));
            
        }
        char buffer[2];
        std::sprintf(buffer, "%d", timerCount--);
        timerLabel->setScale(1.0f);
        timerLabel->setString(buffer);
        timerLabel->runAction(ScaleBy::create(0.5, 1.5f));
        tileTouchEnable = true;
    }
}

