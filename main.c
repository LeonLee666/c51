#include<reg51.h>

#define uchar unsigned char
#define uint unsigned int
sbit lcdrs = P3 ^5;  //定义1602液晶RS端
sbit lcden = P3 ^4;//定义1602液晶LCDEN端
sbit s1 = P3 ^0;   //定义按键--功能键
sbit s2 = P3 ^1;              //定义按键--增加键
sbit s3 = P3 ^2;              //定义按键--减小键
sbit s4 = P3 ^3;              //定义按键--闹钟键
sbit beep = P2 ^2; //定义蜂鸣器端
sbit led = P2 ^3;  //定义LED灯端
uchar count, s1num, s4num;
char year, month, day, week, miao, shi, fen, pshi;
uchar code
table[]=" 20  -  -   ";//定义初始上电时液晶默认显示状态


void delay(uint z)  //延时函数
{
    uint x, y;
    for (x = z; x > 0; x--)
        for (y = 110; y > 0; y--);
}

void zhuanhuan() {
    if (shi == 0)
        pshi = 12;
    if (shi <= 12 && shi >= 1)
        pshi = shi;
    if (shi > 12)
        pshi = shi - 12;
}

void di()//蜂鸣器发声函数
{
    beep = 0;
    delay(300);
    beep = 1;
    delay(300);
}

void write_com(uchar com)//液晶写命令函数
{
    lcdrs = 0;
    lcden = 0;
    P0 = com;
    delay(5);
    lcden = 1;
    delay(5);
    lcden = 0;
}

void write_date(uchar date)//液晶写数据函数
{
    lcdrs = 1;
    lcden = 0;
    P0 = date;
    delay(5);
    lcden = 1;
    delay(5);
    lcden = 0;
}

void shi_func()              //写"AM"、"PM"函数
{
    if (shi >= 12) {
        write_com(0x80 + 0x40 + 1);//写出"PM"
        write_date('P');
        write_date('M');
    } else {
        write_com(0x80 + 0x40 + 1);//写出"AM"
        write_date('A');
        write_date('M');
    }
}

void write_ymd(uchar add1, uchar date1)//写年月日函数
{
    uchar shi, ge;
    shi = date1 / 10;//分解一个2位数的十位和个位
    ge = date1 % 10;
    write_com(0x80 + add1);//设置显示位置
    write_date(0x30 + shi);//送去液晶显示十位
    write_date(0x30 + ge); //送去液晶显示个位
}

void write_sfm(uchar add, uchar date)//写时分秒函数
{
    uchar shi, ge;
    shi = date / 10;//分解一个2位数的十位和个位
    ge = date % 10;
    write_com(0x80 + 0x40 + add);//设置显示位置
    write_date(0x30 + shi);//送去液晶显示十位
    write_date(0x30 + ge); //送去液晶显示个位
}

void write_week(char week)                 //写液晶星期显示函数
{
    write_com(0x80 + 12);
    switch (week) {
        case 1:
            write_date('M');
            delay(5);
            write_date('O');
            delay(5);
            write_date('N');
            break;
        case 2:
            write_date('T');
            delay(5);
            write_date('U');
            delay(5);
            write_date('E');
            break;
        case 3:
            write_date('W');
            delay(5);
            write_date('E');
            delay(5);
            write_date('D');
            break;
        case 4:
            write_date('T');
            delay(5);
            write_date('H');
            delay(5);
            write_date('U');
            break;
        case 5:
            write_date('F');
            delay(5);
            write_date('R');
            delay(5);
            write_date('I');
            break;
        case 6:
            write_date('S');
            delay(5);
            write_date('A');
            delay(5);
            write_date('T');
            break;
        case 7:
            write_date('S');
            delay(5);
            write_date('U');
            delay(5);
            write_date('N');
            break;
    }
}

void writeym() {
    day = 1;
    month++;
    if (month == 13) {
        month = 1;
        year++;
        if (year == 100)
            year = 0;
        write_ymd(3, year);//年若变化则重新写入
    }
    write_ymd(6, month);//月若变化则重新写入
}

