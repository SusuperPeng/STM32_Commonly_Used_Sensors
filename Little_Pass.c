#include "stm32f10x.h"
#include "Little_Pass.h"

static float Filter_Simple=0.1; //滤波系数
float Filter_RC_Simple(float NEW_DATA,float OLD_DATA)
{
	OLD_DATA = Filter_Simple * OLD_DATA + (1 - Filter_Simple) * NEW_DATA;
	return OLD_DATA;
}


#define Threshold_1     8       //阈值1用于一阶带参滤波器，变化角度大于此值时，计数增加
#define Threshold_2     30      //阈值2用于一阶带参滤波器，计数值大于此值时，增大参数，增强滤波跟随

static char new_flag = 0;//本次数据变化方向
static float Filter=0.1; //滤波系数
static char Filter_Counter=0;	//滤波计数器
static float Filter_Weight = 0.1; //滤波权重(代表在滤波结果中的权重)

/*****带系数修改的一阶滤波函数(一阶RC低通滤波)
输入：NEW_DATA       新采样的角度值
          OLD_DATA      上次滤波获得的角度结果
          filter_flag   上次数据变化方向
输出：result      本次滤波角度结果
 */
float Filter_RC(float NEW_DATA,float OLD_DATA,char filter_flag)
{
    //角度变化方向，new_flag=1表示角度增加，=0表示角度正在减小
    if((NEW_DATA-OLD_DATA)>0)
        new_flag=1;
    else if((NEW_DATA-OLD_DATA)<0)
        new_flag=0;
 
    if(new_flag == filter_flag)  //此次变化与前一次变化方向是否一致，相等表示角度变化方向一致
        {
            Filter_Counter++;
            if(fabsf(NEW_DATA-OLD_DATA)>Threshold_1)
        //当变化角度大于Threshold_1度的时候，进行计数器Filter_Counter快速增加，以达到快速增大K值，提高跟随性
                Filter_Counter+=5;
            if(Filter_Counter>Threshold_2)   //计数阈值设置，当角度递增或递减速度达到一定速率时，增大K值
            {
                Filter=Filter_Weight+0.1;          //0.2为Filter的增长值，看实际需要修改
                Filter_Counter=0;
            }
        }
    else
        {
            Filter_Counter=0;
            Filter=0.1;     //角度变化稳定时Filter值，看实际修改
        }
 
    OLD_DATA =(1-Filter)*(OLD_DATA)+Filter*NEW_DATA;
    filter_flag = new_flag;
	return OLD_DATA;
}



