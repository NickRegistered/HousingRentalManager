#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>

#ifndef ROOM_H_INCLUDED
#define ROOM_H_INCLUDED

#define SCR_ROW 25             /*屏幕行数*/
#define SCR_COL 80             /*屏幕列数*/

HANDLE gh_std_out; /*标准输出设备句柄*/
HANDLE gh_std_in;  /*标准输入设备句柄*/
char *gp_scr_att = NULL; /*存放屏幕上字符单元属性值的缓冲区*/

/**
*屏幕窗口信息链结点结点结构
*/
typedef struct layer_node{
    char LayerNum;             /*弹出窗口层数*/
    SMALL_RECT rcArea;         /*弹出窗口区域坐标*/
    CHAR_INFO *pContent;       /*弹出窗口区域字符单元原信息存储缓冲区*/
    char *pScrAtt;             /*弹出窗口区域字符单元原属性存储缓冲区*/
    struct layer_node *next;   /*下一结点的地址*/
}LAYER;
LAYER *gp_top_layer = NULL;

/**
*热区结构
*/
typedef struct hot_area {
    SMALL_RECT *pArea;     /*热区定位数组首地址*/
    char *pSort;           /*热区类别数组首地址*/
    char *pTag;            /*热区序号数组首地址*/
    int num;               /*热区个数*/
} HOT_AREA;


/**
*标签束结构
*/
typedef struct labe1_bundle {
    char **ppLabel;        /**< 标签字符串数组首地址*/
    COORD *pLoc;           /**< 标签定位数组首地址*/
    int num;               /**< 标签个数*/
} LABEL_BUNDLE;

CHAR_INFO *buff_menubar_info;/*菜单条字符信息缓冲区*/

/**
*客房信息链结点结构
*/
typedef struct room_info
{
    short r_num;                     /**< 客房编号*/
    short r_tel;                     /**< 客房电话*/
    char b_flag;                     /**< 阳台标记变量*/
    char r_type;                     /**< 客房类型*/
    char g_flag;                     /**< 入住标记变量*/
    short r_area;                    /**< 客房面积*/
    float rent;                      /**< 客房租金*/
    struct room_info *next;          /**< 指向下一结点的指针*/
    struct guest *gnext;             /**< 指向客人信息支链的指针*/
}ROOM;

/**
*客房类别信息链结点结构
*/
typedef struct room_type
{
    char type;                      /**< 客房类别*/
    short bed_amount;               /**< 该类客房每间房最大入住人数*/
    short room_amount;              /**< 客房总数*/
    short room_vacuum;              /**< 未入住客房数*/
    struct room_type *next;         /**< 指向下一结点的指针*/
    ROOM *rnext;                    /**< 指向房间信息支链的指针*/
}R_type;

/**
*客人信息链结点结构
*/
typedef struct guest
{
    char g_id[20];              /**< 客人身份证号*/
    char g_name[20];            /**< 客人姓名*/
    short r_num;                /**< 入住客房编号*/
    short in_months;            /**< 入住月数*/
    char in_date[18];           /**< 入住时间*/
    char out_date[18];          /**< 退房时间*/
    float pay_s;                /**< 应缴费用*/
    float pay_a;                /**< 实际缴费*/
    struct guest *next;         /**< 指向下一结点的指针*/
}GUEST;


char *main_menu[]={"信息维护<M>",          /*系统主菜单名*/
                   "信息查询<I>",
                   "信息统计<S>",
                   "文件<F>",
                  };

char *sub_menu[]={"[T]类别信息维护",      /*系统子菜单名*/
                  "[R]房间信息维护",
                  "[G]客人信息维护",

                  "[T]类别信息查询",
                  "[R]房间信息查询",
                  "[G]客人信息查询",

                  "[1]客房数量信息按类统计",
                  "[2]各类客房营业额按月统计",
                  "[3]所有客房入住率等信息按年统计",
                  "[4]累计租房月数最多的10个客人",
                  "[5]客人缴费信息统计",

                  "[S]保存数据",
                  "[X]退出系统",
                 };

int gi_sel_menu=1;                          /*被选中的主菜单项号，初值为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/
int sub_menu_num[] = {3,3,5,2};

void LoadFile(R_type **phead);
void RunSys(R_type **phead);
void ExeFunction(int m, int s, R_type **phead);
void ShowMenu(void);
void TagMainMenu(int num);
void ClearScreen(void);
void ClearWindow(void);

void PopMenu(int num);
void PopOff(void);
void PopUp(SMALL_RECT *pRc,WORD att,LABEL_BUNDLE *pLabel,HOT_AREA *pHotArea);
void DrawBox(SMALL_RECT *pRc);
void TagSubMenu(int num);

void maintain_R_type(R_type **phead);
void maintain_ROOM(R_type **phead);
void maintain_GUEST(R_type **phead);

void maintain_rtype_add(R_type **phead);
void maintain_rtype_modify(R_type **phead);
BOOL maintain_rtype_delete(R_type **phead);

void maintain_rinfo_add(R_type **phead);
void maintain_rinfo_modify(R_type **phead);
BOOL maintain_rinfo_delete(R_type **phead);

void maintain_ginfo_add(R_type **phead);
void maintain_ginfo_modify(R_type **phead);
void maintain_ginfo_money(R_type **phead);
BOOL maintain_ginfo_delete(R_type **phead);

BOOL search_R_type(R_type **phead);
R_type* search_rtype(R_type **phead);

void search_ROOM(R_type **phead);
void search_rtype_plus(R_type **phead);
ROOM* search_rnum(R_type **phead);

void search_GUEST(R_type **phead);
GUEST* search_gID(R_type **phead);
void search_ginfo(R_type **phead);

void statistic_1(R_type **phead);
void statistic_2(R_type **phead);
void statistic_3(R_type **phead);
void statistic_4(R_type **phead);
void statistic_5(R_type **phead);

void SaveData(R_type **phead);
void ExitSys(R_type **phead);

#endif // ROOM_H_INCLUDED
