#include <stm32f10x_usart.h>
#include <stdio.h>
#include <global.h>
#include <CoOS.h>
#include <stdarg.h>
OS_EventID Semaphore_rx;

int GetChar (void)  {
	  while((USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET));
	  return ((int)USART_ReceiveData(USART3));
}

void print_info(){
//	printf ("pos: %d %d %d", (int)(1000*robot_position.x), (int)(1000*robot_position.y), (int)(1000*robot_position.theta));
}

//This reads nbytes bytes from the FIFO
//The number of bytes read is returned
int fifo_read(fifo_t * f, void * buf, int nbytes){
     int i;
     char * p;
     p = buf;
     for(i=0; i < nbytes; i++){
          if( f->tail != f->head ){ //see if any data is available
               *p++ = f->buf[f->tail];  //grab a byte from the buffer
               f->tail++;  //increment the tail
               if( f->tail == f->size ){  //check for wrap-around
                    f->tail = 0;
               }
          } else {
               return i; //number of bytes read
          }
     }
     return nbytes;
}
int fifo_read_until_end(fifo_t * f, void * buf){
     int i;
     char * p;
     p = buf;
     for(i=0; i < f->tail; i++){
          if( f->tail != f->tail ){ //see if any data is available
               *p++ = f->buf[f->tail];  //grab a byte from the buffer
               f->tail++;  //increment the tail
               if( f->tail == f->size ){  //check for wrap-around
                    f->tail = 0;
               }
          } else {
               return i; //number of bytes read
          }
     }
     return i;
}

//This writes up to nbytes bytes to the FIFO
//If the head runs in to the tail, not all bytes are written
//The number of bytes written is returned
int fifo_write(fifo_t * f, const void * buf, int nbytes){
     int i;
     const char * p;
     p = buf;
     for(i=0; i < nbytes; i++){
           //first check to see if there is space in the buffer
           if( (f->head + 1 == f->tail) ||
                ( (f->head + 1 == f->size) && (f->tail == 0) )){
                 return i; //no more room
           } else {
               f->buf[f->head] = *p++;
               f->head++;  //increment the head
               if( f->head == f->size ){  //check for wrap-around
                    f->head = 0;
               }
           }
     }
     return nbytes;
}

void USART3_IRQHandler(void)
{
  CoEnterISR();
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // Received characters added to fifo
  {
    uint16_t rx;

    rx = USART_ReceiveData(USART3); // Receive the character

    fifo_write(RxFifo, &rx, 1); // Place in reception fifo
   if (rx=='\r')
    	 isr_PostSem(Semaphore_rx);
  }

  if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
    uint8_t tx;

    if (fifo_read(TxFifo, &tx, 1) == 1) // Anything to send?
      USART_SendData(USART3, (uint16_t)tx); // Transmit the character
    else
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE); // Suppress interrupt when empty
  }
  CoExitISR();
}

signed int short_printf(const char *pFormat, ...)
{
    va_list ap;
    signed int result;
    char buffer [50];

    /* Forward call to vsprintf */
    va_start(ap, pFormat);
    result = vsprintf(buffer, pFormat, ap);
    va_end(ap);
    fifo_write(TxFifo, buffer, result);
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    return result;
}

signed int short_scanf(char * command, char*  order, double* value)
{
	char buffer[20];
	int i;
	int neg = 1;
	int com = 1;
	int coma = 0;
	int before_coma = 0;
	int after_coma = 0;
	fifo_read(RxFifo, buffer,20);
	*command=buffer[0];
	*order=buffer[1];
	*value = 0;
	for (i=2; buffer[i]!='\r'; i++)
	{
		if(buffer[i]=='-')
			neg = -1;
		else if(buffer[i]=='.')
			coma = i+1;
		else if(coma ==0){
			if (before_coma < 214748364)
				before_coma *= 10;
			else
				break;
			if (before_coma < 2147483640)
				before_coma +=(buffer[i]-'0');
			else
				break;
		}
		else {
			if (after_coma < 214748364)
				after_coma *= 10;
			else
				break;
			if (after_coma < 2147483640)
				after_coma +=(buffer[i]-'0');
			else
				break;
		}
	}
	*value = neg * (before_coma + ((double)after_coma) / pow(10,(i-coma)));
	return i;
}

void command_box(char command, char order, double argument)
{

	switch (order){
	case (char)1:
		switch((int)argument){
		case 1:
			asserv_type = angle;
			break;
		case 2:
			asserv_type = distance;
			break;
		case 3:
			asserv_type = position;
			break;
		case 4:
			asserv_type = angle_then_position;
			break;
		case 5:
			asserv_type = angle_and_position;
			break;
		default:
			break;
	}
	case (char)2:
	    robot_position.x = (float_t)argument;
	 	break;
	case (char)3:
	    robot_position.y = (float_t)argument;
	    break;
	case (char)4:
	    robot_position.theta = (float_t)argument;
    	break;
	case (char)5:
	    willing_position.x = (float_t)argument;
		break;
	case (char)6:
		willing_position.y = (float_t)argument;
		break;
	case (char)7:
		willing_position.theta = (float_t)argument;
		break;
	case (char)8:
		encoder_resolution = (uint32_t)argument;
		break;
	case (char)9:
		perimeter_L = (float_t)argument;
		break;
	case (char)10:
		perimeter_R = (float_t)argument;
		break;
	case (char)11:
		wheel_distance = (float_t)argument;
		break;
	case (char)12:
		direct_coef.P = (float_t)argument;
		break;
	case (char)13:
		direct_coef.I = (float_t)argument;
		break;
	case (char)14:
		direct_coef.D = (float_t)argument;
		break;
	case (char)15:
		angular_coef.P = (float_t)argument;
		break;
	case (char)16:
		angular_coef.I = (float_t)argument;
		break;
	case (char)17:
		angular_coef.D = (float_t)argument;
		break;
	case (char)18:
		v_max = (int)argument;
		break;
	case (char)19:
		sign_codeur_L = (int)argument;
		break;
	case (char)20:
		sign_codeur_R = (int)argument;
		break;
	case (char)21:
		direct_range = (float_t)argument;
		break;
	case (char)22:
		angular_range = (float_t)argument;
		break;
	case (char)23:
		direct_speed = (int)argument;
		break;
	case (char)24:
		angular_speed = (int)argument;
		break;
	case (char)25:
		direct_integrate = (int)argument;
		break;
	case (char)26:
		angular_integrate = (int)argument;
		break;
	case (char)27:
		direct_range = (float_t)argument;
		break;
	case (char)28:
		angular_range = (float_t)argument;
		break;
	default:
		break;
	}
}
