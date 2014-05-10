#ifndef __GAMESTAGE_SCENE_H__
#define __GAMESTAGE_SCENE_H__

#include <vector>
#include "cocos2d.h"
#include "CCrazyMatrix.h"

USING_NS_CC;

class GameStageScene : public cocos2d::Layer
{
public:

	virtual bool init();  
	static cocos2d::Scene* createScene();
    CREATE_FUNC(GameStageScene);

private:

	Size winSize;
	std::vector<Sprite*> mTiles;
	std::vector<int> mTilesSelected;
	int lastActionTile;
	CCrazyMatrix matrix;
    StageInfo info;
	int mCurrentLevel;
	Layer* boardLayer;

	void gameStart(float dt);
	void getStageInfo();
	void drawInitBoard();
	void drawBoard();
	
	void showTiles(float dt);
	void hideTiles(float dt);
	void drawSolutionTiles(int n);
	void drawBoardTiles(int n);
	void addEventListener(EventDispatcher* e);
	void explosion(Point s);

	FiniteTimeAction* getActionByShuffleType(ShuffleType type);
};

#endif // __GAMESTAGE_SCENE_H__