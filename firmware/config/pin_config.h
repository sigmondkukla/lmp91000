#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// $[CMU]
// [CMU]$

// $[LFXO]
// [LFXO]$

// $[PRS.ASYNCH0]
// [PRS.ASYNCH0]$

// $[PRS.ASYNCH1]
// [PRS.ASYNCH1]$

// $[PRS.ASYNCH2]
// [PRS.ASYNCH2]$

// $[PRS.ASYNCH3]
// [PRS.ASYNCH3]$

// $[PRS.ASYNCH4]
// [PRS.ASYNCH4]$

// $[PRS.ASYNCH5]
// [PRS.ASYNCH5]$

// $[PRS.ASYNCH6]
// [PRS.ASYNCH6]$

// $[PRS.ASYNCH7]
// [PRS.ASYNCH7]$

// $[PRS.ASYNCH8]
// [PRS.ASYNCH8]$

// $[PRS.ASYNCH9]
// [PRS.ASYNCH9]$

// $[PRS.ASYNCH10]
// [PRS.ASYNCH10]$

// $[PRS.ASYNCH11]
// [PRS.ASYNCH11]$

// $[PRS.ASYNCH12]
// [PRS.ASYNCH12]$

// $[PRS.ASYNCH13]
// [PRS.ASYNCH13]$

// $[PRS.ASYNCH14]
// [PRS.ASYNCH14]$

// $[PRS.ASYNCH15]
// [PRS.ASYNCH15]$

// $[PRS.SYNCH0]
// [PRS.SYNCH0]$

// $[PRS.SYNCH1]
// [PRS.SYNCH1]$

// $[PRS.SYNCH2]
// [PRS.SYNCH2]$

// $[PRS.SYNCH3]
// [PRS.SYNCH3]$

// $[GPIO]
// GPIO SWCLK on PA01
#ifndef GPIO_SWCLK_PORT                         
#define GPIO_SWCLK_PORT                          SL_GPIO_PORT_A
#endif
#ifndef GPIO_SWCLK_PIN                          
#define GPIO_SWCLK_PIN                           1
#endif

// GPIO SWDIO on PA02
#ifndef GPIO_SWDIO_PORT                         
#define GPIO_SWDIO_PORT                          SL_GPIO_PORT_A
#endif
#ifndef GPIO_SWDIO_PIN                          
#define GPIO_SWDIO_PIN                           2
#endif

// GPIO SWV on PA03
#ifndef GPIO_SWV_PORT                           
#define GPIO_SWV_PORT                            SL_GPIO_PORT_A
#endif
#ifndef GPIO_SWV_PIN                            
#define GPIO_SWV_PIN                             3
#endif

// [GPIO]$

// $[TIMER0]
// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[TIMER4]
// [TIMER4]$

// $[USART0]
// USART0 CLK on PC05
#ifndef USART0_CLK_PORT                         
#define USART0_CLK_PORT                          SL_GPIO_PORT_C
#endif
#ifndef USART0_CLK_PIN                          
#define USART0_CLK_PIN                           5
#endif

// USART0 CS on PC06
#ifndef USART0_CS_PORT                          
#define USART0_CS_PORT                           SL_GPIO_PORT_C
#endif
#ifndef USART0_CS_PIN                           
#define USART0_CS_PIN                            6
#endif

// USART0 RX on PC03
#ifndef USART0_RX_PORT                          
#define USART0_RX_PORT                           SL_GPIO_PORT_C
#endif
#ifndef USART0_RX_PIN                           
#define USART0_RX_PIN                            3
#endif

// USART0 TX on PC04
#ifndef USART0_TX_PORT                          
#define USART0_TX_PORT                           SL_GPIO_PORT_C
#endif
#ifndef USART0_TX_PIN                           
#define USART0_TX_PIN                            4
#endif

// [USART0]$

// $[I2C1]
// [I2C1]$

// $[EUSART1]
// [EUSART1]$

// $[KEYSCAN]
// [KEYSCAN]$

// $[LETIMER0]
// [LETIMER0]$

// $[IADC0]
// IADC0 POS on PA00
#ifndef IADC0_POS_PORT                          
#define IADC0_POS_PORT                           SL_GPIO_PORT_A
#endif
#ifndef IADC0_POS_PIN                           
#define IADC0_POS_PIN                            0
#endif

// [IADC0]$

// $[ACMP0]
// [ACMP0]$

// $[ACMP1]
// [ACMP1]$

