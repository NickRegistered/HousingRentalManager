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

#define SCR_ROW 25             /*��Ļ����*/
#define SCR_COL 80             /*��Ļ����*/

HANDLE gh_std_out; /*��׼����豸���*/
HANDLE gh_std_in;  /*��׼�����豸���*/
char *gp_scr_att = NULL; /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/

/**
*��Ļ������Ϣ�������ṹ
*/
typedef struct layer_node{
    char LayerNum;             /*�������ڲ���*/
    SMALL_RECT rcArea;         /*����������������*/
    CHAR_INFO *pContent;       /*�������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;             /*�������������ַ���Ԫԭ���Դ洢������*/
    struct layer_node *next;   /*��һ���ĵ�ַ*/
}LAYER;
LAYER *gp_top_layer = NULL;

/**
*�����ṹ
*/
typedef struct hot_area {
    SMALL_RECT *pArea;     /*������λ�����׵�ַ*/
    char *pSort;           /*������������׵�ַ*/
    char *pTag;            /*������������׵�ַ*/
    int num;               /*��������*/
} HOT_AREA;


/**
*��ǩ���ṹ
*/
typedef struct labe1_bundle {
    char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
    int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

CHAR_INFO *buff_menubar_info;/*�˵����ַ���Ϣ������*/

/**
*�ͷ���Ϣ�����ṹ
*/
typedef struct room_info
{
    short r_num;                     /**< �ͷ����*/
    short r_tel;                     /**< �ͷ��绰*/
    char b_flag;                     /**< ��̨��Ǳ���*/
    char r_type;                     /**< �ͷ�����*/
    char g_flag;                     /**< ��ס��Ǳ���*/
    short r_area;                    /**< �ͷ����*/
    float rent;                      /**< �ͷ����*/
    struct room_info *next;          /**< ָ����һ����ָ��*/
    struct guest *gnext;             /**< ָ�������Ϣ֧����ָ��*/
}ROOM;

/**
*�ͷ������Ϣ�����ṹ
*/
typedef struct room_type
{
    char type;                      /**< �ͷ����*/
    short bed_amount;               /**< ����ͷ�ÿ�䷿�����ס����*/
    short room_amount;              /**< �ͷ�����*/
    short room_vacuum;              /**< δ��ס�ͷ���*/
    struct room_type *next;         /**< ָ����һ����ָ��*/
    ROOM *rnext;                    /**< ָ�򷿼���Ϣ֧����ָ��*/
}R_type;

/**
*������Ϣ�����ṹ
*/
typedef struct guest
{
    char g_id[20];              /**< �������֤��*/
    char g_name[20];            /**< ��������*/
    short r_num;                /**< ��ס�ͷ����*/
    short in_months;            /**< ��ס����*/
    char in_date[18];           /**< ��סʱ��*/
    char out_date[18];          /**< �˷�ʱ��*/
    float pay_s;                /**< Ӧ�ɷ���*/
    float pay_a;                /**< ʵ�ʽɷ�*/
    struct guest *next;         /**< ָ����һ����ָ��*/
}GUEST;


char *main_menu[]={"��Ϣά��<M>",          /*ϵͳ���˵���*/
                   "��Ϣ��ѯ<I>",
                   "��Ϣͳ��<S>",
                   "�ļ�<F>",
                  };

char *sub_menu[]={"[T]�����Ϣά��",      /*ϵͳ�Ӳ˵���*/
                  "[R]������Ϣά��",
                  "[G]������Ϣά��",

                  "[T]�����Ϣ��ѯ",
                  "[R]������Ϣ��ѯ",
                  "[G]������Ϣ��ѯ",

                  "[1]�ͷ�������Ϣ����ͳ��",
                  "[2]����ͷ�Ӫҵ���ͳ��",
                  "[3]���пͷ���ס�ʵ���Ϣ����ͳ��",
                  "[4]�ۼ��ⷿ��������10������",
                  "[5]���˽ɷ���Ϣͳ��",

                  "[S]��������",
                  "[X]�˳�ϵͳ",
                 };

int gi_sel_menu=1;                          /*��ѡ�е����˵���ţ���ֵΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/
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
