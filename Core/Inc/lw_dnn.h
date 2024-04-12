#ifndef __LW_DNN__
#define __LW_DNN__

	#include "main.h"

	typedef enum
	{
		layer_1 = 1,
		layer_2 = 2,
		layer_3 = 3,
		layer_4 = 4
	}lw_layer_t;

	typedef struct
	{
		float head_len; 
		float layers; 
		float save_samples; 
		float feature_len; 
		float layer_1_out;
		float layer_2_out; 
		float layer_3_out; 
		float feature_out; 
	}lw_head_t;


	lw_head_t* lw_read_head(void);
	float* lw_model(void);
	
	float* lw_weight_multiply(lw_head_t* head, float* indata, lw_layer_t layer, uint32_t *addr);
	float* lw_bias_add(lw_head_t* head, float* indata, lw_layer_t layer, uint32_t *addr);
	float* lw_relu_active(lw_head_t* head, float* indata, lw_layer_t layer);
	
	void lw_read_flash(float* read_buffer, uint32_t read_len, uint32_t seek);

#endif

