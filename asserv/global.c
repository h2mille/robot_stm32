#include <global.h>
#include <stm32f10x_flash.h>

void fifo_init(fifo_t * f, char * buf, int size){
     f->head = 0;
     f->tail = 0;
     f->size = size;
     f->buf = buf;
}

void init_global()
{
	asserv_type            = angle_and_position;
	direction_asserv       = don_t_care;
	robot_position.x       = 0;
	robot_position.y       = 0;
	robot_position.theta   = 0;
	willing_position.x     = 0;
	willing_position.y     = 1;
	willing_position.theta = 0;

	encoder_resolution     = 200 * 14;
	perimeter_L            = 0.19;
	perimeter_R            = 0.19;
	wheel_distance         = 0.2;
	sign_codeur_L          = -1;
	sign_codeur_R          = 1;
	v_max                  = 500 ;
	direct_coef.P          = 10000;
    direct_coef.I	       = 00;
    direct_coef.D          = 500;
    angular_coef.P         = 10000;
	angular_coef.I	       = 00;
    angular_coef.D         = 500;

	angular_range       = 0.05;

	fifo_init(RxFifo, (void *)RxBuffer, sizeof(RxBuffer));
	fifo_init(TxFifo, (void *)TxBuffer, sizeof(TxBuffer));


}

uint32_t startAddress = 0x0001F800;//starting from 128KB
                                  //0x8040000 starting 256KB
void save_to_flash(){
	FLASH_Unlock();//unlock flash writing
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(startAddress);//erase the entire page before you can write as I //mentioned

    FLASH_ProgramWord(startAddress +  0, *(int *)&encoder_resolution);
    FLASH_ProgramWord(startAddress +  4, *(int *)&perimeter_L       );
    FLASH_ProgramWord(startAddress +  8, *(int *)&perimeter_R       );
    FLASH_ProgramWord(startAddress + 12, *(int *)&wheel_distance    );
    FLASH_ProgramWord(startAddress + 16, *(int *)&sign_codeur_L     );
	FLASH_ProgramWord(startAddress + 20, *(int *)&sign_codeur_R     );
	FLASH_ProgramWord(startAddress + 24, *(int *)&v_max             );
	FLASH_ProgramWord(startAddress + 28, *(int *)&direct_coef.P     );
	FLASH_ProgramWord(startAddress + 32, *(int *)&direct_coef.I	    );
	FLASH_ProgramWord(startAddress + 36, *(int *)&direct_coef.D     );
	FLASH_ProgramWord(startAddress + 40, *(int *)&angular_coef.P    );
	FLASH_ProgramWord(startAddress + 44, *(int *)&angular_coef.I	);
	FLASH_ProgramWord(startAddress + 48, *(int *)&angular_coef.D    );
	FLASH_Lock();//lock the flash for writing

}

void load_from_flash(){
	encoder_resolution= *(int *)(startAddress + 0);
	perimeter_L       = *(float *)(startAddress + 4);
	perimeter_R       = *(float *)(startAddress + 8);
	wheel_distance    = *(float *)(startAddress + 12);
	sign_codeur_L     = *(int *)(startAddress + 16);
	sign_codeur_R     = *(int *)(startAddress + 20);
	v_max             = *(int *)(startAddress + 24);
	direct_coef.P     = *(float *)(startAddress + 28);
    direct_coef.I	  = *(float *)(startAddress + 32);
    direct_coef.D     = *(float *)(startAddress + 36);
    angular_coef.P    = *(float *)(startAddress + 40);
	angular_coef.I	  = *(float *)(startAddress + 44);
    angular_coef.D    = *(float *)(startAddress + 48);
}
