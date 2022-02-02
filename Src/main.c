/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM8_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
void Razryad_Set(uint8_t  NumRazr);
void Segment_Set(unsigned char segment, uint8_t state);
void Buzzer_Pik(uint16_t delay, uint8_t n_pik);
void G_R_led_set(unsigned char color, uint8_t state);
void Print_Number(uint8_t num);
void Print_Led_display(uint16_t chislo);
void led_displayInit();
uint8_t ShiftLed(uint8_t cycleShiftled, uint16_t delay);
uint8_t check_broken_wire(uint8_t num_pin);
void print_error(uint8_t errNum);
void Test_BMSel();
void print_Program(uint8_t prr);
void MainMenu();
void Write_Shift_Reg(uint8_t* data);
void Read_Shift_Reg(uint8_t* data);
uint8_t ShiftReg_ReadPin(uint8_t pin);
void ShiftReg_WritePin(uint8_t pin, uint8_t state);
void Error(uint8_t err);
uint8_t ChekWire(uint8_t pinNumber, uint16_t delay);
uint8_t CheckAllWare(uint8_t pinNumber);
uint8_t ChekWireForSer(uint8_t pinNumber, uint16_t delay);
/* Private user code ---------------------------------------------------------*/
union ShiftReg
{
	uint32_t data_shift;
	uint8_t data[4];
};

volatile uint16_t cnt;
volatile uint8_t razr1, razr2, razr3, nextRazr;
uint16_t EncCnt;
uint8_t SizePin=20;
uint8_t program=1;
uint16_t CountTim = 0;
uint8_t FlagUpDown = 0;
uint8_t FlagBlink = 1;
uint16_t CountButtonClik = 0;
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
void TIM6_DAC_IRQHandler(void)
{
	/* USER CODE END TIM6_DAC_IRQn 0 */
	HAL_TIM_IRQHandler(&htim6);
	/* USER CODE BEGIN TIM6_DAC_IRQn 1 */
	if (FlagBlink)
	
		switch (nextRazr)
		{
		case 0:
			nextRazr++;
			Razryad_Set(0);
			Razryad_Set(3);
			Print_Number(razr3); 
			break;
		
		case 1:
			nextRazr++;
		
			if (!razr2)
			{
				if (!razr1)
					Razryad_Set(0);
				else
				{
					Print_Number(razr2);
					Razryad_Set(0);
					Razryad_Set(2);
				}			
			}
			else
			{
				Print_Number(razr2);
				Razryad_Set(0);
				Razryad_Set(2);
			}
			
			break;
		
		case 2:
			nextRazr = 0;
			if (razr1)
			{
				Razryad_Set(0);
				Razryad_Set(1);
				Print_Number(razr1);
			}
			else
			{
				Razryad_Set(0);
			}
			break;
		
		default:
			break;
		}
	else
	{
		if (CountTim > 70)
			FlagUpDown = 1;
		
		if (!CountTim)	
			FlagUpDown = 0;
		
		switch (FlagUpDown)
		{
		
		case 0:
		
			Razryad_Set(0);
		
			CountTim++;
			break;
		
		case 1:
			switch (nextRazr)
			{
			case 0:
				nextRazr++;
				Razryad_Set(0);
				Razryad_Set(3);
				Print_Number(razr3); 
				break;
		
			case 1:
				nextRazr++;
		
				if (!razr2)
				{
					if (!razr1)
						Razryad_Set(0);
					else
					{
						Print_Number(razr2);
						Razryad_Set(0);
						Razryad_Set(2);
					}			
				}
				else
				{
					Print_Number(razr2);
					Razryad_Set(0);
					Razryad_Set(2);
				}
			
				break;
		
			case 2:
				nextRazr = 0;
				if (razr1)
				{
					Razryad_Set(0);
					Razryad_Set(1);
					Print_Number(razr1);
				}
				else
				{
					Razryad_Set(0);
				}
				break;
		
			default:
				break;
			}
		
			CountTim--;
			break;
		
	
		default:
			break;
		}
	}

	
	
	
	
}
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_TIM6_Init();
	MX_TIM8_Init();
  
	HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim6);
	
	/* USER CODE BEGIN 2 */
	led_displayInit();
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	Buzzer_Pik(50, 1);
	HAL_Delay(1000);

	while (1)
	{
		print_Program(program);
		
		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
		{
			while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
			{
				HAL_Delay(1);
				CountButtonClik++;
				if (CountButtonClik > 600)
				{
					FlagBlink = 0;
					MainMenu();
					FlagBlink = 1;
				}
			}
			CountButtonClik = 0;
		}
		
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET)
		{
			while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET)
				continue;
			switch (program)
			{
			case 1:
				if (CheckAllWare(SizePin)) { Error(1); break;}
				if (ChekWire(SizePin, 10)){Error(2); break;}
				break;
				
			case 2:
				if (CheckAllWare(1)) { Error(1); break;}
				if (ChekWire(1, 50)){Error(2); break;}
				Test_BMSel();
				break;
				
			case 3:
				
				if (CheckAllWare(10)) { Error(1); break;}
				if (ChekWire(10, 20)){Error(2); break;}
			break;
				
			case 4:
				if (CheckAllWare(10)) { Error(1); break;}
				switch (ChekWireForSer(10, 20))
				{
				case 0:
					//complete
					break;
					
				case 1:
					Error(2);
					break;
					
				case 2:
					Error(3);
					break;
							
				default:
					break;
				}				
				break;
				
			default:
				break;
			}	
		}
	}
}

