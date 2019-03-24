#include "room.h"
char over = 0;/*RunSys中用于判断是否结束运行的变量,只有运行ExitSys时才更改为1*/

unsigned long ul;

int main()
{
    R_type **phead;
    COORD size = {SCR_COL , SCR_ROW};           /*窗口缓冲区大小*/
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE);/*获取标准输出设备句柄*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);  /*获取标准输入设备句柄*/

    SetConsoleTitle("房屋出租管理系统");         /*设置窗口标题*/
    SetConsoleScreenBufferSize(gh_std_out , size);/*设置窗口缓冲区大小*/
    SetConsoleTextAttribute(gh_std_out , att);    /*设置白色前景和黑色背景*/
    LoadFile(phead);
    ClearWindow();                                /*清屏*/

    /*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
    gp_scr_att = (char*)malloc(SCR_COL*SCR_ROW*sizeof(char));/*窗口字符属性*/
    gp_top_layer = (LAYER*)malloc(sizeof(LAYER));
    gp_top_layer->LayerNum = 0;                              /*弹出窗口层号为0*/
    gp_top_layer->rcArea.Left = 0;
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL-1;
    gp_top_layer->rcArea.Bottom = SCR_ROW-1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();         /*显示系统主菜单*/
    RunSys(phead);

    return 0;
}

/**
 * 函数名称: LoadFile
 * 函数功能: 将三类基础数据从数据文件载入到内存缓冲区和十字链表中.
 * 输入参数: phead 主链头指针的地址, 用来保存所创建的十字链表地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 该功能用于运行主要功能之前的数据加载，如果打开文件失败
 *           则不会运行系统，并输出提示信息。
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
         printf("\n客房类别文件打开失败");
         exit(-1);
    }

    while(fread(&tmp1,sizeof(R_type),1,pFile) == 1)
    {/*读入客房类别信息并加入链表*/
        pR_type = (R_type *)malloc(sizeof(R_type));
        *pR_type = tmp1;        //对指针本身内容进行修改，所以需要二级指针作为入口参数
        pR_type->rnext = NULL;  //初始房间类型结点下的房间信息链表为空
        pR_type->next = head;
        head = pR_type;
    }
    if(head == NULL)
    {/*头指针为空指针说明未加入类别信息，调用录入类别功能*/
        printf("\n还未添加任何客房类别信息！请添加客房类别信息:");
        maintain_rtype_add(&head);
    }
    *phead = head;
    fclose(pFile);

    if((pFile = fopen("RoomInfo.dat","rb")) == NULL)
    {
        printf("\n客房基本信息文件打开失败");
        exit(-1);
    }
    while(fread(&tmp2,sizeof(ROOM),1,pFile) == 1)
    {/*读入房间信息并加入链表*/
        pROOM = (ROOM *)malloc(sizeof(ROOM));
        *pROOM = tmp2;
        pROOM->gnext = NULL; //初始化房间信息结点下的客人链表为空

        pR_type = head;
        while((pR_type != NULL)&&(pR_type->type != pROOM->r_type))
        {/*顺序遍历链表，查找该客房的客房类型结点*/
            pR_type = pR_type->next;
        }
        if(pR_type)
        {/*如果找到该类别*/
            pROOM->next = pR_type->rnext;
            pR_type->rnext = pROOM;
        }
        else
        {/*如果未找到则释放存储该房间信息的内存*/
            free(pROOM);
        }
    }
    fclose(pFile);

    if((pFile = fopen("GuestInfo.dat","rb")) == NULL)
    {
        printf("\n客人信息文件打开失败");
        exit(-1);
    }
    while(fread(&tmp3,sizeof(GUEST),1,pFile) == 1)
    {/*读入客人信息并加入链表*/
        pGUEST = (GUEST *)malloc(sizeof(GUEST));
        *pGUEST = tmp3;

        pR_type = head;
        find = 0;
        while(pR_type && !find)
        {/*没有查找完所有客房类别而且没有找到时，进行查找的循环*/
            pROOM = pR_type->rnext;
            while(pROOM && !find)
            {/*没有查找完所有客房而且没有找到时，进行查找的循环*/
                if(pGUEST->r_num == pROOM->r_num)
                {/*查找到客人所在客房时，标记find为1，并停止查找*/
                    find = 1;
                    break;
                }
                pROOM = pROOM->next;
            }
            pR_type = pR_type->next;
        }
        if(find)
        {/*将客人信息结点链接到房间的客人链表中*/
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
 * 函数名称: ShowMenu
 * 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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
    for(i = 0; i<4; i++)        /*第一行第一列输出主菜单项*/
    {
        printf("  %s  ", main_menu[i]);
    }

    /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
    buff_menubar_info = (CHAR_INFO*)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X - 1,0};

    /*将第一行内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
    ReadConsoleOutput(gh_std_out, buff_menubar_info , size,pos , &rcMenu);
    /*将这一行字符单元置为白底黑字*/
    for(i=0; i<size.X ; i++)
    {
        (buff_menubar_info+i)->Attributes = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
    }
    /*修改后的字符信息回写到第一行*/
    WriteConsoleOutput(gh_std_out, buff_menubar_info, size, pos, &rcMenu);
    TagMainMenu(gi_sel_menu);/*标记选中主菜单*/
    SetConsoleCursorPosition(gh_std_out, pos2);

    return;
}

