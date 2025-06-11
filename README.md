int main(void)

{

    ADC_ReInit();

    while (1)
    {	
    
		adc_value[0] = Read_ADC_Channel(ADC_Channel_0);  // PA0
  
		adc_value[1] = Read_ADC_Channel(ADC_Channel_1);  // PA1

		delay_ms(200);
  
    }
    
}