void MainMenu()
{
	TIM8->ARR = 4;
	TIM8->CNT = 0;
	while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
		continue;
	
	while (1)
	{
		
		EncCnt = TIM8->CNT;
		if (!EncCnt)
			EncCnt++;
		
		print_Program(EncCnt);

		if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
		{
			while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
				continue;
					
			switch (EncCnt)
			{						
			case 1:
				program = 1;
				TIM8->ARR = 20;
				TIM8->CNT = 0;
				
				while (1)
				{
					EncCnt = TIM8->CNT;
					Print_Led_display(EncCnt);
					if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
					{
						while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
							continue;
						SizePin = EncCnt;
						return;
					}
				}
			break;
						
			case 2:
				program = 2;
				return;
			break;
				
			case 3:
				program = 3;
				return;
			break;
					
			case 4:
				program = 4;
				return;
			break;
				
			default:
				break;
			}	
		}
	}
}
void led_displayInit()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
	Segment_Set('a', 0);       //
	Segment_Set('b', 0);
	Segment_Set('c', 0);
	Segment_Set('d', 0);
	Segment_Set('e', 0);
	Segment_Set('f', 0);
	Segment_Set('g', 0);
	Segment_Set('p', 0);
	
}
void Print_Led_display(uint16_t chislo)
{
	razr1 = chislo / 100;              ///999
	razr2 = chislo % 100 / 10;
	razr3 = chislo % 100 % 10;	
}

void Test_BMSel()
{
	uint8_t data[3];
	uint32_t spisok[15] = { 0x1E2, 0x1D2, 0x1B2, 0x172, 0xF2, 0x1E4, 0x1D4, 0x1B4, 0x174, 0xF4, 0x1E8, 0x1D8, 0x1B8, 0x178, 0xF8 };
	
	for (uint8_t i=0;i<15;i++)
	{
		data[2] = spisok[i];
		data[1] = spisok[i] >> 8;
		data[0] = spisok[i] >> 16;
		
		Write_Shift_Reg(data);
		HAL_Delay(1000);
	}
	
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	Write_Shift_Reg(data);
		
}





uint8_t CheckAllWare(uint8_t pinNumber)
{
	uint32_t data = 0xffffffff>>(32-pinNumber);
	data =~ data;
	uint8_t transmitData[3];
	transmitData[2] = (uint8_t)(data & 0xff);
	transmitData[1] = (uint8_t)((data >> 8) & 0xff);
	transmitData[0] = (uint8_t)((data >> 16) & 0xff);
	Write_Shift_Reg(transmitData);
	HAL_Delay(10);
	

	union ShiftReg inputData;
	uint8_t input[3];
	Read_Shift_Reg(input);
	inputData.data[0] = input[2];
	inputData.data[1] = input[1];
	inputData.data[2] = input[0];
	
	inputData.data_shift |= data;
	
	if (inputData.data_shift!=data)
	{
		return 1;
	}
	return 0;
}




uint8_t ChekWire(uint8_t pinNumber, uint16_t delay)
{
	for (uint8_t i = 1; i<pinNumber+1;i++)
	{
		ShiftReg_WritePin(i, 0);
		HAL_Delay(delay);
		if (ShiftReg_ReadPin(i)) return 1;
	}
	for (uint8_t j = 1; j < pinNumber + 1; j++)
	{
		ShiftReg_WritePin(j, 1);
		HAL_Delay(delay);
		if (!ShiftReg_ReadPin(j)) return 1; 
	}
	G_R_led_set('G', 1);
	ShiftReg_WritePin(0, 0);
	HAL_Delay(1000);
	G_R_led_set('G', 0);
	return 0;
}

