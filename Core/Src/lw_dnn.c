#include "main.h"

float* lw_model(void)
{
	uint32_t addr = 0; 
	lw_head_t* head = NULL;	

	float* weight_out_1 = NULL;	
	float* bias_out_1 = NULL;	
	float* relu_out_1 = NULL; 

	float* weight_out_2 = NULL;
	float* bias_out_2 = NULL;
	float* relu_out_2 = NULL;


	float* weight_out_3 = NULL;	
	float* bias_out_3 = NULL;	
	float* relu_out_3 = NULL;
	
	float* weight_out_4 = NULL;
	float* bias_out_4 = NULL;
	
	head = lw_read_head();

	weight_out_1 = lw_weight_multiply(head, NULL, layer_1, &addr); 
	bias_out_1 = lw_bias_add(head, weight_out_1, layer_1, &addr); 
	relu_out_1 = lw_relu_active(head, bias_out_1, layer_1); 

	weight_out_2 = lw_weight_multiply(head, relu_out_1, layer_2, &addr);
	bias_out_2 = lw_bias_add(head, weight_out_2, layer_2, &addr);
	relu_out_2 = lw_relu_active(head, bias_out_2, layer_2);
	free(weight_out_1);
	weight_out_1 = NULL;
	bias_out_1 = NULL;
	relu_out_1 = NULL;

	weight_out_3 = lw_weight_multiply(head, relu_out_2, layer_3, &addr);
	bias_out_3 = lw_bias_add(head, weight_out_3, layer_3, &addr);
	relu_out_3 = lw_relu_active(head, bias_out_3, layer_3);
	free(weight_out_2);
	weight_out_2 = NULL;
	bias_out_2 = NULL;
	relu_out_2 = NULL;

	weight_out_4 = lw_weight_multiply(head, relu_out_3, layer_4, &addr);	
	bias_out_4 = lw_bias_add(head, weight_out_4, layer_4, &addr);
	free(weight_out_3);
	weight_out_3 = NULL;
	bias_out_3 = NULL;
	relu_out_3 = NULL;

	free(head);
	return bias_out_4; 
}


float* lw_weight_multiply(lw_head_t* head, float* indata, lw_layer_t layer, uint32_t *addr)
{
	uint32_t col = 0; 
	uint32_t layer_out = 0; 
	uint32_t base_addr = 0; 

		
	switch(layer)
	{
		case 1:
			col = head->feature_len; 
		  layer_out = head->layer_1_out; 
			base_addr = (uint32_t)FLASH_DATA_BASE_ADDR + sizeof(float) * (head->head_len); 
			indata = (float*)((uint32_t)(FLASH_DATA_BASE_ADDR + (uint32_t)(sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out * (head->layer_1_out + 1) + head->layer_3_out *(head->layer_2_out + 1) + head->feature_out *(head->layer_3_out + 1)))));
		break;

		case 2:
			col = head->layer_1_out;
			layer_out = head->layer_2_out;
			base_addr = (uint32_t)FLASH_DATA_BASE_ADDR + sizeof(float) * (head->head_len + head->layer_1_out *(head->feature_len + 1)); 
		break;

		case 3:
			col = head->layer_2_out;
			layer_out = head->layer_3_out;
			base_addr = (uint32_t)FLASH_DATA_BASE_ADDR + sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out *(head->layer_1_out + 1));
		break;

		case 4:
			col = head->layer_3_out;
			layer_out = head->feature_out;
			base_addr = (uint32_t)FLASH_DATA_BASE_ADDR + sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out * (head->layer_1_out + 1) + head->layer_3_out *(head->layer_2_out + 1));
		break;
	}

	float* weight_out = (float*)calloc(sizeof(float), layer_out); 
	if (weight_out != 	NULL)
	{
		for(uint32_t i = 0; i < layer_out; i++)
		{
			for (uint32_t j = 0; j < col; j++)
			{
					weight_out[i] = weight_out[i] + *(float*)(base_addr + 4 *(col *i + j)) * indata[j]; 
			}
		}
		
		*addr = base_addr + 4 *(layer_out * col); 
	}
	else
	{
			printf("Weight out malloc fial!\r\n");
	}

		return weight_out;
}


float* lw_bias_add(lw_head_t* head, float* indata, lw_layer_t layer, uint32_t *addr)
{
	uint32_t layer_out = 0; 

	switch(layer)
	{
		case 1:
		  layer_out = head->layer_1_out; 
		break;

		case 2:
			layer_out = head->layer_2_out;
		break;

		case 3:
			layer_out = head->layer_3_out;
		break;

		case 4:
			layer_out = head->feature_out;
		break;
	}
	
	for(uint32_t i = 0; i < layer_out; i++)
	{
		indata[i] = indata[i] + *(float *)(*addr + 4 *i);
	}
	
	return indata;
}


float* lw_relu_active(lw_head_t* head, float* indata, lw_layer_t layer)
{
	uint32_t layer_out = 0; 

	switch(layer)
	{
		case 1:
		  layer_out = head->layer_1_out; 
		break;

		case 2:
			layer_out = head->layer_2_out;
		break;

		case 3:
			layer_out = head->layer_3_out;
		break;

		case 4:
			layer_out = head->feature_out;
		break;
	}
	
	for (uint16_t i = 0; i < layer_out; i++)
	{
		if (indata[i] > 0)
		{
			indata[i] = indata[i];
		}
		else
		{
			indata[i] = 0;
		}
	}
	
	return indata;
}


lw_head_t* lw_read_head()
{
	lw_head_t* head = (lw_head_t*)calloc(sizeof(lw_head_t), 1); 
	if (head != NULL)
	{
		lw_read_flash((float*)head, sizeof(lw_head_t)/sizeof(float), 0); 
	}
	else
	{
		printf("Head malloc fial!\r\n");
	}

	return head;
}


float* lw_read_sample(lw_head_t* head)
{
	uint32_t seek = sizeof(float)* (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out * (head->layer_1_out + 1) + head->layer_3_out *(head->layer_2_out + 1) + head->feature_out * (head->layer_3_out + 1));

	float* feature_data = (float*)malloc(sizeof(float) * (head->feature_len + 1));

	if (feature_data != NULL)
	{
		lw_read_flash(feature_data, (head->feature_len + 1), seek); 
	}
	else
	{
		printf("feature data malloc fial!\r\n");
	}

	return feature_data;
}


void lw_read_flash(float* read_buffer, uint32_t read_len, uint32_t seek)
{
	uint32_t flash_addr = (uint32_t)FLASH_DATA_BASE_ADDR + seek;
	for(uint32_t i = 0; i < read_len; i++)
	{
		read_buffer[i] = *(float*)(flash_addr);
		flash_addr += 4;
	}
}



