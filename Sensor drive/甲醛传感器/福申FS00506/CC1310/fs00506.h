/* 文件：fs00506.c
 * 日期：2017/1/17
 * 作者：lissettecarlr
 * 版本：V1.0
 * 说明：该文件是用来驱动福申科技的甲烷传感器fs00506,波特率：9600,串口3v电平
 *       工作电压:3.7~6V,量程0-2PPM，分辨率0.01PPM，使用寿命3年 串口输出单位是微克每立方米
 *       标准：<100ug/m3
 * */


#ifndef FS00506_H_
#define FS00506_H_

#include "contiki.h"

//数据接收事件
extern process_event_t FS00506_data_event;

//串口接收的回调函数
int FS00506_input_byte(unsigned char c);

PROCESS_NAME(FS00506_process);

//甲醛浓度值，单位ug/M3

 struct FS00506_Data{
    int HCHO_value; //单位Ug/M3
	int temp_value; //温度
	int humi_value; //湿度
    int  state;//1表示正常读取
	unsigned char  byte_data[7]; //一共发来9个字节，两个作为包头判断，末尾是校验字节
}FS00506;

#endif
