int main(void)

{

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
	ADC1_Init();

while (1)
{

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
 
}

}
