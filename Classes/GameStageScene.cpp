#include "GameStageScene.h"

USING_NS_CC;

#define IMAGE_TILE_NORAML "YellowSquare.png"
#define IMAGE_TILE_SELECTED "MagentaSquare.png"
#define SPEED_FOR_FLIP 0.05
#define SPEED_FOR_FLIP_DELAY 0.5

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
    if (!Layer::init()) {
        return false;
    }
    
	winSize = Director::getInstance()->getWinSize();
	mCurrentLevel = 3;
	gameStart(0);

    return true;
}

void GameStageScene::gameStart(float dt) {

	this->removeAllChildren();

	mTiles.clear();
	mTilesSelected.clear();

	getStageInfo();
    drawBoard();
	addEventListener(_eventDispatcher);
}

void GameStageScene::getStageInfo() {
    info = matrix.getStageInfo(mCurrentLevel);
}

void GameStageScene::drawBoard() {
	
	drawInitBoard();
	scheduleOnce(schedule_selector(GameStageScene::showTiles), 0.05);

	float delayTime = 3.0f * (float)info.matrixSize / 3.0f;
	scheduleOnce(schedule_selector(GameStageScene::hideTiles), delayTime);

	for (int t=0; t<info.actionNum; t++) {


		// FIX ME :
		auto delay = DelayTime::create(delayTime + 2.0 + t * 2.0);
		FiniteTimeAction* action;
		switch(info.actions[t]) {
		case TRANSFORM_FLIP_X:
			action = OrbitCamera::create(0.5, 1, 0, 0, 180, 0, 0);
			log("flip x");
			break;
		case TRANSFORM_FLIP_Y:
			action = OrbitCamera::create(0.5, 1, 0, 0, 180, 0, 0);
			log("flip y");
			break;
		case TRANSFORM_ROT_CW:
			action = RotateBy::create(0.5, 90);
			log("rotate 90");
			break;
		case TRANSFORM_ROT_CCW:
			action = RotateBy::create(0.5, -90);
			log("rotate -90");
			break;
		case TRANSFORM_TRANS1:
			action = RotateBy::create(0.5, 90);
			log("trans 1");
			break;
		case TRANSFORM_TRANS2:
			action = RotateBy::create(0.5, -90);
			log("trans 2");
			break;
		default:
			break;
		}
		//action = RotateBy::create(0.5, 90);
		
		// 나중에 겜 retry 할 때 효과^^
		//action = OrbitCamera::create(0.5, 1, 0, 0, 180, 0, 360);
		
		/**temp**/
		//boardLayer->runAction(Sequence::create(delay, action, NULL));
	}
	float actionsTime = delayTime + 1.0 * (float)info.actionNum;
}

FiniteTimeAction* GameStageScene::getActionByShuffleType(ShuffleType type) {

	// temp 
	log("wow");
	return RotateBy::create(1, 90);
}

void GameStageScene::drawInitBoard() {

	const int BOARD_SIZE = info.matrixSize;
	const int MARGIN_BOTTOM = winSize.height * 0.3;

	boardLayer = Layer::create();
	boardLayer->setPosition(0, MARGIN_BOTTOM);
	boardLayer->setContentSize(Size(winSize.width, winSize.width));
	boardLayer->setColor(Color3B(255, 0,0));

	for (int n=0; n<BOARD_SIZE; n++) {
		for (int m=0; m<BOARD_SIZE; m++) {

			int idx = n * BOARD_SIZE + m;

			auto tile = Sprite::create(IMAGE_TILE_NORAML);
			tile->setPosition(winSize.width/(BOARD_SIZE+1)*(m+1), winSize.width/(BOARD_SIZE+1)*(n+1));// + MARGIN_BOTTOM);
			float tileScale = 3.0f / (float)info.matrixSize;
			tile->setScale(tileScale);
			tile->setTag(idx);
			boardLayer->addChild(tile);

			mTiles.push_back(tile);
			mTilesSelected.push_back(0);
		}
	}

	this->addChild(boardLayer);
}

void GameStageScene::showTiles(float dt) {
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
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [](Touch* touch, Event* event){
        
        auto target = event->getCurrentTarget();
        Point locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);
        
        if (rect.containsPoint(locationInNode)) {
            //log("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y);
            return true;
        }
        return false;
    };
    
    listener->onTouchMoved = [](Touch* touch, Event* event){
        // nothing..
    };
    
    listener->onTouchEnded = [=](Touch* touch, Event* event){
        auto target = event->getCurrentTarget();
		int tileNum = target->getTag();

		mTilesSelected[tileNum] = (mTilesSelected[tileNum] + 1) % 2;
		std::string image = mTilesSelected[tileNum] == 0 ? IMAGE_TILE_NORAML : IMAGE_TILE_SELECTED;
		mTiles[tileNum]->setTexture(image);

		int count = 0;
		for (int i=0; i<mTiles.size(); i++) {
			if (info.before[i] == mTilesSelected.at(i)) {
				count++;
			}
		}
        if (count == mTilesSelected.size()) {
			log("stage clear!");
			explosion(Point(winSize.width/2, winSize.height * 0.9));
			mCurrentLevel += 1;
			
			scheduleOnce(schedule_selector(GameStageScene::gameStart), 3);
			
		}
    };
    
	for (int n=0; n<mTiles.size(); n++) {
		e->addEventListenerWithSceneGraphPriority(listener->clone(), mTiles.at(n));
	}
 
    
}

void GameStageScene::explosion(Point s) {
	ParticleSystem *particle = ParticleExplosion::createWithTotalParticles(5000);
	particle->setTexture(Director::getInstance()->getTextureCache()->addImage("fire.png"));
	particle->setPosition(s);
	particle->setGravity(Point(0, -100));
	//particle->setSpeed(3);
	log("%f", particle->getSpeed());
	particle->setEndSize(0.0);
	this->addChild(particle,100);
}