#include "room.h"
char over = 0;/*RunSys�������ж��Ƿ�������еı���,ֻ������ExitSysʱ�Ÿ���Ϊ1*/

unsigned long ul;

int main()
{
    R_type **phead;
    COORD size = {SCR_COL , SCR_ROW};           /*���ڻ�������С*/
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE);/*��ȡ��׼����豸���*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);  /*��ȡ��׼�����豸���*/

    SetConsoleTitle("���ݳ������ϵͳ");         /*���ô��ڱ���*/
    SetConsoleScreenBufferSize(gh_std_out , size);/*���ô��ڻ�������С*/
    SetConsoleTextAttribute(gh_std_out , att);    /*���ð�ɫǰ���ͺ�ɫ����*/
    LoadFile(phead);
    ClearWindow();                                /*����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char*)malloc(SCR_COL*SCR_ROW*sizeof(char));/*�����ַ�����*/
    gp_top_layer = (LAYER*)malloc(sizeof(LAYER));
    gp_top_layer->LayerNum = 0;                              /*�������ڲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL-1;
    gp_top_layer->rcArea.Bottom = SCR_ROW-1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();         /*��ʾϵͳ���˵�*/
    RunSys(phead);

    return 0;
}

/**
 * ��������: LoadFile
 * ��������: ������������ݴ������ļ����뵽�ڴ滺������ʮ��������.
 * �������: phead ����ͷָ��ĵ�ַ, ����������������ʮ�������ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: �ù�������������Ҫ����֮ǰ�����ݼ��أ�������ļ�ʧ��
 *           �򲻻�����ϵͳ���������ʾ��Ϣ��
 */
void LoadFile(R_type **phead)
{
    R_type *head=NULL, *pR_type,tmp1;
    ROOM *pROOM, tmp2;
    GUEST *pGUEST, tmp3;
    FILE *pFile;
    char find;
    if((pFile = fopen("RoomType.dat","rb")) == NULL)
    {
         printf("\n�ͷ�����ļ���ʧ��");
         exit(-1);
    }

    while(fread(&tmp1,sizeof(R_type),1,pFile) == 1)
    {/*����ͷ������Ϣ����������*/
        pR_type = (R_type *)malloc(sizeof(R_type));
        *pR_type = tmp1;        //��ָ�뱾�����ݽ����޸ģ�������Ҫ����ָ����Ϊ��ڲ���
        pR_type->rnext = NULL;  //��ʼ�������ͽ���µķ�����Ϣ����Ϊ��
        pR_type->next = head;
        head = pR_type;
    }
    if(head == NULL)
    {/*ͷָ��Ϊ��ָ��˵��δ���������Ϣ������¼�������*/
        printf("\n��δ����κοͷ������Ϣ������ӿͷ������Ϣ:");
        maintain_rtype_add(&head);
    }
    *phead = head;
    fclose(pFile);

    if((pFile = fopen("RoomInfo.dat","rb")) == NULL)
    {
        printf("\n�ͷ�������Ϣ�ļ���ʧ��");
        exit(-1);
    }
    while(fread(&tmp2,sizeof(ROOM),1,pFile) == 1)
    {/*���뷿����Ϣ����������*/
        pROOM = (ROOM *)malloc(sizeof(ROOM));
        *pROOM = tmp2;
        pROOM->gnext = NULL; //��ʼ��������Ϣ����µĿ�������Ϊ��

        pR_type = head;
        while((pR_type != NULL)&&(pR_type->type != pROOM->r_type))
        {/*˳������������Ҹÿͷ��Ŀͷ����ͽ��*/
            pR_type = pR_type->next;
        }
        if(pR_type)
        {/*����ҵ������*/
            pROOM->next = pR_type->rnext;
            pR_type->rnext = pROOM;
        }
        else
        {/*���δ�ҵ����ͷŴ洢�÷�����Ϣ���ڴ�*/
            free(pROOM);
        }
    }
    fclose(pFile);

    if((pFile = fopen("GuestInfo.dat","rb")) == NULL)
    {
        printf("\n������Ϣ�ļ���ʧ��");
        exit(-1);
    }
    while(fread(&tmp3,sizeof(GUEST),1,pFile) == 1)
    {/*���������Ϣ����������*/
        pGUEST = (GUEST *)malloc(sizeof(GUEST));
        *pGUEST = tmp3;

        pR_type = head;
        find = 0;
        while(pR_type && !find)
        {/*û�в��������пͷ�������û���ҵ�ʱ�����в��ҵ�ѭ��*/
            pROOM = pR_type->rnext;
            while(pROOM && !find)
            {/*û�в��������пͷ�����û���ҵ�ʱ�����в��ҵ�ѭ��*/
                if(pGUEST->r_num == pROOM->r_num)
                {/*���ҵ��������ڿͷ�ʱ�����findΪ1����ֹͣ����*/
                    find = 1;
                    break;
                }
                pROOM = pROOM->next;
            }
            pR_type = pR_type->next;
        }
        if(find)
        {/*��������Ϣ������ӵ�����Ŀ���������*/
            pGUEST->next = pROOM->gnext;
            pROOM->gnext = pGUEST;
        }
        else
        {
            free(pGUEST);
        }
    }
}

/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ShowMenu(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    CONSOLE_CURSOR_INFO cci;
    COORD size;
    COORD pos = {0,0}, pos2={0,2};
    int i,j;
    int PosA = 2, PosB;
    char ch;

    GetConsoleScreenBufferInfo(gh_std_out , &csbi);
    size.X = csbi.dwSize.X;
    size.Y = 1;

    SetConsoleCursorPosition(gh_std_out , pos);
    for(i = 0; i<4; i++)        /*��һ�е�һ��������˵���*/
    {
        printf("  %s  ", main_menu[i]);
    }

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    buff_menubar_info = (CHAR_INFO*)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X - 1,0};

    /*����һ�����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, buff_menubar_info , size,pos , &rcMenu);
    /*����һ���ַ���Ԫ��Ϊ�׵׺���*/
    for(i=0; i<size.X ; i++)
    {
        (buff_menubar_info+i)->Attributes = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
    }
    /*�޸ĺ���ַ���Ϣ��д����һ��*/
    WriteConsoleOutput(gh_std_out, buff_menubar_info, size, pos, &rcMenu);
    TagMainMenu(gi_sel_menu);/*���ѡ�����˵�*/
    SetConsoleCursorPosition(gh_std_out, pos2);

    return;
}

