#include     <stdio.h>      /*标准输入输出定义*/

#include     <stdlib.h>     /*标准函数库定义*/

#include     <unistd.h>     /*Unix标准函数定义*/

#include     <sys/types.h>  /**/

#include     <sys/stat.h>   /**/

#include     <fcntl.h>      /*文件控制定义*/

#include     <termios.h>    /*PPSIX终端控制定义*/

#include     <errno.h>      /*错误号定义*/


void HexDump(void *v,int len,int addr) {
    int i,j,k;
    char binstr[80];

    char *buf = (char *)v;
    for (i=0;i<len;i++) {
        if (0==(i%16)) {
            sprintf(binstr,"%08x -",i+addr);
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        } else if (15==(i%16)) {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
            sprintf(binstr,"%s  ",binstr);
            for (j=i-15;j<=i;j++) {
                sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
            }
            printf("%s\n",binstr);
        } else {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        }
    }
    if (0!=(i%16)) {
        k=16-(i%16);
        for (j=0;j<k;j++) {
            sprintf(binstr,"%s   ",binstr);
        }
        sprintf(binstr,"%s  ",binstr);
        k=16-k;
        for (j=i-k;j<i;j++) {
            sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
        }
        printf("%s\n",binstr);
    }
}


/***@brief  设置串口通信速率

 *@param  fd     类型 int  打开串口的文件句柄

 *@param  speed  类型 int  串口速度

 *@return  void*/



#define FALSE -1

#define TRUE 0





int speed_arr[] = { B57600, B38400, B115200, B19200, B9600, B4800, B2400, B1200, B300,

    B38400, B19200, B9600, B4800, B2400, B1200, B300, };

int name_arr[] = {57600, 38400, 115200,  19200,  9600,  4800,  2400,  1200,  300,

    38400,  19200,  9600, 4800, 2400, 1200,  300, };

void set_speed(int fd, int speed)   //Linux 下串口USB等设备通信编程入门2中有终端属性的获取设置函数

{

    int   i;

    int   status;

    struct termios   Opt;

    tcgetattr(fd, &Opt);

    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)

    {

        if  (speed == name_arr[i])

        {

            tcflush(fd, TCIOFLUSH);//Update the options and do it NOW

            cfsetispeed(&Opt, speed_arr[i]);

            cfsetospeed(&Opt, speed_arr[i]);

            status = tcsetattr(fd, TCSANOW, &Opt);

            if  (status != 0)

                perror("tcsetattr fd1");

            return;

        }

        tcflush(fd,TCIOFLUSH);

    }

}

/**

 *@brief   设置串口数据位，停止位和效验位

 *@param  fd     类型  int  打开的串口文件句柄*

 *@param  databits 类型  int 数据位   取值 为 7 或者8   数据位为7位或8位

 *@param  stopbits 类型  int 停止位   取值为 1 或者2*    停止位为1或2位

 *@param  parity  类型  int  效验类型 取值为N,E,O,,S     N->无奇偶校验，O->奇校验 E->为偶校验，

 */

int set_Parity(int fd,int databits,int stopbits,char parity)

{

    struct termios options;

    if  ( tcgetattr( fd,&options)  !=  0)

    {

        perror("SetupSerial 1");

        return(FALSE);

    }

    options.c_cflag &= ~CSIZE;

    switch (databits) /*设置数据位数*/

    {

        case 7:

            options.c_cflag |= CS7;

            break;

        case 8:

            options.c_cflag |= CS8;

            break;

        default:

            fprintf(stderr,"Unsupported data size\n");

            return (FALSE);

    }

    switch (parity)

    {

        case 'n':

        case 'N':

            options.c_cflag &= ~PARENB;   /* Clear parity enable */

            options.c_iflag &= ~INPCK;     /* Enable parity checking */

            break;

        case 'o':

        case 'O':

            options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/

            options.c_iflag |= INPCK;             /* Disnable parity checking */

            break;

        case 'e':

        case 'E':

            options.c_cflag |= PARENB;     /* Enable parity */

            options.c_cflag &= ~PARODD;   /* 转换为偶效验*/

            options.c_iflag |= INPCK;       /* Disnable parity checking */

            break;

        case 'S':

        case 's':  /*as no parity*/

            options.c_cflag &= ~PARENB;

            options.c_cflag &= ~CSTOPB;

            break;

        default:

            fprintf(stderr,"Unsupported parity\n");

            return (FALSE);

    }

    /* 设置停止位*/

    switch (stopbits)

    {

        case 1:

            options.c_cflag &= ~CSTOPB;

            break;

        case 2:

            options.c_cflag |= CSTOPB;

            break;

        default:

            fprintf(stderr,"Unsupported stop bits\n");

            return (FALSE);

    }

    /* Set input parity option */

    if (parity != 'n')

        options.c_iflag |= INPCK;

    options.c_cc[VTIME] = 0; // 15 seconds

    options.c_cc[VMIN] = 0;



    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */

    if (tcsetattr(fd,TCSANOW,&options) != 0)

    {

        perror("SetupSerial 3");

        return (FALSE);

    }

    return (TRUE);

}

/**

 *@breif 打开串口

 */

int OpenDev(char *Dev)

{

    int fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY

    if (-1 == fd)

    { /*设置数据位数*/

        perror("Can't Open Serial Port");

        return -1;

    }

    else

        return fd;



}

/**

 *@breif     main()

 */

int main(int argc, char **argv)

{

    int fd;

    int nread;

    char buff[512];

    char *dev ="/dev/cu.usbserial";

    fd = OpenDev(dev);

    if (fd>0)

        set_speed(fd,57600);

    else

    {

        printf("Can't Open Serial Port!\n");

        exit(0);

    }

    if (set_Parity(fd,8,1,'S')== FALSE)  // 8位数据，非两位的停止位，不使用奇偶校验 ，不允许输入奇偶校验



    {

        printf("Set Parity Error\n");

        exit(1);

    }

    printf("will read...\n");

    while(1)

    {
        // printf("test loop -1\n");
        while((nread = read(fd,buff,512))>0)

        {

            printf("\nLen %d\n",nread);

            buff[nread+1]='\0';

            printf("\n%s",buff);
            HexDump(buff, nread, 0);

        }
        // printf("test loop\n");
        sleep(1);
        // printf("test loop 1\n");
    }

    //close(fd);

    //exit(0);

}