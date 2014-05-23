//
//  StageClearScene.h
//  Crz Matrix
//
//  Created by 정 도원 on 2014. 5. 12..
//
//

#ifndef __Crz_Matrix__StageClearScene__
#define __Crz_Matrix__StageClearScene__

#include "cocos2d.h"

USING_NS_CC;

class StageClearScene : public cocos2d::Layer {

public:
    
	virtual bool init();
	static cocos2d::Scene* createScene();
    CREATE_FUNC(StageClearScene);

private:
    
    Size winSize;
    
    void explosionEffect(Point s);
    void playNextStage();
};

#endif /* defined(__Crz_Matrix__StageClearScene__) */