uint8_t ChekWireForSer(uint8_t pinNumber, uint16_t delay)
{
	for (uint8_t i = 1; i < pinNumber + 1; i++)
	{
		ShiftReg_WritePin(i, 0);
		HAL_Delay(delay);
		if ((i == 7)|(i==8))
		{
			if (!(ShiftReg_ReadPin(7) && ShiftReg_ReadPin(8))) return 2;
		}
		else
		{
			if (ShiftReg_ReadPin(i)) return 1;
		}
	}
	
	for (uint8_t j = 1; j < pinNumber + 1; j++)
	{
		ShiftReg_WritePin(j, 1);
		HAL_Delay(delay);
		if (!ShiftReg_ReadPin(j)) return 1; 
		if ((j == 7) | (j == 8))
		{
			if (!(ShiftReg_ReadPin(7) && ShiftReg_ReadPin(8))) return 2;
		}
		else
		{
			if (!ShiftReg_ReadPin(j)) return 1;
		}
	}
	G_R_led_set('G', 1);
	ShiftReg_WritePin(0, 0);
	HAL_Delay(1000);
	G_R_led_set('G', 0);
	return 0;
}
void Error(uint8_t err)
{
	G_R_led_set('R', 1);
	print_error(err);
	Buzzer_Pik(200, 1);
	while (1)
	{	
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
		for (uint16_t i = 0; i < 500; i++)
		{
			HAL_Delay(1);
			if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
			{
				while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET)
					continue;
				
				//Write_Shift_Reg();
				G_R_led_set('R', 0);
				return;	
			}
		}		
	}
}

void ShiftReg_WritePin(uint8_t pin, uint8_t state)
{
	uint32_t data = (1 << (pin - 1));
	
	if (!state) data = data ^ 0xffffffff;	
	
	uint8_t outData[3];
	outData[2] = (uint8_t)(data & 0xff);
	outData[1] = (uint8_t)((data>>8) & 0xff);
	outData[0] = (uint8_t)((data>>16) & 0xff);
	Write_Shift_Reg(outData);
}

uint8_t ShiftReg_ReadPin(uint8_t pin)
{
	union ShiftReg inputData;
	uint8_t input[3];
	Read_Shift_Reg(input);
	inputData.data[0] = input[2];
	inputData.data[1] = input[1];
	inputData.data[2] = input[0];
	return (uint8_t)((inputData.data_shift >> (pin - 1)) & 0x1);
}

void Write_Shift_Reg(uint8_t* data)
{
	HAL_SPI_Transmit(&hspi1, (uint8_t*)data, 3, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}
void Read_Shift_Reg(uint8_t* data)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);	
	HAL_SPI_Receive(&hspi1, (uint8_t*)data, 3, 100);
}