/**
 * 函数名称: TagMainMenu
 * 函数功能: 在指定主菜单项上置选中标志.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD size;
    COORD pos={0,0};
    int PosA=2, PosB;
    char ch;
    int i;

    if(num == 0)/*num为0时去除标记*/
    {
        PosA=0;
        PosB=0;
    }

    else    /*否则，定位选中主菜单项的起止位置：PosA为起始位置，PosB为截止位置*/
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

    /*去除前面菜单项的标记*/
    for(i=0; i<PosA; i++)
    {
        (buff_menubar_info+i)->Attributes = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
    }
    /*标记选中菜单项，黑底白字*/
    for(i=PosA; i<PosB; i++)
    {
        (buff_menubar_info+i)->Attributes = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
    }
    /*去除后面菜单项的标记*/
    for(i=PosB; i<size.X; i++)
    {
        (buff_menubar_info+i)->Attributes = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
    }
    /*将做好标记的信息回写到第一行*/
    SMALL_RECT rcMenu = {0,0,size.X-1,0};
    WriteConsoleOutput(gh_std_out,buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: ClearWindow
 * 函数功能: 清楚控制台窗口屏幕信息
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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
 * 函数名称: ClearScreen
 * 函数功能: 清除主菜单条下的屏幕信息.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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
 * 函数名称: PopMenu
 * 函数功能: 弹出指定主菜单项对应的子菜单.
 * 输入参数: num 指定的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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

    if (num != gi_sel_menu)       /*如果指定主菜单不是已选中菜单*/
    {
        if (gp_top_layer->LayerNum != 0) /*如果此前已有子菜单弹出*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNum != 0) /*若已弹出该子菜单，则返回*/
    {
        return;
    }

    gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
    TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
    LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += sub_menu_num[i-1];
    }
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel = sub_menu + loc;   /*标签束第一个标签字符串的地址*/
    labels.num = sub_menu_num[gi_sel_menu-1]; /*标签束中标签字符串的个数*/
    COORD aLoc[labels.num];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    for (i=0; i<labels.num; i++) /*确定标签字符串的输出位置，存放在坐标数组中*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
    SMALL_RECT aArea[areas.num];                    /*定义数组存放所有热区位置*/
    char aSort[areas.num];                      /*定义数组存放所有热区对应类别*/
    char aTag[areas.num];                         /*定义数组存放每个热区的编号*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*热区类别都为0(按钮型)*/
        aTag[i] = i + 1;           /*热区按顺序编号*/
    }
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/
    pos.X = rcPop.Left + 2;

    gi_sel_sub_menu = 0;
    return;
}

/**
 * 函数名称: PopUp
 * 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
 * 输入参数: pRc 弹出窗口位置数据存放的地址
 *           att 弹出窗口区域字符属性
 *           pLabel 弹出窗口中标签束信息存放的地址
             pHotArea 弹出窗口中热区信息存放的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*弹出窗口所在位置字符单元信息入栈*/
    size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
    /*申请存放弹出窗口相关信息的动态存储区*/
    nextLayer = (LAYER *)malloc(sizeof(LAYER));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNum = gp_top_layer->LayerNum + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
    /*设置弹出窗口区域字符的新属性*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*将标签束中的标签字符串在设定的位置输出*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),pLabel->pLoc[i], &ul);
        }
    }
    /*设置弹出窗口区域字符单元的新属性*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环设置字符单元的层号*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNum;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
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
 * 函数名称: PopOff
 * 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopOff(void)
{
    LAYER *nextLayer;
    COORD size;
    COORD pos={0,0};
    char *pCh;
    int i, j;
    if((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {/*顶层为主界面屏幕信息时，不用关闭*/
        return;
    }

    nextLayer = gp_top_layer->next;

    /*恢复弹出窗口覆盖区域原外观*/
    size.X = gp_top_layer->rcArea.Right-gp_top_layer->rcArea.Left+1;
    size.Y = gp_top_layer->rcArea.Bottom-gp_top_layer->rcArea.Top+1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));

    /*恢复字符单元属性*/
    pCh = gp_top_layer->pScrAtt;
    for(i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for(j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);/*释放动态存储区*/
    free(gp_top_layer->pScrAtt);
    gp_top_layer = nextLayer;
    return;
}

