//
//  GBAI.cpp
//  Gobang
//
//  Created by PY on 13-6-10.
//  Copyright (c) 2013年 PY. All rights reserved.
//

#include "GBAI.h"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

int InDepth = 0;
PieceTeam AITeam = PieceTeamWhite;
GBIntPoint BestPiece = GBIntPointMake(0, 0);

//====================对外函数=========================
/*
 @brief 对当前棋局，计算team在board上放置棋子的最好位置。
 @param board 棋局
 @param team 要放子的一方
 @param depth 搜索最大深度
 @return team的最佳放子位置
 */
GBIntPoint getAIPos(int **board, PieceTeam team, int depth)
{
    InDepth = depth;
    AITeam = team;
    
    AlphaBeta(board, depth, -INFINIT, INFINIT, team);
    
    delete2DIntArray(board);
    printf("AI piece: %d, %d", BestPiece.r, BestPiece.c);
    return BestPiece;
}

/*
 @brief 供测试使用
 */
void aiTest(int **board, PieceTeam team)
{
    //    int score = Evaluate(board, PieceTeamBlack);
    //    printf("Black: %d\n", score);
    //    score = Evaluate(board, PieceTeamWhite);
    //    printf("White: %d\n", score);
}

//=============内部函数===============

/*
 @brief 除最高位保留，反转pattern低5位
 @param ori 需要反转的pattern
 @return 反转后的pattern
 */
PatType ReversePattern(PatType ori)
{
    int live = ori & 32;
    int pat = ori & 31;
    PatType n = 0;
    for (int i=1; i<=5; ++i)
    {
        n <<= 1;
        int cur = (pat >> (i-1)) & 1;
        n |= cur;
    }
    n |= live;
    return n;
}


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
int CalScore(PatType pat[])
{
    struct SNode {
        int score;
        int req[2][2];
        SNode():score(0){req[0][0]=req[1][0]=-1;}
        SNode(int sc, int pat1, int cnt1, int pat2=-1, int cnt2=-1){
            score = sc;
            req[0][0] = pat1;
            req[0][1] = cnt1;
            req[1][0] = pat2;
            req[1][1] = cnt2;
        }
    };
    
    SNode scale[] = {   // 分值表
        SNode(INFINIT, 63, 1),
        SNode(INFINIT, 31, 1),
        SNode(90, 30, 1),
        SNode(90, 62, 2),
        SNode(90, 62, 1, 28, 1),
        SNode(80, 28, 2),
        SNode(70, 60, 1, 28, 1),
        SNode(60, 62, 1),
        SNode(50, 28, 1),
        SNode(40, 24, 2),
        SNode(30, 60, 1),
        SNode(20, 24, 1),
        SNode(10, 56, 1)
    };
    
    bool flag = true;
    for (int i=0; i<(sizeof(scale)/sizeof(scale[0])); ++i) {
        // 从分值最高的模式开始检测 一旦满足直接返回最高分
        SNode cur = scale[i];
        
        flag = true;
        for (int j=0; j<2; ++j) {
            if (cur.req[j][0] == -1)
                continue;
            
            if (pat[cur.req[j][0]]+pat[ReversePattern(cur.req[j][0])] < cur.req[j][1])
                flag = false;
        }
        if (flag)
            return cur.score;
    }
    
    return 0;
}

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
int getPattern(int **board, PieceTeam team, GBIntPoint st, GBIntPoint delt)
{
    int tr = st.r + delt.r*(-1); //获得反方向前一个位置 判断是死是活
    int tc = st.c + delt.c*(-1);
    
    bool live = true;
    if (tr<0 || tr>=kBoardRow || tc<0 || tc>=kBoardCol) {
        live = false;
    }
    else {
        if (board[tr][tc]==team)  // 连排的中间 不需要检查
            return 0;
        live = (board[tr][tc]==PieceTeamNone)?true:false;
    }
    
    PatType curPat = 1;
    for (int l=1; l<5; ++l) {
        curPat <<= 1;
        tr = st.r + delt.r*l;
        tc = st.c + delt.c*l;
        if (tr<0 || tr>=kBoardRow || tc<0 || tc>=kBoardCol){ //超出边界
            return -l;
        }
        else if (board[tr][tc]==PieceTeamNone){ //当前格点没有棋子
            curPat |= 0;
        }
        else if (board[tr][tc] == team){    //当前格点是自己棋子
            curPat |= 1;
        }
        else{   //当前格点是对方棋子
            return -l;
        }
    }
    curPat |= (live?0:1)<<5;
    return curPat;

}

