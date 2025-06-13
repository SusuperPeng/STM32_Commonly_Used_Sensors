
int main(void)

{

	float  val,val1,val2,val1_prev,val2_prev;
 
	char filter_flag;
 
 
 while(1)
 
 {
 
 val =  (float)Read_ADC_Values();

 
	val1 = Filter_RC_Simple(val,val1_prev);
 
	val1_prev = val1;

 
	val2=Filter_RC(val,val2_prev, filter_flag);
 
	val2_prev = val2;
 
  filter_flag = new_flag;

 
  u1_printf("%f,%f,%f\r\n", val,val1,val2);  // 串口输出
  
  }
  
  }
