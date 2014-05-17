#include "CCrazyMatrix.h"

int g_designedNumShuffles[MAX_LEVEL] = {0, 1, 1, 1, 1, 1, 1, 2, 1, 1,
                                        0, 0, 1, 1, 1, 2, 2, 2, 2, 3};
int g_designedNumPairs[MAX_LEVEL] =    {2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                        6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int g_designedNumSides[MAX_LEVEL] =    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                                        5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
int g_designedTimeLimit[MAX_LEVEL] =   {5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                        5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

CCrazyMatrix::CCrazyMatrix()
{
    
}

CCrazyMatrix::CCrazyMatrix(int _numPairs, int _numSides, int _numShuffles, int _level)
{
    if (_numPairs > (_numSides*_numSides)/2 )
        numPairs = (_numSides*_numSides)/2;
    else
        numPairs = _numPairs;
    numSides = _numSides;

	if (_numShuffles > MAX_SHUFFLES)
		numShuffles = MAX_SHUFFLES;
	else
		numShuffles = _numShuffles;

	level = _level;
	setPositionLight();
}

CCrazyMatrix::CCrazyMatrix(int _level)
{
	if (_level > MAX_LEVEL) {
		level = MAX_LEVEL;
	} else {
		level = _level;
	}
	levelDesign();
	setPositionLight();
}

StageInfo CCrazyMatrix::getStageInfo(int _level)
{
	if (_level > MAX_LEVEL) {
		level = MAX_LEVEL;
	} else {
		level = _level;
	}

	levelDesign();
	setPositionLight();

	StageInfo info;
		
	info.matrixSize = numSides;
	info.level = level;
	info.actionNum = numShuffles;
    info.solutionNum = numPairs;
    info.TimeLimit = timeLimit;

	info.before = new int[numSides*numSides];
	info.after = new int[numSides*numSides];
	memset(info.before, 0, sizeof(int)*numSides*numSides);
	memset(info.after, 0, sizeof(int)*numSides*numSides);

	int idx;
	for (int i=0; i<numSides; i++) {
		for (int j=0; j<numSides; j++) {
			idx = i*numSides + j;
			info.before[idx] = boardOriginal[i][j];
			info.after[idx] = board[i][j];
		}
	}
	
	for (int i=0; i<numShuffles; i++) {
		info.actions[i] = (ShuffleType)gameSchedule[i];
		//printf("%d %d\n",info.actions[i], gameSchedule[i]);
	}

	return info;
}

void CCrazyMatrix::levelDesign()
{
	numPairs =  g_designedNumPairs[level-1];
	numShuffles = g_designedNumShuffles[level-1];
	numSides = g_designedNumSides[level-1];
    timeLimit = g_designedTimeLimit[level-1];
}

CCrazyMatrix::~CCrazyMatrix()
{
    for (int i=0; i<numSides; i++) {
        delete[] board[i];
        delete[] boardCheck[i];
		delete[] boardOriginal[i];
    }

    delete[] board;
    delete[] boardCheck;
	delete[] boardOriginal;

    delete[] orderSolution;
    delete[] idxX;
    delete[] idxY;
}

void CCrazyMatrix::boardCopy()
{
    for (int i=0; i<numSides; i++) {
        for (int j=0; j<numSides; j++) {
            boardCheck[i][j] = board[i][j];
		}
	}
}

void CCrazyMatrix::setPositionLight()
{
    numTries = numCorrects = numErrors = 0;

    board = new int*[numSides];
	boardOriginal = new int*[numSides];
    for (int i=0; i<numSides; i++) {
        board[i] = new int[numSides];
		boardOriginal[i] = new int[numSides];
        memset(board[i], 0, sizeof(int)*numSides);
		memset(boardOriginal[i], 0, sizeof(int)*numSides);
    }

    boardCheck = new int*[numSides];
    for (int i=0; i<numSides; i++) {
        boardCheck[i] = new int[numSides];
        memset(boardCheck[i], 0, sizeof(int)*numSides);
    }

    orderSolution = new int[numPairs];
    idxX = new int[numPairs];
    idxY = new int[numPairs];
    memset(orderSolution, 0, sizeof(int)*numPairs);
    memset(idxX, 0, sizeof(int)*numPairs);
    memset(idxY, 0, sizeof(int)*numPairs);

    lightGameDesign();
	lightGameShuffle();
}

void CCrazyMatrix::setPosition()
{
    numTries = numCorrects = numErrors = 0;

    board = new int*[numSides];
    for (int i=0; i<numSides; i++) {
        board[i] = new int[numSides];
        memset(board[i], 0, sizeof(int)*numSides);
    }

    orderSolution = new int[numPairs*2];
    idxX = new int[numPairs*2];
    idxY = new int[numPairs*2];
    memset(orderSolution, 0, sizeof(int)*numPairs*2);
    memset(idxX, 0, sizeof(int)*numPairs*2);
    memset(idxY, 0, sizeof(int)*numPairs*2);

    gameDesign();
}

void CCrazyMatrix::lightGameDesign()
{
    srand((unsigned int)time(NULL));
    orderSolution[0] = rand()%(numSides*numSides);

    int count = 1;
    while (count < numPairs) {
        int mFlag = 1;
        int number = 0;
        number = rand()%(numSides*numSides);
        for (int i=0; i<count; i++)
            if (orderSolution[i] == number) mFlag = 0;
        if(mFlag) {
            orderSolution[count] = number;
            count++;
        }
    }

    for (int i=0; i<numPairs; i++) {
        idxX[i] = orderSolution[i]/numSides;
        idxY[i] = orderSolution[i]%numSides;
    }

    for (int i=0; i<numPairs; i++) {
        board[idxX[i]][idxY[i]] = 1;
		boardOriginal[idxX[i]][idxY[i]] = 1;
    }
    boardCopy();
	//printf("Initial Game Board\n");
	//Print();
}

void CCrazyMatrix::lightGameShuffle()
{
	srand((unsigned int)time(NULL));
    
    gameSchedule[0] = rand() % TRANSFORM_NUM_TYPE;
    
    if (level < TARGET_LEVEL) {
        while (gameSchedule[0] < 2) {
            gameSchedule[0] = rand() % TRANSFORM_NUM_TYPE;
        }
    }

	int temp;

	for (int i=1; i<numShuffles; i++) {
		temp = rand() % TRANSFORM_NUM_TYPE;
		if (level < TARGET_LEVEL) {
            while ((temp == gameSchedule[i-1]) || (temp < 2)) {
                temp = rand() % TRANSFORM_NUM_TYPE;
            }
        }
        
        else {
            while (temp == gameSchedule[i-1]) {
                temp = rand() % TRANSFORM_NUM_TYPE;
            }
        }
		gameSchedule[i] = temp;
	}

	for (int i=0; i<numShuffles; i++) {
		switch(gameSchedule[i]) {
			case TRANSFORM_FLIP_X:
				transformFlipX();
				//printf("FLIP_X\n");
				//Print();
				break;
			case TRANSFORM_FLIP_Y:
				transformFlipY();
				//printf("FLIP_Y\n");
				//Print();
				break;
			case TRANSFORM_ROT_CW:
				transformRotCW();
				//printf("ROT_CW\n");
				//Print();
				break;
			case TRANSFORM_ROT_CCW:
				transformRotCCW();
				//printf("ROT_CCW\n");
				//Print();
				break;
			case TRANSFORM_TRANS1:
				transformTrans1();
				//printf("TRNAS1\n");
				//Print();
				break;
			case TRANSFORM_TRANS2:
				transformTrans2();
				//printf("TRNAS2\n");
				//Print();
				break;
			default:
				break;
		}
	}
}

void CCrazyMatrix::gameDesign()
{
    srand((unsigned int)time(NULL));
    orderSolution[0] = rand()%(numSides*numSides);

    int count = 1;
    while (count < numPairs*2) {
        int mFlag = 1;
        int number = 0;
        number = rand()%(numSides*numSides);
        for (int i=0; i<count; i++)
            if (orderSolution[i] == number) mFlag = 0;
        if(mFlag) {
            orderSolution[count] = number;
            count++;
        }
    }

    for (int i=0; i<numPairs*2; i++) {
        idxX[i] = orderSolution[i]/numSides;
        idxY[i] = orderSolution[i]%numSides;
    }

    for (int i=0; i<numPairs*2; i++) {
        board[idxX[i]][idxY[i]] = 1;
    }
}

int CCrazyMatrix::submitAnswerLight(int idx)
{
    int submitX, submitY;
    int flag = 0;

    submitX = (idx-1)/numSides;
    submitY = (idx-1)%numSides;

    numTries += 1;

    int boardAns = board[submitX][submitY];
    int boardCheckAns = boardCheck[submitX][submitY];

    if (boardAns != 0 && boardCheckAns != 0) {
        numCorrects += 1;
        boardCheck[submitX][submitY] = 0;
    }

    if (boardAns != 0 && boardCheckAns == 0) {
        numTries -= 1;
    }

    if (boardAns == 0 && boardCheckAns == 0) {
        numErrors += 1;
    }

    if (numPairs == numCorrects)
        flag = 1;

    return flag;
}

void CCrazyMatrix::submitAnswerLight(int *_submit, int *_result, int *_status)
{
	int idx;
	for (int i=0; i<numSides; i++) {
		for (int j=0; j<numSides; j++) {
			idx = i*numSides + j;
			boardCheck[i][j] = _submit[idx];
			// 1 on 0 off
			if (_submit[idx] == 1) 
				_status[idx] = (_status[idx] + 1)%2;
		}
	}

	for (int i=0; i<numSides; i++) {
		for (int j=0; j<numSides; j++) {
			idx = i*numSides + j;
			if ( (board[i][j] == boardCheck[i][j]) && (board[i][j] == 1))
				_result[idx] = 1;
			else
				_result[idx] = 0;
		}
	}

	int sum = 0;
	for (int i=0; i<numSides*numSides; i++) {
		sum = sum + _submit[i] * _result[i];
	}
	numCorrects = sum/numPairs;
	if (numCorrects != numPairs)
		numErrors += 1;
	else
		numCorrects += 1;
	numTries += 1;
}

int CCrazyMatrix::submitAnswer(int idx1, int idx2)
{
    int submitX[2];
    int submitY[2];
    int flag = 0;

    submitX[0] = (idx1-1)/numSides;
    submitY[0] = (idx1-1)%numSides;
    submitX[1] = (idx2-1)/numSides;
    submitY[1] = (idx2-1)%numSides;

    numTries += 1;

    int boardAns1 = board[submitX[0]][submitY[0]];
    int boardAns2 = board[submitX[1]][submitY[1]];

    if ((boardAns1 == boardAns2) && (boardAns1 != 0) && (boardAns2 != 0) ) {
        numCorrects += 1;
    } else {
        numErrors += 1;
    }

    if (numPairs == numCorrects)
        flag = 1;

    return flag;
}


int CCrazyMatrix::getNumTries()
{
    return numTries;
}

int CCrazyMatrix::getNumCorrects()
{
    return numCorrects;
}

int CCrazyMatrix::getNumErrors()
{
    return numErrors;
}

void CCrazyMatrix::getBoard(int *_board)
{
	int idx;
	for (int i=0; i<numSides; i++) {
		for (int j=0; j<numSides; j++) {
			idx = i*numSides + j;
			_board[idx] = board[i][j];
		}
	}
}

void CCrazyMatrix::getGameSchedule(int *_gameSchedule)
{
	for (int i=0; i<MAX_SHUFFLES; i++) {
		_gameSchedule[i] = gameSchedule[i];
	}
}

void CCrazyMatrix::transformFlipX()
{
    int n = numSides;
    for (int i=0; i<n/2; i++) {
        for (int j=0; j<n; j++) {
            int saved = board[i][j];
            board[i][j] = board[n-1-i][j];
            board[n-1-i][j] = saved;
        }
    }
    boardCopy();
}

void CCrazyMatrix::transformFlipY()
{
    int n = numSides;
    for (int i=0; i<n; i++) {
        for (int j=0; j<n/2; j++) {
            int saved = board[i][j];
            board[i][j] = board[i][n-1-j];
            board[i][n-1-j] = saved;
        }
    }
    boardCopy();
}

void CCrazyMatrix::transformTrans1()
{
    int n = numSides;
    for (int i=0; i<n; i++) {
        for (int j=0; j<i; j++) {
            int saved = board[i][j];
            board[i][j] = board[j][i];
            board[j][i] = saved;
        }
    }
    boardCopy();
}

void CCrazyMatrix::transformTrans2()
{
    int n = numSides;
    for (int i=0; i<n-1; i++) {
        for (int j=0; j<n-i-1; j++) {
            int saved = board[i][j];
            board[i][j] = board[n-1-j][n-1-i];
            board[n-1-j][n-1-i] = saved;
        }
    }
    boardCopy();
}

void CCrazyMatrix::transformRotCW()
{
    int n = numSides;
    for (int i=0; i<n/2; i++) {
        for (int j=i; j<n-i-1; j++) {
            int saved = board[i][j];
            board[i][j] = board[n-j-1][i];
            board[n-j-1][i] = board[n-1-i][n-1-j];
            board[n-1-i][n-1-j] = board[j][n-1-i];
            board[j][n-1-i] = saved;
        }
    }
    boardCopy();
}

void CCrazyMatrix::transformRotCCW()
{
    int n = numSides;
    for (int i=0; i<n/2; i++) {
        for (int j=i; j<n-i-1; j++) {
            int saved = board[i][j];
            board[i][j] = board[j][n-1-i];
            board[j][n-1-i] = board[n-1-i][n-1-j];
            board[n-1-i][n-1-j] = board[n-j-1][i];
            board[n-j-1][i] = saved;
        }
    }
    boardCopy();
} 

void CCrazyMatrix::Print()
{
	for (int i=0; i<numSides; i++) {
		printf("> ");
		for (int j=0; j<numSides; j++) {
			printf("%d ",board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