/**
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD size;
    COORD pos={0,0};
    int PosA=2, PosB;
    char ch;
    int i;

    if(num == 0)/*numΪ0ʱȥ�����*/
    {
        PosA=0;
        PosB=0;
    }

    else    /*���򣬶�λѡ�����˵������ֹλ�ã�PosAΪ��ʼλ�ã�PosBΪ��ֹλ��*/
    {
        for(i=1; i<num; i++)
        {
            PosA += strlen(main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo(gh_std_out, &csbi);
    size.X = csbi.dwSize.X;
    size.Y = 1;

    /*ȥ��ǰ��˵���ı��*/
    for(i=0; i<PosA; i++)
    {
        (buff_menubar_info+i)->Attributes = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
    }
    /*���ѡ�в˵���ڵװ���*/
    for(i=PosA; i<PosB; i++)
    {
        (buff_menubar_info+i)->Attributes = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
    }
    /*ȥ������˵���ı��*/
    for(i=PosB; i<size.X; i++)
    {
        (buff_menubar_info+i)->Attributes = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
    }
    /*�����ñ�ǵ���Ϣ��д����һ��*/
    SMALL_RECT rcMenu = {0,0,size.X-1,0};
    WriteConsoleOutput(gh_std_out,buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: ClearWindow
 * ��������: �������̨������Ļ��Ϣ
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearWindow(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD home={0,0};
    unsigned long size;

    GetConsoleScreenBufferInfo(gh_std_out, &csbi);
    size = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputAttribute(gh_std_out, csbi.wAttributes, size, home, &ul);
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * ��������: ClearScreen
 * ��������: ������˵����µ���Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen(void)
{
    unsigned long size;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD pos = {0,1};

    GetConsoleScreenBufferInfo(gh_std_out, &csbi);
    size = csbi.dwSize.X*(csbi.dwSize.Y - 1);

    FillConsoleOutputAttribute(gh_std_out, csbi.wAttributes, size, pos, &ul);
    FillConsoleOutputCharacter(gh_std_out, ' ', size, pos, &ul);
    SetConsoleCursorPosition(gh_std_out, pos);

    return;
}

/**
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopMenu(int num)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
    {
        if (gp_top_layer->LayerNum != 0) /*�����ǰ�����Ӳ˵�����*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNum != 0) /*���ѵ������Ӳ˵����򷵻�*/
    {
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += sub_menu_num[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = sub_menu_num[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2;

    gi_sel_sub_menu = 0;
    return;
}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER *)malloc(sizeof(LAYER));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNum = gp_top_layer->LayerNum + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),pLabel->pLoc[i], &ul);
        }
    }
    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNum;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++)
        {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                    | (pHotArea->pTag[i] << 2);
        }
    }
    return;
}