void init()//初始化函数
{
    uchar num;
    lcden = 0;
    year = 13;//初始化种变量值
    month = 6;
    day = 21;
    week = 5;
    shi = 13;
    fen = 59;
    miao = 58;
    count = 0;
    s1num = 0;
    s4num = 0;
    write_com(0x38);//初始化1602液晶
    write_com(0x0c);
    write_com(0x06);
    write_com(0x01);
    write_com(0x80);//设置显示初始坐标
    for (num = 0; num < 15; num++)//显示年月日星期
    {
        write_date(table[num]);
        delay(5);
    }
    delay(5);
    write_com(0x80 + 0x40 + 6);//写出时间显示部分的两个":"
    write_date(':');
    delay(5);
    write_com(0x80 + 0x40 + 9);
    write_date(':');
    delay(5);
    write_week(week);
    write_ymd(3, year);//分别送去液晶显示
    write_ymd(6, month);
    write_ymd(9, day);
    write_sfm(10, miao);//分别送去液晶显示
    write_sfm(7, fen);
    write_sfm(4, shi);
    TMOD = 0x01;//设置定时器0工作模式1
    TH0 = (65536 - 50000) / 256;//定时器装初值
    TL0 = (65536 - 50000) % 256;
    EA = 1;              //开总中断
    ET0 = 1;              //开定时器0中断
    TR0 = 1;              //启动定时器0
}