/**
 * 函数名称: DrawBox
 * 函数功能: 在指定区域画边框.
 * 输入参数: pRc 存放区域位置信息的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-','|'};
    COORD pos = {pRc->Left,pRc->Top};/*定位在区域左上角*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画左上角*/

    for(pos.X=pRc->Left+1; pos.X<pRc->Right; pos.X++)/*画上边框横线*/
    {
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画右上角*/

    for(pos.Y=pRc->Top+1; pos.Y<pRc->Bottom; pos.Y++)/*画左右边框横线*/
    {
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1,pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }

    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画左下角*/

    for(pos.X=pRc->Left+1; pos.X<pRc->Right; pos.X++)/*画下边框横线*/
    {
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画右下角*/

    return;
}

/**
 * 函数名称: TagSubMenu
 * 函数功能: 在指定子菜单项上做选中标记.
 * 输入参数: num 选中的子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    /*计算弹出子菜单的区域位置，存放在rcPop中*/
    LocSubMenu(gi_sel_menu, &rcPop);
    if((num<1) || (num==gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {/*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
        return;
    }

    pos.X = rcPop.Left+2;
    width = rcPop.Right-rcPop.Left-3;

    if(gi_sel_sub_menu != 0)/*取消原选中子菜单项的标记*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        /*白底黑字*/
        att = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    }

    /*在定制子菜单项上做标记*/
    pos.X = rcPop.Left+2;
    pos.Y = rcPop.Top+num;
    /*黑底白字*/
    att = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;/*修改选中子菜单项号*/
    return;
}

/**
 * 函数名称: LocSubMenu
 * 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: rc 存放区域位置信息的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc=0;
    rc->Top = 1;    /*区域上边定位在第二行，行号为1*/
    rc->Left = 1;
    for(i=1; i<num; i++)
    {/*计算区域左边界位置和第一个子菜单项在子菜单字符串数组中的位置*/
        rc->Left += strlen(main_menu[i-1]) + 4;
        loc += sub_menu_num[i-1];
    }

    rc->Right = strlen(sub_menu[loc]);/*暂存第一个子菜单项字符串长度*/
    for(i=1; i<sub_menu_num[num-1]; i++)
    {/*查找最长子菜单字符串，将其长度存放在rc->Right*/
        len = strlen(sub_menu[loc+i]);
        if(rc->Right < len)
        {
            rc->Right = len;
        }
    }
            rc->Right += rc->Left+3;/*计算区域右边界*/
            /*计算区域下边的行号*/
        rc->Bottom = rc->Top + sub_menu_num[num-1]+1;
        if(rc->Right >= SCR_COL)/*右边界越界处理*/
        {
            len = rc->Right - SCR_COL + 1;
            rc->Left -= len;
            rc->Right = SCR_COL - 1;
        }

    return;
}

/**
 * 函数名称: RunSys
 * 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
 * 输入参数: phead 主链头指针的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void RunSys(R_type **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
    char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    while (over == 0)
    {
        /*从控制台输入缓冲区中读一条记录*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == KEY_EVENT  /*如果记录由按键产生*/
                 && inRec.Event.KeyEvent.bKeyDown) /*且键被按下*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

            if (asc == 0) /*控制键的处理*/
            {
                if (gp_top_layer->LayerNum == 0) /*如果未弹出子菜单*/
                {
                    switch (vkc) /*处理方向键(左、右、下)，不响应其他控制键*/
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
                else  /*已弹出子菜单时*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += sub_menu_num[i-1];
                    }  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                    switch (vkc) /*方向键(左、右、上、下)的处理*/
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
            { /*如果按下左或右Alt键*/
                switch (vkc)  /*判断组合键Alt+字母*/
                {
                    case 88:  /*Alt+X 退出*/
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

            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*按下普通键*/
                if (gp_top_layer->LayerNum == 0)  /*如果未弹出子菜单*/
                {
                    switch (vkc)
                    {
                        case 13: /*回车*/
                            ClearScreen();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*已弹出子菜单时的键盘输入处理*/
                {
                    if (vkc == 27) /*如果按下ESC键*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }

                    else if(vkc == 13) /*如果按下回车键*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        ExeFunction(gi_sel_menu, num, phead);
                    }

                    else /*其他普通键的处理*/
                    {
                        /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += sub_menu_num[i-1];
                        }

                        /*依次与当前子菜单中每一项的代表字符进行比较*/
                        for (i=loc; i<loc+sub_menu_num[gi_sel_menu-1]; i++)
                        {
                            if (strlen(sub_menu[i])>0 && vkc==sub_menu[i][1])
                            { /*如果匹配成功*/
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
 * 函数名称: ExeFunction
 * 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
 * 输入参数: m 主菜单项号
 *           s 子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ExeFunction(int m, int s, R_type **phead)
{
    /*函数指针数组，用来存放所有功能函数的入口地址*/
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
 * 函数名称: maintain_R_type
 * 函数功能: 选择维护客房分类信息模块的子模块
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void maintain_R_type(R_type **phead)
{
    void (*pFunction[3])();
    pFunction[0] = maintain_rtype_add;
    pFunction[1] = maintain_rtype_modify;
    pFunction[2] = maintain_rtype_delete;
    int option;

    fflush(stdin);
    printf("1.添加客房类别信息\n2.修改客房类别信息\n3.删除客房类别信息\n");
    scanf("%d",&option);

    if(option > 0 && option < 4 && pFunction[option] != NULL)
    {
        (*pFunction[option-1])(phead);
    }
    else
    {
        printf("\n请输入合理的选项！");
    }

}

/**
 * 函数名称: maintain_rtype_add
 * 函数功能: 录入客房分类信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void maintain_rtype_add(R_type **phead)
{
    R_type *pR_type, *tmp;

    tmp = (R_type*)malloc(sizeof(R_type));
    tmp->rnext = NULL;

    printf("\n请输入客房类别(S,D,T,F):");
    fflush(stdin);
    scanf("%c",&tmp->type);
    if(tmp->type>'Z')
    {
        tmp->type+='A'-'a';
    }
    getchar();
    /*初始客房类别房间数、空房数为0*/
    tmp->room_amount = 0;
    tmp->room_vacuum = 0;
    printf("客房最多入住人数:");
    scanf("%d",&tmp->bed_amount);

    pR_type = *phead;
    /*此循环用于查找客房的类别*/
    while((pR_type != NULL) && (pR_type->type != tmp->type))
        pR_type = pR_type->next;

    if(pR_type != 0)
    {/*如果查找到该类别*/
        printf("\n已经存在该类客房信息");
        free(tmp);
    }
    else
    {
        tmp->next = *phead;
        *phead = tmp;
        printf("\n类别信息录入成功!");

        pR_type = *phead;
        if(pR_type != NULL)
        {
            printf("\n已录入的客房类别有");
            pR_type = *phead;
            while(pR_type)
            {
                printf("\n类别:%-4c每间房最大容量:%4d人",pR_type->type,pR_type->bed_amount);
                pR_type = pR_type->next;
            }
        }
    }
}

/**
 * 函数名称: maintain_rtype_modify
 * 函数功能: 修改客房分类信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:修改客房类别信息需满足一些限制条件
 * 比如，修改的客房类别必须存在，修改后的客房类别不能与已有类别重复
 * 通过先删除需修改的类别信息，再加入修改后的客房类别信息，可以满足以上约束条件
 */
void maintain_rtype_modify(R_type **phead)
{
    if(maintain_rtype_delete(phead) == TRUE)
    {
        printf("\n修改后的信息:\n");
        maintain_rtype_add(phead);
    }
}

/**
 * 函数名称: maintain_rtype_delete
 * 函数功能: 删除客房分类信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: BOOL型，找到该分类并删除时返回TRUE
 *            未找到则返回FALSE
 * 调用说明:
 */
BOOL maintain_rtype_delete(R_type **phead)
{
    R_type *pR_type, *pR_type1;
    ROOM *pROOM, *pROOM_after;
    GUEST *pGUEST, *pGUEST_after;
    char type;

    printf("\n请输入客房类别:");
    fflush(stdin);
    scanf("%c",&type);
    getchar();
    pR_type = *phead;
    pR_type1 = pR_type;
    while((pR_type != NULL) && (pR_type->type != type))
    {/*此循环用于查找客房类别，pR_type1 用于保存 pR_type的前驱结点*/
        pR_type1 = pR_type;
        pR_type = pR_type->next;
    }
    if(pR_type)/*找到该类客房时*/
    {
        if(pR_type1 == pR_type)/*如果该类别位于表头*/
        {/*直接将链表指向下一个结点，表头结点*/
            *phead = pR_type->next;
        }
        else
        {
            pR_type1->next = pR_type->next;
        }
        /*记录房间类型结点下的房间链表头结点，以删除房间结点*/
        pROOM = pR_type->rnext;

        while(pROOM)
        {/*此二重循环用于删除该类别下的房间以及客人信息*/
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
        printf("\n不存在该类别的客房");
        return FALSE;
    }
}

/**
 * 函数名称: maintain_ROOM
 * 函数功能: 选择维护客房信息模块的子模块
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void maintain_ROOM(R_type **phead)
{
    void (*pFunction[3])();
    pFunction[0] = maintain_rinfo_add;
    pFunction[1] = maintain_rinfo_modify;
    pFunction[2] = maintain_rinfo_delete;
    int option;

    fflush(stdin);
    printf("1.添加客房信息\n2.修改客房信息\n3.删除客房信息\n");
    scanf("%d",&option);

    if(option > 0 && option < 4 && pFunction[option] != NULL)
    {
        (*pFunction[option-1])(phead);
    }
    else
    {
        printf("\n请输入合理的选项！");
    }

}

/**
 * 函数名称: maintain_rinfo_add
 * 函数功能: 录入客房信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void maintain_rinfo_add(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM, *tmp;
    char find = 0;

    tmp = (ROOM *)malloc(sizeof(ROOM));

    fflush(stdin);
    printf("\n请输入客房编号:");
    scanf("%d",&tmp->r_num);
    tmp->r_tel = 8000 + tmp->r_num;
    getchar();
    printf("客房类别:");
    scanf("%c",&tmp->r_type);

    if(tmp->r_type>'Z')
    {
        tmp->r_type+='A'-'a';
    }
    printf("客房面积:");
    scanf("%d",&tmp->r_area);
    printf("每月租金:");
    scanf("%f",&tmp->rent);
    getchar();
    printf("是否有阳台(是1/否0):");
    scanf("%c",&tmp->b_flag);
    getchar();
    tmp->g_flag = 0;
    tmp->gnext = NULL;

    pR_type = *phead;

    /*此二重循环用于查找该客房编号的客房*/
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
    {/*如果查找到*/
        printf("\n已经存在该编号的客房信息");
    }
    else
    {/*如果没找到该客房编号的客房*/
        /*此循环用于查找该客房的类别*/
         pR_type = *phead;
        while(pR_type && pR_type->type != tmp->r_type)
            pR_type = pR_type->next;

        if(!pR_type)
        {/*如果没有找到该类别*/
            printf("\n不存在该类别的客房");
            free(tmp);
        }
        else
        {/*如果找到则用插入的方式添加结点，并修改相关信息*/
            tmp->next = pR_type->rnext;
            pR_type->rnext = tmp;
            pR_type->room_amount++;
            pR_type->room_vacuum++;

            pROOM = pR_type->rnext;
            printf("\n客房信息录入成功!");
            if(pROOM)
            {
                printf("\n录入的%c类客房有:\n",pR_type->type);
                while(pROOM)
                {
                    printf("\n客房编号:%4d",pROOM->r_num);
                    pROOM = pROOM->next;
                }
            }
        }
    }

}

/**
 * 函数名称: maintain_rinfo_delete
 * 函数功能: 删除客房信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: BOOL型，找到客房并删除时返回TRUE
 *           未找到则返回FALSE
 * 调用说明:
 */
BOOL maintain_rinfo_delete(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM, *pROOM1;
    GUEST *pGUEST, *pGUEST_after;
    short num;
    char find = 0;
    printf("\n请输入客房编号:");
    scanf("%d",&num);
    pR_type = *phead;

    /*此循环用于查找客房，pROOM1 用于保存 pROOM 的前驱结点*/
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
    {/*如果查找到该客房*/

        if(pROOM1 == pROOM)/*如果该客房位于类别链表下客房链表的表头*/
            pR_type->rnext = pROOM->next;
        else
            pROOM1->next = pROOM->next;


        pGUEST = pROOM->gnext;
        while(pGUEST)
        {/*此循环用于删除客房内的客人信息*/
            pGUEST_after = pGUEST->next;
            free(pGUEST);
            pGUEST = pGUEST_after;
        }

        /*修改相关信息*/
        pR_type->room_amount--;
        if(pROOM->g_flag == 0)pR_type->room_vacuum--;

        free(pROOM);
        return TRUE;
    }
    else
    {
        printf("\n不存在该编号的客房");
        return FALSE;
    }
}

/**
 * 函数名称: maintain_rtype_modify
 * 函数功能: 修改客房分类信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void maintain_rinfo_modify(R_type **phead)
{
    if(maintain_rinfo_delete(phead) == TRUE)
    {
    printf("\n修改后的信息:");
    maintain_rinfo_add(phead);
    }
}

/**
 * 函数名称: maintain_GUEST
 * 函数功能: 选择维护客人信息模块的子模块
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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
    printf("1.添加客人信息\n2.修改客人信息\n3.删除客人信息\n4.修改客人缴费信息\n");
    scanf("%d",&option);

    if(option > 0 && option < 5)
    {
        (*pFunction[option-1])(phead);
    }
    else
    {
        printf("\n请输入合理的选项！");
    }

    return TRUE;
}

/**
 * 函数名称: maintain_ginfo_add
 * 函数功能: 录入客人信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void maintain_ginfo_add(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *tmp, *pGUEST;
    char flag, find = 0;

    tmp = (GUEST *)malloc(sizeof(GUEST));
    printf("\n请输入客人的基本信息:");
    fflush(stdin);
    printf("\n身份证号:");
    scanf("%s",&tmp->g_id);
    printf("姓名:");
    scanf("%s",&tmp->g_name);
    printf("入住客房编号:");
    scanf("%d",&tmp->r_num);
    printf("入住时间(yyyy/mm/dd-hh:mm):");
    scanf("%s",&tmp->in_date);
    getchar();
    printf("是否在住(y/n):");
    scanf("%c",&flag);
    printf("实际缴费:");
    scanf("%f",&tmp->pay_a);
    if(flag == 'y' || flag == 'Y')
    {
        *(tmp->out_date) = '\0';
        tmp->in_months = 0;
    }
    else
    {
        printf("\n退房时间(yyyy/mm/dd-hh:mm):");
        scanf("%s",&tmp->out_date);
        tmp->in_months = months(tmp->in_date, tmp->out_date);
    }

    pR_type = *phead;
    while(pR_type && !find)
    {/*此二重循环用于查找客人所在的客房*/
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
        {/*如果找到则跳出循环*/
            break;
        }
        pR_type = pR_type->next;
    }
    if(find)
    {
        if(*tmp->out_date == '\0')
        {/*退房时间为空串则说明在住*/
            if(pROOM->g_flag == pR_type->bed_amount)
            {
                printf("该房间已达到最大入住人数");
                return;
            }
            else
            {
                tmp->next = pROOM->gnext;
                tmp->pay_s = tmp->in_months * pROOM->rent;
                pROOM->gnext = tmp;
                if(pROOM->g_flag == 0)
                {/*如果入住的房间为空房*/
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

        printf("\n录入客人信息成功!");

        pGUEST = pROOM->gnext;
        if(pGUEST)
        {
            printf("\n%d房录入的客人有:",pROOM->r_num);
            while(pGUEST)
            {
                printf("\n姓名:%-10s身份证号:%20s",pGUEST->g_name,pGUEST->g_id);
                pGUEST = pGUEST->next;
            }
        }

    }

    else
    {
        printf("\n该客人所在客房的编号不存在！");
        free(tmp);
    }
}

/**
 * 函数名称: maintain_ginfo_delete
 * 函数功能: 删除客人信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
BOOL maintain_ginfo_delete(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST, *pGUEST1;
    char id[20], find = 0;
    printf("\n请输入客人的身份证号:");
    fflush(stdin);
    scanf("%s",id);
    pR_type = *phead;
    while(pR_type && !find)
    {/*此三重循环用于查找要删除的客人信息结点*/
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
    {/*如果找到要删除的客人信息结点*/
        if(pGUEST1 == pGUEST)   /*该结点位于表头*/
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
        printf("\n不存在该身份证号码的客人");
        return FALSE;
    }

}

void maintain_ginfo_modify(R_type **phead)
{
   if(maintain_ginfo_delete(phead) == TRUE)
   {
    printf("\n修改后的信息:");
    maintain_ginfo_add(phead);
   }
}

/**
 * 函数名称: maintain_ginfo_money
 * 函数功能: 修改客人缴费信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void maintain_ginfo_money(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    char id[20], find = 0;
    char date[12];
    float money;

    printf("\n请输入客人的身份证号:");
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
        printf("\n请输入缴费时间(yyyy/mm/dd):");
        scanf("%s",date);

        if(*(pGUEST->out_date) == '\0')
        {
            pGUEST->pay_s = pROOM->rent * months(pGUEST->in_date, date);
            printf("\n客人应缴费用:%f",pGUEST->pay_s);
            printf("\n客人已缴费用:%f",pGUEST->pay_a);
            printf("\n请输入客人该月缴费:");
            scanf("%f",&money);
            pGUEST->pay_a += money;
        }

        else
        {
            pGUEST->pay_s = pROOM->rent * months(pGUEST->in_date, pGUEST->out_date);
            printf("\n客人应缴费用:%f",pGUEST->pay_s);
            printf("\n客人已缴费用:%f",pGUEST->pay_a);
            printf("\n该客人已退房，客人本次实际缴费：");
            scanf("%f",&money);
            pGUEST->pay_a += money;
        }
    }
    else
    {
        printf("\n不存在该客人");
    }
}

/**
 * 函数名称: search_R_type
 * 函数功能: 查询客房分类信息的子模块
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
BOOL search_R_type(R_type **phead)
{
    search_rtype(phead);
    return TRUE;
}

/**
 * 函数名称: search_rtype
 * 函数功能: 查询客房分类信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
R_type* search_rtype(R_type **phead)
{
    R_type *pR_type;
    pR_type = *phead;
    char type;
    printf("\n请输入客房类别:");
    fflush(stdin);
    scanf("%c",&type);
    if(type>'Z')type += 'A'-'a';

    while((pR_type != NULL) && (pR_type->type != type))
        pR_type = pR_type->next;

    if(pR_type)
    {
        printf("\n%c类别的客房信息如下:",pR_type->type);
        printf("\n最多入住人数:%d",pR_type->bed_amount);
        printf("\n客房套数:%d",pR_type->room_amount);
        printf("\n空房套数:%d",pR_type->room_vacuum);
        return pR_type;
    }
    else
    {
        printf("\n不存在该类别的客房");
        return NULL;
    }
}

/**
 * 函数名称: search_ROOM
 * 函数功能: 查询客房信息的子模块
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void search_ROOM(R_type **phead)
{
    char option;
    fflush(stdin);
    printf("1.按客房编号查询\n2.按客房基本信息查询\n");
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
        printf("\n请输入合理的选项！");

}

/**
 * 函数名称: search_rnum
 * 函数功能: 按客房编号查询客房信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
ROOM* search_rnum(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    char find = 0;
    short searchnum;
    printf("\n请输入客房的号码:");
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
        printf("%d房信息如下:",pROOM->r_num);
        printf("\n客房电话  :%d",pROOM->r_tel);
        printf("\n客房类别  :%c",pROOM->r_type);
        printf("\n客房面积  :%d",pROOM->r_area);
        printf("\n每月租金  :%f",pROOM->rent);
        printf("\n有无阳台(1有/0无):%c",pROOM->b_flag);
        printf("\n是否有人入住:");
        if(pROOM->g_flag != 0)
        {
            printf("有");
        }
        else
        {
            printf("无");
        }
        return pROOM;
    }
    else
    {
        printf("\n不存在该编号的客房");
        return NULL;
    }
}

/**
 * 函数名称: search_rtype_plus
 * 函数功能: 按客房类型及租金查询客房功能
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void search_rtype_plus(R_type **phead)
{
    R_type *pR_type=*phead;
    ROOM *pROOM;
    char type;
    float rent;
    float epsilon;
    char find = 0;
    printf("\n客房类型:");
    fflush(stdin);
    scanf("%c",&type);
    printf("每月租金:");
    scanf("%f",&rent);
    printf("可接受的浮动范围:");
    scanf("%f",&epsilon);
    printf("符合要求的客房:\n");
    printf("%-10s%-20s%-20s%-10s%-10s","客房编号","客人数量","有无阳台(1有/0无)","客房面积","每月租金");
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
        printf("\n\n不存在这样的客房");
    }
}

/**
 * 函数名称: search_GUEST
 * 函数功能: 查询客人信息的子模块
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void search_GUEST(R_type **phead)
{
    char option;
    fflush(stdin);
    printf("1.按客人身份证号查询\n2.按客人基本信息查询\n");
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
        printf("\n请输入合理的选项！");

    return TRUE;
}

/**
 * 函数名称: search_gID
 * 函数功能: 按客人身份证查询客房信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
GUEST* search_gID(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    char id[20],find=0;
    printf("\n请输入客人的身份证号:");
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
        printf("\n身份证号为%s的客人信息如下:",pGUEST->g_id);
        printf("\n所在客房:%d",pROOM->r_num);
        printf("\n姓名    :%s",pGUEST->g_name);
        printf("\n房间号码:%d",pGUEST->r_num);
        printf("\n押金    :%f",pROOM->rent);
        printf("\n入住时间:%s",pGUEST->in_date);
        printf("\n实际缴费:%f",pGUEST->pay_a);
        if(*(pGUEST->out_date) != '\0')
        {
            printf("\n退房时间:%s",pGUEST->out_date);
            printf("\n入住月数:%d",pGUEST->in_months);
            printf("\n应缴费用:%.2f",pGUEST->pay_s);
        }
        return pGUEST;
    }
    else
    {
        printf("\n不存在该身份证号的客人");
        return NULL;
    }
}

/**
 * 函数名称: search_ginfo
 * 函数功能: 按客人基本信息查询客人信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void search_ginfo(R_type **phead)
{
    char begin[18],over[18],name[20],choice;
    char match,flag,i,find=0;

    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;

    printf("\n请选择以客人的姓或名进行搜索(1姓/2名):");
    fflush(stdin);
    scanf("%d",&choice);
    printf("搜索的关键字:");
    scanf("%s",name);
    getchar();
    printf("入住时间范围起点:(yyyy/mm/dd):");
    scanf("%s",begin);
    getchar();
    printf("入住时间范围终点(yyyy/mm/dd):");
    scanf("%s",over);
    getchar();
    pR_type=*phead;
    printf("符合条件的客人有:");
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
    if(!find)printf("\n无");
}

/**
 * 函数名称: statistic_1
 * 函数功能: 统计各类客房数目信息
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void statistic_1(R_type **phead)
{
    R_type *pR_type;
    pR_type = *phead;

    while(pR_type)
    {
        printf("\n客房类型:%c",pR_type->type);
        printf("\n客房总数:%d",pR_type->room_amount);
        printf("\n已住客房数:%d",pR_type->room_amount-pR_type->room_vacuum);
        printf("\n未住客房数:%d\n",pR_type->room_vacuum);
        pR_type=pR_type->next;
    }

}

/**
 * 函数名称: statistic_2
 * 函数功能:统计给定年份的所有客房每月营业额
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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
    printf("\n请输入要统计的年份:");
    fflush(stdin);
    scanf("%s",year);

    ClearScreen();

    printf("\n年度:%s%20c%12s",year,' ',"单位:元人民币");
    pR_type=*phead;
    type_num=0;
    printf("\n月份");
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
                    {/*如果入住时间早于检查点，并在检查点时在住（退房时间晚于检查点或状态为在住）*/
                        flag[i1] = 1;
                        /*标记此月份*/
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
    printf("\n合计");
    for(j=0;j<type_num;j++)
        printf("%12.2f",amount[j]);
}

/**
 * 函数名称: statistic_3
 * 函数功能: 所有客房入住率等信息按年统计
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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

    printf("\n请输入要统计的年份:");
    fflush(stdin);
    scanf("%s",year);

    ClearScreen();

    printf("\n年度:%s%20c%12s",year,' ',"单位:元人民币");
    pR_type = *phead;
    type_num = 0;
    printf("\n%8s%12s%12s%12s%12s","客房编号","类别","营业额","入住月数","入住率");
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
                    {/*如果入住时间在收租时间之前，且没有退房（退房时间在收租时间之后或在住）*/
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
    printf("\n%8s%12c%12.2f%12d%12.2f%%\n","合计",' ',income,MONTHS,(MONTHS/(NUM*12))*100);


}

/**
 * 函数名称: statistic_4
 * 函数功能: 累计租房月数最多的10个客人
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void statistic_4(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST, *rank[11]={NULL}, *rtmp;
    int a[11]={0}, atmp;
    char now[12], i;

    printf("\n请输入当前时间(yyyy/mm/dd):");
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
    printf("%-20s%-12s%-14s%-12s%-14s","身份证号","姓名","累计住宿月数","应缴总额","实际缴费总额");
    for(i=0;i<10;i++)
    {
        if(a[i])
        {
            printf("\n%-20s%-12s%-14d%-12.2f%-14.2f\n",rank[i]->g_id,rank[i]->g_name,a[i],rank[i]->pay_s,rank[i]->pay_a);
        }
    }


}

/**
 * 函数名称: statistic_5
 * 函数功能: 客人缴费欠费信息统计
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void statistic_5(R_type **phead)
{
    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    pR_type=*phead;

    char now[12];

     printf("\n请输入当前时间(yyyy/mm/dd):");
    scanf("%s",now);

    printf("\n%-10s%-20s%-10s%-12s%-12s%-12s%-12s\n","姓名","身份证号","是否在住","客房号码","应缴费用","实际缴费","欠费");
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
                    printf("%-10s","  是");
                }
                else
                {
                    printf("%-10s","  否");
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
 * 函数名称: SaveData
 * 函数功能: 保存系统数据
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SaveData(R_type **phead)
{

    R_type *pR_type;
    ROOM *pROOM;
    GUEST *pGUEST;
    FILE *pFile1, *pFile2, *pFile3;

    if((pFile1 = fopen("RoomType.dat","wb")) == NULL)
    {
        printf("\n客房类别文件打开失败");
        exit(-1);
    }

    if((pFile2 = fopen("RoomInfo.dat","wb")) == NULL)
    {
        printf("\n客房基本信息文件打开失败");
        exit(-1);
    }

    if((pFile3 = fopen("GuestInfo.dat","wb")) == NULL)
    {
        printf("\n客人信息文件打开失败");
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
 * 函数名称: ExitSys
 * 函数功能: 退出系统，并选择是否保存
 * 输入参数: 链表头指针地址phead
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ExitSys(R_type **phead)
{
    char save;
    printf("是否保存修改？(y/n)\n");
    fflush(stdin);
    scanf("%s",&save);

    if(save=='y'||save=='Y')
    {
      SaveData(phead);
    }

    over = 1;
}

/**
 * 函数名称: months
 * 函数功能: 按格式计算两个时间点之间的月数
 * 输入参数: 字符串in,out
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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
