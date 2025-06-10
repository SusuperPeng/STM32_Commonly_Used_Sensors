int main()

{
	
 ADC_MultiChannel_Init();

 while(1)
 
	{	
 
		adc_val0 = ADC_ConvertedValues[0];
  
		adc_val1 = ADC_ConvertedValues[1];
  
		adc_val2 = ADC_ConvertedValues[2];
  
		u1_printf("%d,%d,%d\r\n",adc_val0,adc_val1,adc_val2);
  
		delay_ms(200);

	}