void keyscan()//按键扫描函数
{
    if (s1 == 0) {
        delay(5);
        if (s1 == 0)//确认功能键被按下
        {
            s1num++;//功能键按下次数记录
            while (!s1);//释放确认
            if (s1num == 1)//第一次被按下时
            {
                TR0 = 0;  //关闭定时器
                write_com(0x80 + 3);//光标定位到年位置
                write_com(0x0f); //光标开始闪烁
            }
            if (s1num == 2)//第二次按下光标闪烁定位到月位置
            {
                write_com(0x80 + 6);
            }
            if (s1num == 3)//第三次按下光标闪烁定位到日位置
            {
                write_com(0x80 + 9);
            }
            if (s1num == 4)//第四次按下光标闪烁定位到星期位置
            {
                write_com(0x80 + 12);
            }
            if (s1num == 7)//第七次被按下时光标定位到秒位置
            {
                write_com(0x80 + 0x40 + 10);
            }
            if (s1num == 6)//第六次按下光标闪烁定位到分位置
            {
                write_com(0x80 + 0x40 + 7);
            }
            if (s1num == 5)//第五次按下光标闪烁定位到时位置
            {
                write_com(0x80 + 0x40 + 4);
            }
            if (s1num == 8)//第七次按下
            {
                s1num = 0;//记录按键数清零
                write_com(0x0c);//取消光标闪烁
                TR0 = 1;              //启动定时器使时钟开始走
            }
        }
    }
    if (s1num != 0)//只有功能键被按下后，增加和减小键才有效
    {
        if (s2 == 0) {
            delay(5);
            if (s2 == 0)//增加键确认被按下
            {
                while (!s2);//按键释放
                if (s1num == 1)//若功能键第一次按下
                {
                    year++; //则调整年加1
                    if (year == 100)//若满100后将清零
                        year = 0;
                    write_ymd(3, year);//每调节一次送液晶显示一下
                    write_com(0x80 + 3);//显示位置重新回到调节处
                }
                if (s1num == 2)//若功能键第二次按下
                {
                    month++;//则调整月加1
                    if (month == 13)//若满12后将置一
                        month = 1;
                    write_ymd(6, month);//每调节一次送液晶显示一下
                    write_com(0x80 + 6);//显示位置重新回到调节处
                }
                if (s1num == 3)//若功能键第三次按下
                {
                    day++;//则调整日加1
                    if (year % 4 == 0 && month == 2) {
                        if (day == 30)//若满29后将置一
                            day = 1;
                    }
                    if (year % 4 != 0 && month == 2) {
                        if (day == 29)//若满28后将置一
                            day = 1;
                    }
                    if (month != 2 && month != 4 && month != 6 && month != 9 && month != 11) {
                        if (day == 32)//若满31后将置一
                            day = 1;
                    }
                    if (month == 4 || month == 6 || month == 9 || month == 11) {
                        if (day == 31)//若满30后将置一
                            day = 1;
                    }
                    write_ymd(9, day);;//每调节一次送液晶显示一下
                    write_com(0x80 + 9);//显示位置重新回到调节处
                }
                if (s1num == 4)//若功能键第四次按下
                {
                    week++;//则调整星期加1
                    if (week == 8)//若满8后将置一
                        week = 1;
                    write_week(week);//每调节一次送液晶显示一下
                    write_com(0x80 + 12);//显示位置重新回到调节处
                }
                if (s1num == 7)//若功能键第七次按下
                {
                    miao++; //则调整秒加1
                    if (miao == 60)//若满60后将清零
                        miao = 0;
                    write_sfm(10, miao);//每调节一次送液晶显示一下
                    write_com(0x80 + 0x40 + 10);//显示位置重新回到调节处
                }
                if (s1num == 6)//若功能键第二次按下
                {
                    fen++;//则调整分钟加1
                    if (fen == 60)//若满60后将清零
                        fen = 0;
                    write_sfm(7, fen);//每调节一次送液晶显示一下
                    write_com(0x80 + 0x40 + 7);//显示位置重新回到调节处
                }
                if (s1num == 5)//若功能键第五次按下
                {
                    shi++;
                    if (shi == 24)//若满24后将清零
                    {
                        shi = 0;
                    }
                    if (s4num == 0) {
                        write_sfm(4, shi);;//每调节一次送液晶显示一下
                        write_com(0x80 + 0x40 + 4);//显示位置重新回到调节处
                    }
                    if (s4num == 1) {
                        zhuanhuan();
                        shi_func();
                        write_sfm(4, pshi);;//每调节一次送液晶显示一下
                        write_com(0x80 + 0x40 + 4);//显示位置重新回到调节处
                    }
                }
            }
        }
    }
    if (s3 == 0) {
        delay(5);
        if (s3 == 0)//确认减小键被按下
        {
            while (!s3);//按键释放
            if (s1num == 1)//若功能键第一次按下
            {
                year--;//则调整秒减1
                if (year == -1)//若减到负数则将其重新设置为99
                    year = 99;
                write_ymd(3, year);//每调节一次送液晶显示一下
                write_com(0x80 + 3);//显示位置重新回到调节处
            }
            if (s1num == 2)//若功能键第二次按下
            {
                month--;//则调整分钟减1
                if (month == 0)//若减到负数则将其重新设置为59
                    month = 12;
                write_ymd(6, month);//每调节一次送液晶显示一下
                write_com(0x80 + 6);//显示位置重新回到调节处
            }
            if (s1num == 3)//若功能键第二次按下
            {
                day--;//则调整日加1
                if (year % 4 == 0 && month == 2) {
                    if (day == 0)//若满29后将置一
                        day = 29;
                }
                if (year % 4 != 0 && month == 2) {
                    if (day == 0)//若满28后将置一
                        day = 28;
                }
                if (month != 2 && month != 4 && month != 6 && month != 9 && month != 11) {
                    if (day == 0)//若满31后将置一
                        day = 31;
                }
                if (month == 4 || month == 6 || month == 9 || month == 11) {
                    if (day == 0)//若满30后将置一
                        day = 30;
                }
                write_ymd(9, day);;//每调节一次送液晶显示一下
                write_com(0x80 + 9);//显示位置重新回到调节处
            }
            if (s1num == 4)//若功能键第二次按下
            {
                week--;//则调整小时减1
                if (week == 0)//若减到负数则将其重新设置为23
                    week = 7;
                write_week(week);//每调节一次送液晶显示一下
                write_com(0x80 + 12);//显示位置重新回到调节处
            }
            if (s1num == 7)//若功能键第一次按下
            {
                miao--;//则调整秒减1
                if (miao == -1)//若减到负数则将其重新设置为59
                    miao = 59;
                write_sfm(10, miao);//每调节一次送液晶显示一下
                write_com(0x80 + 0x40 + 10);//显示位置重新回到调节处
            }
            if (s1num == 6)//若功能键第二次按下
            {
                fen--;//则调整分钟减1
                if (fen == -1)//若减到负数则将其重新设置为59
                    fen = 59;
                write_sfm(7, fen);//每调节一次送液晶显示一下
                write_com(0x80 + 0x40 + 7);//显示位置重新回到调节处
            }
            if (s1num == 5)//若功能键第二次按下
            {
                shi--;
                if (shi == -1)//若满24后将清零
                    shi = 23;
                if (s4num == 0) {
                    write_sfm(4, shi);;//每调节一次送液晶显示一下
                    write_com(0x80 + 0x40 +
                              4);//显示位置重新回到调节处
                }
                if (s4num == 1) {
                    zhuanhuan();
                    shi_func();
                    write_sfm(4, pshi);;//每调节一次送液晶显示一下
                    write_com(0x80 + 0x40 + 4);//显示位置重新回到调节处
                }
            }
        }
    }
    if (s1num == 0) {
        if (s4 == 0) {
            delay(5);
            if (s4 == 0)//确认功能键被按下
            {
                s4num++;//功能键按下次数记录
                while (!s4);//释放确认
                if (s4num == 1)//第一次被按下时
                {
                    zhuanhuan();
                    shi_func();
                    write_sfm(4, pshi);
                }
                if (s4num == 2)//第二次按下
                {
                    s4num = 0;//记录按键数清零
                    write_com(0x80 + 0x40 + 1);
                    write_date(' ');
                    write_date(' ');
                    write_sfm(4, shi);
                }
            }
        }
    }
}