// $[VDAC0]
// VDAC0 CH0_MAIN_OUT on PB00
#ifndef VDAC0_CH0_MAIN_OUT_PORT                 
#define VDAC0_CH0_MAIN_OUT_PORT                  SL_GPIO_PORT_B
#endif
#ifndef VDAC0_CH0_MAIN_OUT_PIN                  
#define VDAC0_CH0_MAIN_OUT_PIN                   0
#endif

// [VDAC0]$

// $[VDAC1]
// [VDAC1]$

// $[PCNT0]
// [PCNT0]$

// $[HFXO0]
// [HFXO0]$

// $[I2C0]
// I2C0 SCL on PA05
#ifndef I2C0_SCL_PORT                           
#define I2C0_SCL_PORT                            SL_GPIO_PORT_A
#endif
#ifndef I2C0_SCL_PIN                            
#define I2C0_SCL_PIN                             5
#endif

// I2C0 SDA on PA04
#ifndef I2C0_SDA_PORT                           
#define I2C0_SDA_PORT                            SL_GPIO_PORT_A
#endif
#ifndef I2C0_SDA_PIN                            
#define I2C0_SDA_PIN                             4
#endif

// [I2C0]$

// $[EUSART0]
// [EUSART0]$

// $[PTI]
// [PTI]$

// $[MODEM]
// [MODEM]$

// $[CUSTOM_PIN_NAME]
#ifndef LMP_VOUT_PORT                           
#define LMP_VOUT_PORT                            SL_GPIO_PORT_A
#endif
#ifndef LMP_VOUT_PIN                            
#define LMP_VOUT_PIN                             0
#endif

#ifndef LMP_SDA_PORT                            
#define LMP_SDA_PORT                             SL_GPIO_PORT_A
#endif
#ifndef LMP_SDA_PIN                             
#define LMP_SDA_PIN                              4
#endif

#ifndef LMP_SCL_PORT                            
#define LMP_SCL_PORT                             SL_GPIO_PORT_A
#endif
#ifndef LMP_SCL_PIN                             
#define LMP_SCL_PIN                              5
#endif

#ifndef LED_PORT                                
#define LED_PORT                                 SL_GPIO_PORT_A
#endif
#ifndef LED_PIN                                 
#define LED_PIN                                  8
#endif

#ifndef LMP_VREF_PORT                           
#define LMP_VREF_PORT                            SL_GPIO_PORT_B
#endif
#ifndef LMP_VREF_PIN                            
#define LMP_VREF_PIN                             0
#endif

#ifndef LDO_EN_PORT                             
#define LDO_EN_PORT                              SL_GPIO_PORT_C
#endif
#ifndef LDO_EN_PIN                              
#define LDO_EN_PIN                               0
#endif

#ifndef PWR_BTN_PORT                            
#define PWR_BTN_PORT                             SL_GPIO_PORT_C
#endif
#ifndef PWR_BTN_PIN                             
#define PWR_BTN_PIN                              1
#endif

#ifndef IMU_SDO_PORT                            
#define IMU_SDO_PORT                             SL_GPIO_PORT_C
#endif
#ifndef IMU_SDO_PIN                             
#define IMU_SDO_PIN                              3
#endif

#ifndef IMU_SDI_PORT                            
#define IMU_SDI_PORT                             SL_GPIO_PORT_C
#endif
#ifndef IMU_SDI_PIN                             
#define IMU_SDI_PIN                              4
#endif

#ifndef IMU_SPC_PORT                            
#define IMU_SPC_PORT                             SL_GPIO_PORT_C
#endif
#ifndef IMU_SPC_PIN                             
#define IMU_SPC_PIN                              5
#endif

#ifndef IMU_CS_PORT                             
#define IMU_CS_PORT                              SL_GPIO_PORT_C
#endif
#ifndef IMU_CS_PIN                              
#define IMU_CS_PIN                               6
#endif

#ifndef IMU_INT2_PORT                           
#define IMU_INT2_PORT                            SL_GPIO_PORT_C
#endif
#ifndef IMU_INT2_PIN                            
#define IMU_INT2_PIN                             7
#endif

#ifndef BATT_MEAS_PORT                          
#define BATT_MEAS_PORT                           SL_GPIO_PORT_D
#endif
#ifndef BATT_MEAS_PIN                           
#define BATT_MEAS_PIN                            0
#endif

// [CUSTOM_PIN_NAME]$


#endif // PIN_CONFIG_H