/*
 @brief 评价函数，用于alpha-beta叶节点的评价。只会评价棋局上某一方的得分。
 @param oriBoard 棋局
 @param team 要评价的一方
 @return 此方得分
 */
int Evaluate(int **oriBoard, PieceTeam team)
{
    int dr[] = {0, 1, 1, 1}; //右、右上、上、左上
    int dc[] = {1, 1, 0,-1}; //由于是从左下到右上的顺序搜的 所以只需要检查这四个方向，避免重复
//    int dr[] = { 1, 1, 0,-1,-1,-1, 0, 1}; //从正上方开始顺时针
//    int dc[] = { 0, 1, 1, 1, 0,-1,-1,-1};
    int **board = copy2DIntArray(oriBoard, kBoardRow, kBoardCol);
    PatType pattern[70] = {0};  // 存放模式的数组，key为模式，value为数量
    
    // 寻找棋盘上该方所有的模式并存入pattern数组
    for (int i=0; i<kBoardRow; ++i) {
        for (int j=0; j<kBoardCol; ++j) {
            if(board[i][j] != team)
                continue;
            for (int k=0; k<4; ++k) {   //检查4个方向
                int curPat = getPattern(board, team, GBIntPointMake(i, j), GBIntPointMake(dr[k], dc[k]));
                if (curPat > 0) {   //正向能够找到模式
                    ++pattern[curPat];
                }
                else if (curPat < 0) {  //正向有阻碍，尝试从最后一个位置反向查
                    curPat = -curPat;
                    int nr=i+dr[k]*(curPat-1), nc=j+dc[k]*(curPat-1);
                    // 获取到最后一个连续有子的位置
                    while (board[nr][nc] != team) {
                        nr -= dr[k];
                        nc -= dc[k];
                    }
                    curPat = getPattern(board, team, GBIntPointMake(nr, nc), GBIntPointMake(-dr[k], -dc[k]));
                    if (curPat > 0) {
                        ++pattern[curPat];
                    }
                }
            }
        }
    }
    
    delete2DIntArray(board);
    // 通过得到的模式来计算得分
    int score = CalScore(pattern);
    return score;
}

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
int oneStepEvaluation(int **oriBoard, PieceTeam team, GBIntPoint pos)
{
    int **board = copy2DIntArray(oriBoard, kBoardRow, kBoardCol);
    int attScore = Evaluate(board, team);
    board[pos.r][pos.c] = AnotherTeam(team);
    int defScore = Evaluate(board, AnotherTeam(team));
    
    delete2DIntArray(board);
    
    if (attScore>=80 && defScore<80){
        // 自己在pos处下子有必赢模式而对方没有
        return attScore;
    }
    else if (attScore<80 && defScore>=80){
        // 对方在pos处下子有必赢模式而自己没有
        return defScore;
    }
    else if (attScore>=80 && defScore>=80){
        // 双方在pos处下子都有必赢模式，使用较大得分
        return MAX(attScore, defScore);
    }
    
    return -INFINIT;
}

/*
 @brief 用于每层搜索的估价。基本方法同oneStepEvaluation函数。不过不苛求必赢模式。
 @param oriBoard 棋局
 @param team 要判断的一方
 @param pos 要进行估测价值的位置
 @return team方在pos处下子的估价。
 */
int sortEvaluation(int **oriBoard, PieceTeam team, GBIntPoint pos)
{
    int **board = copy2DIntArray(oriBoard, kBoardRow, kBoardCol);
    board[pos.r][pos.c] = team;
    int attScore = Evaluate(board, team);
    board[pos.r][pos.c] = AnotherTeam(team);
    int defScore = Evaluate(board, AnotherTeam(team));
    delete2DIntArray(board);
    return MAX(attScore, defScore);
}

