//
//  GBAI.h
//  Gobang
//
//  Created by PY on 13-6-10.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#ifndef __Gobang__GBAI__
#define __Gobang__GBAI__

#include <iostream>
#include "GBConfig.h"
#include "vector"
#include "set"
#define INFINIT 1000

struct SortNode {
    int score;
    GBIntPoint pos;
    SortNode(){
        score = 0;
        pos.r = pos.c = 0;
    }
    SortNode(int s, int r, int c)
    {
        score = s;
        pos.r = r;
        pos.c = c;
    }
    bool operator<(const SortNode &t1) const{
        return (score < t1.score);
    }
    bool operator>(const SortNode &t1) const{
        return (score > t1.score);
    }
};

typedef unsigned short PatType;
typedef std::vector<GBIntPoint> SeqIntPt;
typedef std::set<SortNode, std::greater<SortNode> > SortType;

//===========以下是对外公开调用的函数============
/*
 @brief 对当前棋局，计算team在board上放置棋子的最好位置。
 @param board 棋局
 @param team 要放子的一方
 @param depth 搜索最大深度
 @return team的最佳放子位置
 */
GBIntPoint getAIPos(int **board, PieceTeam team=PieceTeamWhite, int depth=kDefaultSearchDepth);

/*
 @brief 供测试使用
 */
void aiTest(int **board, PieceTeam team);

//============以下是内部使用的函数，不公开=========

/*
 @brief alpah-beta搜索函数，寻找最适合的一个放子点
 @param board 棋局
 @param depth 当前深度
 @param alpha MAX结点的搜索下限
 @param beta MIN结点的搜索上限
 @param team 当前要下子的一方
 @return 此时的alpha值
 */
int AlphaBeta(int **oriBoard, int depth, int alpha, int beta, PieceTeam team);

/*
 @brief 获得当前所有可扩展的位置。为了减少无意义的搜索，限定可扩展位置只在所有棋子周围距离为2
        的未放子处。
 @param board 当前棋局
 @return 所有符合要求的位置坐标
 */
SeqIntPt getCandidate(int **board);

/*
 @brief 用于每层搜索的估价。基本方法同oneStepEvaluation函数。不过不苛求必赢模式。
 @param oriBoard 棋局
 @param team 要判断的一方
 @param pos 要进行估测价值的位置
 @return team方在pos处下子的估价。
 */
int sortEvaluation(int **oriBoard, PieceTeam team, GBIntPoint pos);

/*
 @brief 用于第一层的剪枝，反应该方在此位置下子的价值。使用必赢模式（80分及以上）来估测。
        先得到该方如果在该位置下子，其自己的得分（进攻分），再得到对方在该位置下子，对方
        的得分（防守分）。在任意一方有必赢局面的情况下，使用最大得分作为该位置对该方来说
        的价值。
 @param oriBoard 棋局
 @param team 要判断的一方（其实这里可以省略，由于是第一层，所以必定会是AI一方，
             所以这里的值必定是AITeam）
 @param pos 要进行估测价值的位置
 @return team方在pos处下子所得到的最大价值，若双方都无必赢模式，返回负无穷。
 */
int oneStepEvaluation(int **oriBoard, PieceTeam team, GBIntPoint pos);

/*
 @brief 评价函数，用于alpha-beta叶节点的评价。只会评价棋局上某一方的得分。
 @param oriBoard 棋局
 @param team 要评价的一方
 @return 此方得分
 */
int Evaluate(int **oriBoard, PieceTeam team);

/*
 @brief 给定队伍、起点和方向，检测该方向上该子的模式并返回
 @param board 当前棋盘
 @param team 要检测的队伍
 @param st 检测的起点
 @param delt 要检测的方向
 @return 1、当该点反方向前一个位置还是同一个队伍的棋子，不做检测，返回0；
         2、该点在给定方向上5个棋子（包括起始点）范围内有墙或者其他棋子，返回负值，
            其绝对值为障碍点距离起始点的距离；
         3、不属于以上情况，返回该方向上的模式。
 
 */
int getPattern(int **board, PieceTeam team, GBIntPoint st, GBIntPoint delt);

/*
 @brief 通过当前棋局中已有的模式计算得分。模式为一个无符号整数，其中只有六个位有效，
         最高位代表死(1)活(0)。为1则代表有棋子，0则为无棋子。
         评分规则如下（镜像同样）：
             0 11111(31)       => 100    成5
             1 11111(63)       => 100    成5
             0 11110(30)       => 90     活4
             1 11110(62) *2    => 90     双死4
             1 11110 0 11100   => 90     死4活3
             0 11100(28) *2    => 80     双活3
             1 11100 0 11100   => 70     死3活3
             1 11110(62)       => 60     死4
             0 11100(28)       => 50     活3
             0 11000(24) *2    => 40     双活2
             1 11100(60)       => 30     死3
             0 11000(24)       => 20     活2
             1 11000(56)       => 10     死2
 
 @param pat 当前棋局中已有的模式，下标为模式编号，值为此编号的模式的数量
 @return 总得分，无匹配得分模式返回0分
 */
int CalScore(PatType pat[]);

/*
 @brief 除最高位保留，反转pattern低5位
 @param ori 需要反转的pattern
 @return 反转后的pattern
 */
PatType ReversePattern(PatType ori);

#endif /* defined(__Gobang__GBAI__) */