void Print_Number(uint8_t num)
{
	switch (num)
	{
	case 0:
		Segment_Set('a', 1);
		Segment_Set('b', 1);
		Segment_Set('c', 1);
		Segment_Set('d', 1);
		Segment_Set('e', 1);
		Segment_Set('f', 1);
		Segment_Set('g', 0);
		break;
		
	case 1:
		Segment_Set('a', 0);
		Segment_Set('b', 1);
		Segment_Set('c', 1);
		Segment_Set('d', 0);
		Segment_Set('e', 0);
		Segment_Set('f', 0);
		Segment_Set('g', 0);
		break;
	case 2:
		Segment_Set('a', 1);      //abdeg
		Segment_Set('b', 1);
		Segment_Set('c', 0);
		Segment_Set('d', 1);
		Segment_Set('e', 1);
		Segment_Set('f', 0);
		Segment_Set('g', 1);
		break;
	case 3:
		Segment_Set('a', 1);      //abcdg
		Segment_Set('b', 1);
		Segment_Set('c', 1);
		Segment_Set('d', 1);
		Segment_Set('e', 0);
		Segment_Set('f', 0);
		Segment_Set('g', 1);
		break;
	case 4:
		Segment_Set('a', 0);
		Segment_Set('b', 1);      //bcfg
		Segment_Set('c', 1);
		Segment_Set('d', 0);
		Segment_Set('e', 0);
		Segment_Set('f', 1);
		Segment_Set('g', 1);
		break;
	case 5:
		Segment_Set('a', 1);      //acdfg
		Segment_Set('b', 0);
		Segment_Set('c', 1);
		Segment_Set('d', 1);
		Segment_Set('e', 0);
		Segment_Set('f', 1);
		Segment_Set('g', 1);
		break;
	case 6:
		Segment_Set('a', 1);      //acdefg
		Segment_Set('b', 0);
		Segment_Set('c', 1);
		Segment_Set('d', 1);
		Segment_Set('e', 1);
		Segment_Set('f', 1);
		Segment_Set('g', 1);
		break;
	case 7:
		Segment_Set('a', 1);      //abc
		Segment_Set('b', 1);
		Segment_Set('c', 1);
		Segment_Set('d', 0);
		Segment_Set('e', 0);
		Segment_Set('f', 0);
		Segment_Set('g', 0);
		break;
	case 8:
		Segment_Set('a', 1);      //
		Segment_Set('b', 1);
		Segment_Set('c', 1);
		Segment_Set('d', 1);
		Segment_Set('e', 1);
		Segment_Set('f', 1);
		Segment_Set('g', 1);
		break;
	case 9:
		Segment_Set('a', 1);      //abcdeg
		Segment_Set('b', 1);
		Segment_Set('c', 1);
		Segment_Set('d', 1);
		Segment_Set('e', 0);
		Segment_Set('f', 1);
		Segment_Set('g', 1);
		break;
		
	case 'e':
		Segment_Set('a', 1);       //abcdeg
		Segment_Set('b', 0);
		Segment_Set('c', 0);
		Segment_Set('d', 1);
		Segment_Set('e', 1);
		Segment_Set('f', 1);
		Segment_Set('g', 1);
		break;
		
	case 'r':
		Segment_Set('a', 0);        //abcdeg
		Segment_Set('b', 0);
		Segment_Set('c', 0);
		Segment_Set('d', 0);
		Segment_Set('e', 1);
		Segment_Set('f', 0);
		Segment_Set('g', 1);
		break;
	case 'P':
		Segment_Set('a', 1);         //abcdeg
		Segment_Set('b', 1);
		Segment_Set('c', 0);
		Segment_Set('d', 0);
		Segment_Set('e', 1);
		Segment_Set('f', 1);
		Segment_Set('g', 1);
		break;
		
	default:
		break;

	}	
}
void print_error(uint8_t errNum)
{
	razr1 = 'e';
	razr2 = 'r';
	razr3 = errNum;
}

void print_Program(uint8_t prr)
{
	razr1 = 0;
	razr2 = 'P';
	razr3 = prr;
}

void Buzzer_Pik(uint16_t delay, uint8_t n_pik)
{
	for (uint8_t i = 0; i < n_pik; i++)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_Delay(delay);
	}
}

void G_R_led_set(unsigned char color, uint8_t state)
{
	switch (color)
	{
	case 'R':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
		else 
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
		break;
		
	case 'G':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
		break;
		
	default:
		break;
	}
	
}

void Razryad_Set(uint8_t  NumRazr)
{
	switch (NumRazr)
	{
		
	case 0:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
		break;
		
	case 1:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
		break;	
		
	default:
		break;
	}
}

void Segment_Set(unsigned char segment, uint8_t state)
{
	switch (segment)
	{
	case 'a':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
		break;	
		
	case 'b':
		if (state)
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		break;
		
	case 'c':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
		break;
		
	case 'd':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
		break;
		
	case 'e':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		break;
		
	case 'f':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
		break;
		
	case 'g':
		if (state)
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		break;
		
	case 'p':
		if (state)
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
		break;
		
	default:
		break;
		
		
	}
	
	
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 80;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_I2C1_Init(void)
{

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE; 
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

	/* USER CODE BEGIN TIM6_Init 0 */

	/* USER CODE END TIM6_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM6_Init 1 */

	/* USER CODE END TIM6_Init 1 */
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 6;     ///1 секунда
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 65535 - 1;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM6_Init 2 */

	/* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{
	TIM_Encoder_InitTypeDef sConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 16;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 20;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}


static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC,
		GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
	                        |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12,
		GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
		GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4 
	                        |GPIO_PIN_10,
		GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);

	/*Configure GPIO pins : PC0 PC1 PC2 PC3 
	                         PC4 PC5 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 
	                        | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA0 PA1 PA3 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PA4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PB0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PB1 PB10 PB11 */
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PB15 */
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PC8 */
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PC11 PC12 */
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


	/*
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	
	
	
	GPIOC->AFR[0] = (0x03 << 24) | (0x03 << 28);
	*/
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