void main()//主函数
{
    init();//首先初始化各数据
    while (1)//进入主程序大循环
    {
        keyscan();//不停的检测按键是否被按下
        if (s1num != 0)
            led = 0;
        else
            led = miao % 2;
        if ((shi > 7 && shi < 23) && (fen == 0) && (miao == 0)) {
            //di();
        }
    }
}

void timer0() interrupt 1 //定时器0中断服务程序
{
    TH0 = (65536 - 50000) / 256;//再次装定时器初值
    TL0 = (65536 - 50000) % 256;
    count++;                            //中断次数累加
    if (count == 20) //20次50毫秒为1秒
    {
        count = 0;
        miao++;
        if (miao == 60)//秒加到60则进位分钟
        {
            miao = 0;//同时秒数清零
            fen++;
            if (fen == 60)//分钟加到60则进位小时
            {
                fen = 0;//同时分钟数清零
                shi++;
                if (shi == 24)//小时加到24则小时清零
                {
                    shi = 0;
                    week++;
                    if (week == 8) {
                        week = 1;
                    }
                    write_week(week);//星期若变化则重新写入
                    day++;
                    if (year % 4 == 0 && month == 2)//判断是否为闰年的2月
                    {
                        if (day == 30) {
                            writeym();
                        }
                        write_ymd(9, day);//日若变化则重新写入
                    }
                    if (year % 4 != 0 && month == 2)//判断是否为平年的2月
                    {
                        if (day == 29) {
                            writeym();
                        }
                        write_ymd(9, day);//日若变化则重新写入
                    }
                    if (month != 2 && month != 4 && month != 6 && month != 9 && month != 11) {
                        if (day == 32) {
                            writeym();
                        }
                        write_ymd(9, day);//日若变化则重新写入
                    }
                    if (month == 4 || month == 6 || month == 9 || month == 11) {
                        if (day == 31) {
                            writeym();
                        }
                        write_ymd(9, day);//日若变化则重新写入
                    }
                }
                if (s4num == 1) {
                    zhuanhuan();
                    shi_func();
                    write_sfm(4, pshi);//12小时制小时若变化则重新写入
                } else write_sfm(4, shi);//24小时制小时若变化则重新写入
            }
            write_sfm(7, fen);//分钟若变化则重新写入
        }
        write_sfm(10, miao);//秒若变化则重新写入
    }
}