/**
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopOff(void)
{
    LAYER *nextLayer;
    COORD size;
    COORD pos={0,0};
    char *pCh;
    int i, j;
    if((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {/*����Ϊ��������Ļ��Ϣʱ�����ùر�*/
        return;
    }

    nextLayer = gp_top_layer->next;

    /*�ָ��������ڸ�������ԭ���*/
    size.X = gp_top_layer->rcArea.Right-gp_top_layer->rcArea.Left+1;
    size.Y = gp_top_layer->rcArea.Bottom-gp_top_layer->rcArea.Top+1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));

    /*�ָ��ַ���Ԫ����*/
    pCh = gp_top_layer->pScrAtt;
    for(i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for(j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);/*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    gp_top_layer = nextLayer;
    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-','|'};
    COORD pos = {pRc->Left,pRc->Top};/*��λ���������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*�����Ͻ�*/

    for(pos.X=pRc->Left+1; pos.X<pRc->Right; pos.X++)/*���ϱ߿����*/
    {
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*�����Ͻ�*/

    for(pos.Y=pRc->Top+1; pos.Y<pRc->Bottom; pos.Y++)/*�����ұ߿����*/
    {
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1,pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }

    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*�����½�*/

    for(pos.X=pRc->Left+1; pos.X<pRc->Right; pos.X++)/*���±߿����*/
    {
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*�����½�*/

    return;
}

/**
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    /*���㵯���Ӳ˵�������λ�ã������rcPop��*/
    LocSubMenu(gi_sel_menu, &rcPop);
    if((num<1) || (num==gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {/*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left+2;
    width = rcPop.Right-rcPop.Left-3;

    if(gi_sel_sub_menu != 0)/*ȡ��ԭѡ���Ӳ˵���ı��*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        /*�׵׺���*/
        att = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    }

    /*�ڶ����Ӳ˵����������*/
    pos.X = rcPop.Left+2;
    pos.Y = rcPop.Top+num;
    /*�ڵװ���*/
    att = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;/*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc=0;
    rc->Top = 1;    /*�����ϱ߶�λ�ڵڶ��У��к�Ϊ1*/
    rc->Left = 1;
    for(i=1; i<num; i++)
    {/*����������߽�λ�ú͵�һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(main_menu[i-1]) + 4;
        loc += sub_menu_num[i-1];
    }

    rc->Right = strlen(sub_menu[loc]);/*�ݴ��һ���Ӳ˵����ַ�������*/
    for(i=1; i<sub_menu_num[num-1]; i++)
    {/*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(sub_menu[loc+i]);
        if(rc->Right < len)
        {
            rc->Right = len;
        }
    }
            rc->Right += rc->Left+3;/*���������ұ߽�*/
            /*���������±ߵ��к�*/
        rc->Bottom = rc->Top + sub_menu_num[num-1]+1;
        if(rc->Right >= SCR_COL)/*�ұ߽�Խ�紦��*/
        {
            len = rc->Right - SCR_COL + 1;
            rc->Left -= len;
            rc->Right = SCR_COL - 1;
        }

    return;
}

/**
 * ��������: RunSys
 * ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
 * �������: phead ����ͷָ��ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void RunSys(R_type **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (over == 0)
    {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
                 && inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            if (asc == 0) /*���Ƽ��Ĵ���*/
            {
                if (gp_top_layer->LayerNum == 0) /*���δ�����Ӳ˵�*/
                {
                    switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu < 1)
                            {
                                gi_sel_menu = 4;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu > 4)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 40:
                            ClearScreen();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else  /*�ѵ����Ӳ˵�ʱ*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += sub_menu_num[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu < 1)
                            {
                                gi_sel_menu = 4;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 38:
                            num = gi_sel_sub_menu - 1;
                            if (num < 1)
                            {
                                num = sub_menu_num[gi_sel_menu-1];
                            }
                            if (strlen(sub_menu[loc+num-1]) == 0)
                            {
                                num--;
                            }
                            TagSubMenu(num);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu > 4)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 40:
                            num = gi_sel_sub_menu + 1;
                            if (num > sub_menu_num[gi_sel_menu-1])
                            {
                                num = 1;
                            }
                            if (strlen(sub_menu[loc+num-1]) == 0)
                            {
                                num++;
                            }
                            TagSubMenu(num);
                            break;
                    }
                }
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*������������Alt��*/
                switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
                {
                    case 88:  /*Alt+X �˳�*/
                        if (gp_top_layer->LayerNum != 0)
                        {
                            PopOff();
                            gi_sel_sub_menu = 0;
                        }
                        ExeFunction(4,1,phead);
                        break;
                    case 77:  /*Alt+M*/
                        PopMenu(1);
                        break;
                    case 73: /*Alt+I*/
                        PopMenu(2);
                        break;
                    case 83: /*Alt+S*/
                        PopMenu(3);
                        break;
                    case 70: /*Alt+F*/
                        PopMenu(4);
                        break;
                }
            }

            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*������ͨ��*/
                if (gp_top_layer->LayerNum == 0)  /*���δ�����Ӳ˵�*/
                {
                    switch (vkc)
                    {
                        case 13: /*�س�*/
                            ClearScreen();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                {
                    if (vkc == 27) /*�������ESC��*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }

                    else if(vkc == 13) /*������»س���*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        ExeFunction(gi_sel_menu, num, phead);
                    }

                    else /*������ͨ���Ĵ���*/
                    {
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += sub_menu_num[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+sub_menu_num[gi_sel_menu-1]; i++)
                        {
                            if (strlen(sub_menu[i])>0 && vkc==sub_menu[i][1])
                            { /*���ƥ��ɹ�*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                ExeFunction(gi_sel_menu, i-loc+1,phead);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ExeFunction(int m, int s, R_type **phead)
{
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    void (* pFunction[13])();
    int i, loc;

    pFunction[0] = maintain_R_type;
    pFunction[1] = maintain_ROOM;
    pFunction[2] = maintain_GUEST;

    pFunction[3] = search_R_type;
    pFunction[4] = search_ROOM;
    pFunction[5] = search_GUEST;

    pFunction[6] = statistic_1;
    pFunction[7] = statistic_2;
    pFunction[8] = statistic_3;
    pFunction[9] = statistic_4;
    pFunction[10] = statistic_5;

    pFunction[11] = SaveData;
    pFunction[12] = ExitSys;

    for(i=1, loc=0; i<m; i++)
    {
        loc += sub_menu_num[i-1];
    }
    loc += s-1;
    if(pFunction[loc] != NULL)
    {
        ClearScreen();
        (*pFunction[loc])(phead);
    }

}

/**
 * ��������: maintain_R_type
 * ��������: ѡ��ά���ͷ�������Ϣģ�����ģ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_R_type(R_type **phead)
{
    void (*pFunction[3])();
    pFunction[0] = maintain_rtype_add;
    pFunction[1] = maintain_rtype_modify;
    pFunction[2] = maintain_rtype_delete;
    int option;

    fflush(stdin);
    printf("1.��ӿͷ������Ϣ\n2.�޸Ŀͷ������Ϣ\n3.ɾ���ͷ������Ϣ\n");
    scanf("%d",&option);

    if(option > 0 && option < 4 && pFunction[option] != NULL)
    {
        (*pFunction[option-1])(phead);
    }
    else
    {
        printf("\n����������ѡ�");
    }

}

/**
 * ��������: maintain_rtype_add
 * ��������: ¼��ͷ�������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_rtype_add(R_type **phead)
{
    R_type *pR_type, *tmp;

    tmp = (R_type*)malloc(sizeof(R_type));
    tmp->rnext = NULL;

    printf("\n������ͷ����(S,D,T,F):");
    fflush(stdin);
    scanf("%c",&tmp->type);
    if(tmp->type>'Z')
    {
        tmp->type+='A'-'a';
    }
    getchar();
    /*��ʼ�ͷ���𷿼������շ���Ϊ0*/
    tmp->room_amount = 0;
    tmp->room_vacuum = 0;
    printf("�ͷ������ס����:");
    scanf("%d",&tmp->bed_amount);

    pR_type = *phead;
    /*��ѭ�����ڲ��ҿͷ������*/
    while((pR_type != NULL) && (pR_type->type != tmp->type))
        pR_type = pR_type->next;

    if(pR_type != 0)
    {/*������ҵ������*/
        printf("\n�Ѿ����ڸ���ͷ���Ϣ");
        free(tmp);
    }
    else
    {
        tmp->next = *phead;
        *phead = tmp;
        printf("\n�����Ϣ¼��ɹ�!");

        pR_type = *phead;
        if(pR_type != NULL)
        {
            printf("\n��¼��Ŀͷ������");
            pR_type = *phead;
            while(pR_type)
            {
                printf("\n���:%-4cÿ�䷿�������:%4d��",pR_type->type,pR_type->bed_amount);
                pR_type = pR_type->next;
            }
        }
    }
}

/**
 * ��������: maintain_rtype_modify
 * ��������: �޸Ŀͷ�������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:�޸Ŀͷ������Ϣ������һЩ��������
 * ���磬�޸ĵĿͷ���������ڣ��޸ĺ�Ŀͷ����������������ظ�
 * ͨ����ɾ�����޸ĵ������Ϣ���ټ����޸ĺ�Ŀͷ������Ϣ��������������Լ������
 */
void maintain_rtype_modify(R_type **phead)
{
    if(maintain_rtype_delete(phead) == TRUE)
    {
        printf("\n�޸ĺ����Ϣ:\n");
        maintain_rtype_add(phead);
    }
}

/**
 * ��������: maintain_rtype_delete
 * ��������: ɾ���ͷ�������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: BOOL�ͣ��ҵ��÷��ಢɾ��ʱ����TRUE
 *            δ�ҵ��򷵻�FALSE
 * ����˵��:
 */
BOOL maintain_rtype_delete(R_type **phead)
{
    R_type *pR_type, *pR_type1;
    ROOM *pROOM, *pROOM_after;
    GUEST *pGUEST, *pGUEST_after;
    char type;

    printf("\n������ͷ����:");
    fflush(stdin);
    scanf("%c",&type);
    getchar();
    pR_type = *phead;
    pR_type1 = pR_type;
    while((pR_type != NULL) && (pR_type->type != type))
    {/*��ѭ�����ڲ��ҿͷ����pR_type1 ���ڱ��� pR_type��ǰ�����*/
        pR_type1 = pR_type;
        pR_type = pR_type->next;
    }
    if(pR_type)/*�ҵ�����ͷ�ʱ*/
    {
        if(pR_type1 == pR_type)/*��������λ�ڱ�ͷ*/
        {/*ֱ�ӽ�����ָ����һ����㣬��ͷ���*/
            *phead = pR_type->next;
        }
        else
        {
            pR_type1->next = pR_type->next;
        }
        /*��¼�������ͽ���µķ�������ͷ��㣬��ɾ��������*/
        pROOM = pR_type->rnext;

        while(pROOM)
        {/*�˶���ѭ������ɾ��������µķ����Լ�������Ϣ*/
            pROOM_after = pROOM->next;
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                pGUEST_after = pGUEST->next;
                free(pGUEST);
                pGUEST = pGUEST_after;
            }
            free(pROOM);
            pROOM = pROOM_after;
        }
           free(pR_type);
           return TRUE;
    }
    else
    {
        printf("\n�����ڸ����Ŀͷ�");
        return FALSE;
    }
}

/**
 * ��������: maintain_ROOM
 * ��������: ѡ��ά���ͷ���Ϣģ�����ģ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_ROOM(R_type **phead)
{
    void (*pFunction[3])();
    pFunction[0] = maintain_rinfo_add;
    pFunction[1] = maintain_rinfo_modify;
    pFunction[2] = maintain_rinfo_delete;
    int option;

    fflush(stdin);
    printf("1.��ӿͷ���Ϣ\n2.�޸Ŀͷ���Ϣ\n3.ɾ���ͷ���Ϣ\n");
    scanf("%d",&option);

    if(option > 0 && option < 4 && pFunction[option] != NULL)
    {
        (*pFunction[option-1])(phead);
    }
    else
    {
        printf("\n����������ѡ�");
    }

}

/**
 * ��������: maintain_rinfo_add
 * ��������: ¼��ͷ���Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_rinfo_add(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM, *tmp;
    char find = 0;

    tmp = (ROOM *)malloc(sizeof(ROOM));

    fflush(stdin);
    printf("\n������ͷ����:");
    scanf("%d",&tmp->r_num);
    tmp->r_tel = 8000 + tmp->r_num;
    getchar();
    printf("�ͷ����:");
    scanf("%c",&tmp->r_type);

    if(tmp->r_type>'Z')
    {
        tmp->r_type+='A'-'a';
    }
    printf("�ͷ����:");
    scanf("%d",&tmp->r_area);
    printf("ÿ�����:");
    scanf("%f",&tmp->rent);
    getchar();
    printf("�Ƿ�����̨(��1/��0):");
    scanf("%c",&tmp->b_flag);
    getchar();
    tmp->g_flag = 0;
    tmp->gnext = NULL;

    pR_type = *phead;

    /*�˶���ѭ�����ڲ��Ҹÿͷ���ŵĿͷ�*/
    while(pR_type && !find)
    {
        pROOM = pR_type->rnext;
        while(pROOM && !find)
        {
            if(pROOM->r_num == tmp->r_num)
            {
                find = 1;break;
            }
            pROOM = pROOM->next;
        }

        if(find)
        {
            break;
        }
        pR_type = pR_type->next;
    }

    if(find)
    {/*������ҵ�*/
        printf("\n�Ѿ����ڸñ�ŵĿͷ���Ϣ");
    }
    else
    {/*���û�ҵ��ÿͷ���ŵĿͷ�*/
        /*��ѭ�����ڲ��Ҹÿͷ������*/
         pR_type = *phead;
        while(pR_type && pR_type->type != tmp->r_type)
            pR_type = pR_type->next;

        if(!pR_type)
        {/*���û���ҵ������*/
            printf("\n�����ڸ����Ŀͷ�");
            free(tmp);
        }
        else
        {/*����ҵ����ò���ķ�ʽ��ӽ�㣬���޸������Ϣ*/
            tmp->next = pR_type->rnext;
            pR_type->rnext = tmp;
            pR_type->room_amount++;
            pR_type->room_vacuum++;

            pROOM = pR_type->rnext;
            printf("\n�ͷ���Ϣ¼��ɹ�!");
            if(pROOM)
            {
                printf("\n¼���%c��ͷ���:\n",pR_type->type);
                while(pROOM)
                {
                    printf("\n�ͷ����:%4d",pROOM->r_num);
                    pROOM = pROOM->next;
                }
            }
        }
    }

}

/**
 * ��������: maintain_rinfo_delete
 * ��������: ɾ���ͷ���Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: BOOL�ͣ��ҵ��ͷ���ɾ��ʱ����TRUE
 *           δ�ҵ��򷵻�FALSE
 * ����˵��:
 */
BOOL maintain_rinfo_delete(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM, *pROOM1;
    GUEST *pGUEST, *pGUEST_after;
    short num;
    char find = 0;
    printf("\n������ͷ����:");
    scanf("%d",&num);
    pR_type = *phead;

    /*��ѭ�����ڲ��ҿͷ���pROOM1 ���ڱ��� pROOM ��ǰ�����*/
    while(pR_type && !find)
    {
        pROOM1 = pROOM = pR_type->rnext;
        while(pROOM && !find)
        {
            if(pROOM->r_num == num){find=1;break;}
            pROOM1 = pROOM;
            pROOM = pROOM->next;
        }
        if(find)break;
        pR_type = pR_type->next;
    }

    if(find)
    {/*������ҵ��ÿͷ�*/

        if(pROOM1 == pROOM)/*����ÿͷ�λ����������¿ͷ�����ı�ͷ*/
            pR_type->rnext = pROOM->next;
        else
            pROOM1->next = pROOM->next;


        pGUEST = pROOM->gnext;
        while(pGUEST)
        {/*��ѭ������ɾ���ͷ��ڵĿ�����Ϣ*/
            pGUEST_after = pGUEST->next;
            free(pGUEST);
            pGUEST = pGUEST_after;
        }

        /*�޸������Ϣ*/
        pR_type->room_amount--;
        if(pROOM->g_flag == 0)pR_type->room_vacuum--;

        free(pROOM);
        return TRUE;
    }
    else
    {
        printf("\n�����ڸñ�ŵĿͷ�");
        return FALSE;
    }
}

/**
 * ��������: maintain_rtype_modify
 * ��������: �޸Ŀͷ�������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_rinfo_modify(R_type **phead)
{
    if(maintain_rinfo_delete(phead) == TRUE)
    {
    printf("\n�޸ĺ����Ϣ:");
    maintain_rinfo_add(phead);
    }
}

/**
 * ��������: maintain_GUEST
 * ��������: ѡ��ά��������Ϣģ�����ģ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_GUEST(R_type **phead)
{
    void (*pFunction[4])();
    pFunction[0] = maintain_ginfo_add;
    pFunction[1] = maintain_ginfo_modify;
    pFunction[2] = maintain_ginfo_delete;
    pFunction[3] = maintain_ginfo_money;
    int option;

    fflush(stdin);
    printf("1.��ӿ�����Ϣ\n2.�޸Ŀ�����Ϣ\n3.ɾ��������Ϣ\n4.�޸Ŀ��˽ɷ���Ϣ\n");
    scanf("%d",&option);

    if(option > 0 && option < 5)
    {
        (*pFunction[option-1])(phead);
    }
    else
    {
        printf("\n����������ѡ�");
    }

    return TRUE;
}

/**
 * ��������: maintain_ginfo_add
 * ��������: ¼�������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_ginfo_add(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *tmp, *pGUEST;
    char flag, find = 0;

    tmp = (GUEST *)malloc(sizeof(GUEST));
    printf("\n��������˵Ļ�����Ϣ:");
    fflush(stdin);
    printf("\n���֤��:");
    scanf("%s",&tmp->g_id);
    printf("����:");
    scanf("%s",&tmp->g_name);
    printf("��ס�ͷ����:");
    scanf("%d",&tmp->r_num);
    printf("��סʱ��(yyyy/mm/dd-hh:mm):");
    scanf("%s",&tmp->in_date);
    getchar();
    printf("�Ƿ���ס(y/n):");
    scanf("%c",&flag);
    printf("ʵ�ʽɷ�:");
    scanf("%f",&tmp->pay_a);
    if(flag == 'y' || flag == 'Y')
    {
        *(tmp->out_date) = '\0';
        tmp->in_months = 0;
    }
    else
    {
        printf("\n�˷�ʱ��(yyyy/mm/dd-hh:mm):");
        scanf("%s",&tmp->out_date);
        tmp->in_months = months(tmp->in_date, tmp->out_date);
    }

    pR_type = *phead;
    while(pR_type && !find)
    {/*�˶���ѭ�����ڲ��ҿ������ڵĿͷ�*/
        pROOM = pR_type->rnext;
        while(pROOM && !find)
        {
            if(pROOM->r_num == tmp->r_num)
            {
                find=1;
                break;
            }
            pROOM = pROOM->next;
        }

        if(find)
        {/*����ҵ�������ѭ��*/
            break;
        }
        pR_type = pR_type->next;
    }
    if(find)
    {
        if(*tmp->out_date == '\0')
        {/*�˷�ʱ��Ϊ�մ���˵����ס*/
            if(pROOM->g_flag == pR_type->bed_amount)
            {
                printf("�÷����Ѵﵽ�����ס����");
                return;
            }
            else
            {
                tmp->next = pROOM->gnext;
                tmp->pay_s = tmp->in_months * pROOM->rent;
                pROOM->gnext = tmp;
                if(pROOM->g_flag == 0)
                {/*�����ס�ķ���Ϊ�շ�*/
                    pR_type->room_vacuum--;
                }

                pROOM->g_flag++;
            }
        }
        else
        {
                tmp->next = pROOM->gnext;
                tmp->pay_s = tmp->in_months*pROOM->rent;
                pROOM->gnext = tmp;
        }

        printf("\n¼�������Ϣ�ɹ�!");

        pGUEST = pROOM->gnext;
        if(pGUEST)
        {
            printf("\n%d��¼��Ŀ�����:",pROOM->r_num);
            while(pGUEST)
            {
                printf("\n����:%-10s���֤��:%20s",pGUEST->g_name,pGUEST->g_id);
                pGUEST = pGUEST->next;
            }
        }

    }

    else
    {
        printf("\n�ÿ������ڿͷ��ı�Ų����ڣ�");
        free(tmp);
    }
}

/**
 * ��������: maintain_ginfo_delete
 * ��������: ɾ��������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
BOOL maintain_ginfo_delete(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST, *pGUEST1;
    char id[20], find = 0;
    printf("\n��������˵����֤��:");
    fflush(stdin);
    scanf("%s",id);
    pR_type = *phead;
    while(pR_type && !find)
    {/*������ѭ�����ڲ���Ҫɾ���Ŀ�����Ϣ���*/
        pROOM = pR_type->rnext;
        while(pROOM && !find)
        {
            pGUEST1 = pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                if(strcmp(pGUEST->g_id,id) == 0){find=1;break;}
                pGUEST1 = pGUEST;
                pGUEST = pGUEST->next;
            }
            if(find)
            {
                break;
            }
            pROOM = pROOM->next;
        }

        if(find)
        {
            break;
        }
        pR_type = pR_type->next;
    }

    if(find)
    {/*����ҵ�Ҫɾ���Ŀ�����Ϣ���*/
        if(pGUEST1 == pGUEST)   /*�ý��λ�ڱ�ͷ*/
        {
            pROOM->gnext = pGUEST->next;
        }
        else
        {
            pGUEST1->next = pGUEST->next;
        }

        if(pGUEST->in_months == 0)
        {
            pROOM->g_flag--;
            if(pROOM->g_flag == 0)
            {
                pR_type->room_vacuum++;
            }
        }

        free(pGUEST);
        return TRUE;

    }
    else
    {
        printf("\n�����ڸ����֤����Ŀ���");
        return FALSE;
    }

}

void maintain_ginfo_modify(R_type **phead)
{
   if(maintain_ginfo_delete(phead) == TRUE)
   {
    printf("\n�޸ĺ����Ϣ:");
    maintain_ginfo_add(phead);
   }
}

/**
 * ��������: maintain_ginfo_money
 * ��������: �޸Ŀ��˽ɷ���Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void maintain_ginfo_money(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    char id[20], find = 0;
    char date[12];
    float money;

    printf("\n��������˵����֤��:");
    fflush(stdin);
    scanf("%s",id);
    pR_type = *phead;
    while(pR_type && !find)
    {
        pROOM = pR_type->rnext;
        while(pROOM && !find)
        {
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                if(strcmp(pGUEST->g_id,id) == 0){find=1;break;}
                pGUEST = pGUEST->next;
            }
            if(find)break;
            pROOM = pROOM->next;
        }
        if(find)break;
        pR_type = pR_type->next;
    }
    if(find)
    {
        printf("\n������ɷ�ʱ��(yyyy/mm/dd):");
        scanf("%s",date);

        if(*(pGUEST->out_date) == '\0')
        {
            pGUEST->pay_s = pROOM->rent * months(pGUEST->in_date, date);
            printf("\n����Ӧ�ɷ���:%f",pGUEST->pay_s);
            printf("\n�����ѽɷ���:%f",pGUEST->pay_a);
            printf("\n��������˸��½ɷ�:");
            scanf("%f",&money);
            pGUEST->pay_a += money;
        }

        else
        {
            pGUEST->pay_s = pROOM->rent * months(pGUEST->in_date, pGUEST->out_date);
            printf("\n����Ӧ�ɷ���:%f",pGUEST->pay_s);
            printf("\n�����ѽɷ���:%f",pGUEST->pay_a);
            printf("\n�ÿ������˷������˱���ʵ�ʽɷѣ�");
            scanf("%f",&money);
            pGUEST->pay_a += money;
        }
    }
    else
    {
        printf("\n�����ڸÿ���");
    }
}

/**
 * ��������: search_R_type
 * ��������: ��ѯ�ͷ�������Ϣ����ģ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
BOOL search_R_type(R_type **phead)
{
    search_rtype(phead);
    return TRUE;
}

/**
 * ��������: search_rtype
 * ��������: ��ѯ�ͷ�������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
R_type* search_rtype(R_type **phead)
{
    R_type *pR_type;
    pR_type = *phead;
    char type;
    printf("\n������ͷ����:");
    fflush(stdin);
    scanf("%c",&type);
    if(type>'Z')type += 'A'-'a';

    while((pR_type != NULL) && (pR_type->type != type))
        pR_type = pR_type->next;

    if(pR_type)
    {
        printf("\n%c���Ŀͷ���Ϣ����:",pR_type->type);
        printf("\n�����ס����:%d",pR_type->bed_amount);
        printf("\n�ͷ�����:%d",pR_type->room_amount);
        printf("\n�շ�����:%d",pR_type->room_vacuum);
        return pR_type;
    }
    else
    {
        printf("\n�����ڸ����Ŀͷ�");
        return NULL;
    }
}

/**
 * ��������: search_ROOM
 * ��������: ��ѯ�ͷ���Ϣ����ģ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void search_ROOM(R_type **phead)
{
    char option;
    fflush(stdin);
    printf("1.���ͷ���Ų�ѯ\n2.���ͷ�������Ϣ��ѯ\n");
    scanf("%d",&option);

    if(option == 1)
    {
        search_rnum(phead);
    }
    else if(option == 2)
    {
        search_rtype_plus(phead);
    }
    else
        printf("\n����������ѡ�");

}

/**
 * ��������: search_rnum
 * ��������: ���ͷ���Ų�ѯ�ͷ���Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
ROOM* search_rnum(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    char find = 0;
    short searchnum;
    printf("\n������ͷ��ĺ���:");
    fflush(stdin);
    scanf("%d",&searchnum);
    pR_type = *phead;
    while(pR_type && !find)
    {
        pROOM = pR_type->rnext;
        while(pROOM && !find)
        {
            if(pROOM->r_num == searchnum){find=1;break;}
            else pROOM = pROOM->next;
        }
        if(find)
        {
            break;
        }

        pR_type = pR_type->next;
    }
    if(find)
    {
        printf("%d����Ϣ����:",pROOM->r_num);
        printf("\n�ͷ��绰  :%d",pROOM->r_tel);
        printf("\n�ͷ����  :%c",pROOM->r_type);
        printf("\n�ͷ����  :%d",pROOM->r_area);
        printf("\nÿ�����  :%f",pROOM->rent);
        printf("\n������̨(1��/0��):%c",pROOM->b_flag);
        printf("\n�Ƿ�������ס:");
        if(pROOM->g_flag != 0)
        {
            printf("��");
        }
        else
        {
            printf("��");
        }
        return pROOM;
    }
    else
    {
        printf("\n�����ڸñ�ŵĿͷ�");
        return NULL;
    }
}

/**
 * ��������: search_rtype_plus
 * ��������: ���ͷ����ͼ�����ѯ�ͷ�����
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void search_rtype_plus(R_type **phead)
{
    R_type *pR_type=*phead;
    ROOM *pROOM;
    char type;
    float rent;
    float epsilon;
    char find = 0;
    printf("\n�ͷ�����:");
    fflush(stdin);
    scanf("%c",&type);
    printf("ÿ�����:");
    scanf("%f",&rent);
    printf("�ɽ��ܵĸ�����Χ:");
    scanf("%f",&epsilon);
    printf("����Ҫ��Ŀͷ�:\n");
    printf("%-10s%-20s%-20s%-10s%-10s","�ͷ����","��������","������̨(1��/0��)","�ͷ����","ÿ�����");
    while((pR_type != NULL)&&(pR_type->type != type))
    {
        pR_type = pR_type->next;
    }

        pROOM = pR_type->rnext;
        while(pROOM != NULL)
        {
            if((rent-epsilon>pROOM->rent) || (rent+epsilon<pROOM->rent))
            {
                pROOM = pROOM->next;
                continue;
            }

            printf("\n%-10d%-20d%-20c%-10d%-10f",pROOM->r_num,pROOM->g_flag,pROOM->b_flag,pROOM->r_area,pROOM->rent);
            find = 1;
            pROOM = pROOM->next;
        }

     if(!find)
    {
        printf("\n\n�����������Ŀͷ�");
    }
}

/**
 * ��������: search_GUEST
 * ��������: ��ѯ������Ϣ����ģ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void search_GUEST(R_type **phead)
{
    char option;
    fflush(stdin);
    printf("1.���������֤�Ų�ѯ\n2.�����˻�����Ϣ��ѯ\n");
    scanf("%d",&option);

    if(option == 1)
    {
        search_gID(phead);
    }
    else if(option == 2)
    {
        search_ginfo(phead);
    }
    else
        printf("\n����������ѡ�");

    return TRUE;
}

/**
 * ��������: search_gID
 * ��������: ���������֤��ѯ�ͷ���Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
GUEST* search_gID(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    char id[20],find=0;
    printf("\n��������˵����֤��:");
    fflush(stdin);
    scanf("%s",id);
    pR_type = *phead;
    while(pR_type && !find)
    {
        pROOM = pR_type->rnext;
        while(pROOM && !find)
        {
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                if(strcmp(pGUEST->g_id,id) == 0){find=1;break;}
                pGUEST = pGUEST->next;
            }
            if(find)break;
            pROOM = pROOM->next;
        }
        if(find)break;
        pR_type = pR_type->next;
    }
    if(find)
    {
        printf("\n���֤��Ϊ%s�Ŀ�����Ϣ����:",pGUEST->g_id);
        printf("\n���ڿͷ�:%d",pROOM->r_num);
        printf("\n����    :%s",pGUEST->g_name);
        printf("\n�������:%d",pGUEST->r_num);
        printf("\nѺ��    :%f",pROOM->rent);
        printf("\n��סʱ��:%s",pGUEST->in_date);
        printf("\nʵ�ʽɷ�:%f",pGUEST->pay_a);
        if(*(pGUEST->out_date) != '\0')
        {
            printf("\n�˷�ʱ��:%s",pGUEST->out_date);
            printf("\n��ס����:%d",pGUEST->in_months);
            printf("\nӦ�ɷ���:%.2f",pGUEST->pay_s);
        }
        return pGUEST;
    }
    else
    {
        printf("\n�����ڸ����֤�ŵĿ���");
        return NULL;
    }
}

/**
 * ��������: search_ginfo
 * ��������: �����˻�����Ϣ��ѯ������Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void search_ginfo(R_type **phead)
{
    char begin[18],over[18],name[20],choice;
    char match,flag,i,find=0;

    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;

    printf("\n��ѡ���Կ��˵��ջ�����������(1��/2��):");
    fflush(stdin);
    scanf("%d",&choice);
    printf("�����Ĺؼ���:");
    scanf("%s",name);
    getchar();
    printf("��סʱ�䷶Χ���:(yyyy/mm/dd):");
    scanf("%s",begin);
    getchar();
    printf("��סʱ�䷶Χ�յ�(yyyy/mm/dd):");
    scanf("%s",over);
    getchar();
    pR_type=*phead;
    printf("���������Ŀ�����:");
    while(pR_type)
    {
        pROOM = pR_type->rnext;
        while(pROOM)
        {
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                if(choice == 1)
                {
                    if((*name == *pGUEST->g_name) && (*(name+1)==*(pGUEST->g_name+1)))flag=1;
                    else flag=0;
                }
                if(choice == 2)
                {
                    flag = 1;
                    for(i=0; *(name+i)!='\0'; i++)
                    {
                        if(*(name+i) != *(pGUEST->g_name+i+2)){flag=0;break;}
                    }
                }

                if(flag && months(begin,pGUEST->in_date)>=0 && months(pGUEST->in_date,over)>=0)
                {
                    find=1;
                    printf("\n%s    %s\n",pGUEST->g_name,pGUEST->g_id);
                }
                pGUEST = pGUEST->next;
            }
            pROOM = pROOM->next;
        }
        pR_type = pR_type->next;
    }
    if(!find)printf("\n��");
}

/**
 * ��������: statistic_1
 * ��������: ͳ�Ƹ���ͷ���Ŀ��Ϣ
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void statistic_1(R_type **phead)
{
    R_type *pR_type;
    pR_type = *phead;

    while(pR_type)
    {
        printf("\n�ͷ�����:%c",pR_type->type);
        printf("\n�ͷ�����:%d",pR_type->room_amount);
        printf("\n��ס�ͷ���:%d",pR_type->room_amount-pR_type->room_vacuum);
        printf("\nδס�ͷ���:%d\n",pR_type->room_vacuum);
        pR_type=pR_type->next;
    }

}

/**
 * ��������: statistic_2
 * ��������:ͳ�Ƹ�����ݵ����пͷ�ÿ��Ӫҵ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void statistic_2(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    float income[4][13]={0},amount[4]={0};
    char type_num,end_of_month[13][7]={"/0 ","/01/31","/02/29","/03/31","/04/30","/05/31","/06/30","/07/31","/08/31","/09/30","/10/31","/11/30","/12/31"};
    char year[5],check_day[20];
    char flag[13]={0};
    char i1,i,j;
    printf("\n������Ҫͳ�Ƶ����:");
    fflush(stdin);
    scanf("%s",year);

    ClearScreen();

    printf("\n���:%s%20c%12s",year,' ',"��λ:Ԫ�����");
    pR_type=*phead;
    type_num=0;
    printf("\n�·�");
    while(pR_type)
    {
        printf("%12c",pR_type->type);
        pROOM=pR_type->rnext;
        while(pROOM)
        {
            memset(flag,0,13);
            pGUEST=pROOM->gnext;
            while(pGUEST)
            {
                for(i1=1;i1<=12;i1++)
                {
                    strcpy(check_day,year);
                    strcat(check_day,end_of_month[i1]);
                    if(months(pGUEST->in_date,check_day)>0 && (months(check_day,pGUEST->out_date)>0 || *(pGUEST->out_date) == '\0'))
                    {/*�����סʱ�����ڼ��㣬���ڼ���ʱ��ס���˷�ʱ�����ڼ����״̬Ϊ��ס��*/
                        flag[i1] = 1;
                        /*��Ǵ��·�*/
                    }
                }

                pGUEST=pGUEST->next;
            }
            for(i1=1;i1<=12;i1++)
            {
                income[type_num][i1]+=flag[i1]*pROOM->rent;
            }
            pROOM=pROOM->next;
        }
        type_num++;
        pR_type=pR_type->next;
    }

    for(i=1;i<=12;i++)
    {
        printf("\n%4d",i);
        for(j=0;j<type_num;j++)
        {
            printf("%12.2f",amount[j]+=income[j][i]);
        }
    }
    printf("\n�ϼ�");
    for(j=0;j<type_num;j++)
        printf("%12.2f",amount[j]);
}