/*
 @brief 获得当前所有可扩展的位置。为了减少无意义的搜索，限定可扩展位置只在所有棋子周围距离为2
        的未放子处。
 @param board 当前棋局
 @return 所有符合要求的位置坐标
 */
SeqIntPt getCandidate(int **board)
{
    int dr[] = { 1, 1, 0,-1,-1,-1, 0, 1}; //从正上方开始顺时针
    int dc[] = { 0, 1, 1, 1, 0,-1,-1,-1};
    SeqIntPt candi;
    bool check[kBoardRow][kBoardCol] = {false};
    for (int i=0; i<kBoardRow; ++i) {
        for (int j=0; j<kBoardCol; ++j) {
            if (board[i][j] == PieceTeamNone)
                continue;
            for (int dep=1; dep<=2; ++dep){
                for (int k=0; k<8; ++k) {
                    int tr = i + dr[k]*dep;
                    int tc = j + dc[k]*dep;
                    if (tr<0 || tr>=kBoardRow || tc<0 || tc>=kBoardCol)
                        continue;
                    
                    if (board[tr][tc]==PieceTeamNone && !check[tr][tc]) {
                        check[tr][tc] = true;
                        candi.push_back(GBIntPointMake(tr, tc));
                    }
                }
                
            }
        }
    }
    return candi;
}


/*
 @brief alpah-beta搜索函数，寻找最适合的一个放子点
 @param board 棋局
 @param depth 当前深度
 @param alpha MAX结点的搜索下限
 @param beta MIN结点的搜索上限
 @param team 当前要下子的一方
 @return 此时的alpha值
 */
int AlphaBeta(int **oriBoard, int depth, int alpha, int beta, PieceTeam team)
{
    int **board = copy2DIntArray(oriBoard, kBoardRow, kBoardCol);
    
    if (depth == 0) {
        // 评估分综合双方棋局得到
        int s1 = Evaluate(board, team);
        int s2 = Evaluate(board, AnotherTeam(team));
        int score = s1-s2;
        return score;
    }
    
    SeqIntPt candi = getCandidate(board);
    
    // 先对所有可扩展结点进行估价，并排序
    SortType sortedCandi;
    for (SeqIntPt::iterator iter=candi.begin(); iter!=candi.end(); ++iter){
        int score = sortEvaluation(board, team, *iter);
        sortedCandi.insert(SortNode(score, iter->r, iter->c));
    }
    
    // 根据启发函数对可扩展结点排序后的顺序来扩展
    for(SortType::iterator it=sortedCandi.begin(); it!=sortedCandi.end(); ++it)
    {
        GBIntPoint pos = it->pos;
        if (board[pos.r][pos.c] != PieceTeamNone)
            continue;
        board[pos.r][pos.c] = team;
        
        // 第一层的剪枝优化：如果己方或对方在此处有必赢棋局，直接放在此处
        if (depth == InDepth) {
            int tscore = oneStepEvaluation(board, team, pos);
//            printf("candidate: %d, %d; score: %d\n",pos.r, pos.c, tscore);
            if (tscore>alpha){  //评分80及以上的局势都是必赢的局
//                for (int i=kBoardRow-1; i>=0; --i) {
//                    for (int j=0; j<kBoardCol; ++j) {
//                        printf("%d ", board[i][j]);
//                    }
//                    printf("\n");
//                }
//                printf("%d\n", tscore);
//                Evaluate(board, team);
                
                alpha = tscore;
                BestPiece.r = pos.r;
                BestPiece.c = pos.c;
            }
        }
        
        // 递归搜索
        int val = -AlphaBeta(board, depth - 1, -beta, -alpha, AnotherTeam(team));
        
        // 还原现场
        board[pos.r][pos.c] = PieceTeamNone;
        
        // alpha-beta 剪枝
        if (val >= beta) {
            delete2DIntArray(board);
            return beta;
        }
        if (val>alpha && board[pos.r][pos.c]==PieceTeamNone) {
            alpha = val;
            if (depth == InDepth){  // 第一层，更新最优位置
                BestPiece.r = pos.r;
                BestPiece.c = pos.c;
            }
        }
    }
    
    delete2DIntArray(board);
    return alpha;
}
