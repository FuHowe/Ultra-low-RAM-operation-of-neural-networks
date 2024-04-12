#include "main.h"

int main(void)
{
	lw_init_borad();
	
	float* model_out = lw_model();

	printf("model out = [%f %f %f %f]\r\n", model_out[0], model_out[1], model_out[2], model_out[3]);

	free(model_out);
	model_out = NULL;
	
	return 0;
}





