/**
 * ��������: statistic_3
 * ��������: ���пͷ���ס�ʵ���Ϣ����ͳ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void statistic_3(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    char type_num, end_of_month[13][7]={"/0 ","/01/31","/02/29","/03/31","/04/30","/05/31","/06/30","/07/31","/08/31","/09/30","/10/31","/11/30","/12/31"};
    char year[5], check_day[20];
    char flag[13] = {0};
    char i1;

    int MONTHS = 0;
    float income = 0, NUM = 0;

    printf("\n������Ҫͳ�Ƶ����:");
    fflush(stdin);
    scanf("%s",year);

    ClearScreen();

    printf("\n���:%s%20c%12s",year,' ',"��λ:Ԫ�����");
    pR_type = *phead;
    type_num = 0;
    printf("\n%8s%12s%12s%12s%12s","�ͷ����","���","Ӫҵ��","��ס����","��ס��");
    while(pR_type)
    {
        pROOM = pR_type->rnext;
        while(pROOM)
        {
            memset(flag,0,13);
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                for(i1=1; i1<=12; i1++)
                {
                    strcpy(check_day,year);
                    strcat(check_day,end_of_month[i1]);
                    if((months(pGUEST->in_date,check_day)>0)&&((months(check_day,pGUEST->out_date)>0 || *(pGUEST->out_date)=='\0')))
                    {/*�����סʱ��������ʱ��֮ǰ����û���˷����˷�ʱ��������ʱ��֮�����ס��*/
                        if(flag[i1] == 0)
                        {
                            flag[0]++;
                            flag[i1] = 1;
                        }
                    }
                }

                pGUEST=pGUEST->next;
            }
            printf("\n%8d%12c%12.2f%12d%10d/12",pROOM->r_num,pROOM->r_type,pROOM->rent**flag,*flag,*flag);
            MONTHS += *flag;
            NUM++;
            income += pROOM->rent * *flag;
            pROOM = pROOM->next;
        }
        pR_type = pR_type->next;
    }
    printf("\n%8s%12c%12.2f%12d%12.2f%%\n","�ϼ�",' ',income,MONTHS,(MONTHS/(NUM*12))*100);


}

/**
 * ��������: statistic_4
 * ��������: �ۼ��ⷿ��������10������
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void statistic_4(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST, *rank[11]={NULL}, *rtmp;
    int a[11]={0}, atmp;
    char now[12], i;

    printf("\n�����뵱ǰʱ��(yyyy/mm/dd):");
    scanf("%s",now);

    pR_type = *phead;
    while(pR_type)
    {
        pROOM = pR_type->rnext;
        while(pROOM)
        {
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                a[10] = pGUEST->in_months?pGUEST->in_months:months(pGUEST->in_date,now);
                rank[10] = pGUEST;
                pGUEST->pay_s = a[10]*pROOM->rent;
                for(i=10; i>=0; i--)
                {
                    if(a[i-1]<a[i])
                    {
                        atmp = a[i];
                        a[i] = a[i-1];
                        a[i-1] = atmp;

                        rtmp = rank[i];
                        rank[i] = rank[i-1];
                        rank[i-1] = rtmp;
                    }
                }
                pGUEST = pGUEST->next;
            }
            pROOM = pROOM->next;
        }
        pR_type = pR_type->next;
    }

    ClearScreen();
    printf("%-20s%-12s%-14s%-12s%-14s","���֤��","����","�ۼ�ס������","Ӧ���ܶ�","ʵ�ʽɷ��ܶ�");
    for(i=0;i<10;i++)
    {
        if(a[i])
        {
            printf("\n%-20s%-12s%-14d%-12.2f%-14.2f\n",rank[i]->g_id,rank[i]->g_name,a[i],rank[i]->pay_s,rank[i]->pay_a);
        }
    }


}

/**
 * ��������: statistic_5
 * ��������: ���˽ɷ�Ƿ����Ϣͳ��
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void statistic_5(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    pR_type=*phead;

    char now[12];

     printf("\n�����뵱ǰʱ��(yyyy/mm/dd):");
    scanf("%s",now);

    printf("\n%-10s%-20s%-10s%-12s%-12s%-12s%-12s\n","����","���֤��","�Ƿ���ס","�ͷ�����","Ӧ�ɷ���","ʵ�ʽɷ�","Ƿ��");
    while(pR_type)
    {
        pROOM = pR_type->rnext;
        while(pROOM)
        {
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                if(pGUEST->in_months == 0)
                {
                    pGUEST->in_months = months(pGUEST->in_date,now);
                    pGUEST->pay_s = pROOM->rent * pGUEST->in_months;
                }

                printf("\n%-10s%-20s",pGUEST->g_name,pGUEST->g_id);
                if(*(pGUEST->out_date) == '\0')
                {
                    printf("%-10s","  ��");
                }
                else
                {
                    printf("%-10s","  ��");
                }
                printf("%-12d%-12.2f%-12.2f%-12.2f\n",pGUEST->r_num,pGUEST->pay_s,pGUEST->pay_a,pGUEST->pay_s-pGUEST->pay_a);

                pGUEST=pGUEST->next;
            }
            pROOM = pROOM->next;
        }
        pR_type = pR_type->next;
    }

}

/**
 * ��������: SaveData
 * ��������: ����ϵͳ����
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SaveData(R_type **phead)
{

    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    FILE *pFile1, *pFile2, *pFile3;

    if((pFile1 = fopen("RoomType.dat","wb")) == NULL)
    {
        printf("\n�ͷ�����ļ���ʧ��");
        exit(-1);
    }

    if((pFile2 = fopen("RoomInfo.dat","wb")) == NULL)
    {
        printf("\n�ͷ�������Ϣ�ļ���ʧ��");
        exit(-1);
    }

    if((pFile3 = fopen("GuestInfo.dat","wb")) == NULL)
    {
        printf("\n������Ϣ�ļ���ʧ��");
        exit(-1);
    }

    pR_type = *phead;
    while(pR_type)
    {
        pROOM = pR_type->rnext;
        while(pROOM)
        {
            pGUEST = pROOM->gnext;
            while(pGUEST)
            {
                fwrite(pGUEST,sizeof(GUEST),1,pFile3);
                pGUEST = pGUEST->next;
            }
            fwrite(pROOM,sizeof(ROOM),1,pFile2);
            pROOM = pROOM->next;
        }
        fwrite(pR_type,sizeof(R_type),1,pFile1);
        pR_type = pR_type->next;
    }
    fclose(pFile1);
    fclose(pFile2);
    fclose(pFile3);

}

/**
 * ��������: ExitSys
 * ��������: �˳�ϵͳ����ѡ���Ƿ񱣴�
 * �������: ����ͷָ���ַphead
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ExitSys(R_type **phead)
{
    char save;
    printf("�Ƿ񱣴��޸ģ�(y/n)\n");
    fflush(stdin);
    scanf("%s",&save);

    if(save=='y'||save=='Y')
    {
      SaveData(phead);
    }

    over = 1;
}

/**
 * ��������: months
 * ��������: ����ʽ��������ʱ���֮�������
 * �������: �ַ���in,out
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
int months(char *in,char *out)
{
    int yearin = 0,yearout = 0,monthin = 0,monthout = 0,dayin, dayout, i;
    if(*(out) == '\0')
        return 0;
    else
    {
        for(i=0; i<4; i++)
        {
            yearin = yearin * 10 + (*(in+i) - '0');
            yearout = yearout * 10 + (*(out+i) - '0');
        }
        monthin = (*(in+5) - '0') * 10 + (*(in+6) - '0');
        monthout = (*(out+5) - '0') * 10 + (*(out+6) - '0');
        dayin = (*(in+8) - '0') * 10 + (*(in+9) - '0');
        dayout = (*(out+8) - '0') * 10 + (*(out+9) - '0');
        if(dayin-dayout<0)monthout++;
        return (yearout-yearin)*12+monthout-monthin;

    }
}
