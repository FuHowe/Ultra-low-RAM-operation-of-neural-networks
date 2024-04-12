#include "main.h"

uint32_t seek = 0;
float* lw_model(void)
{
	lw_head_t* head = NULL;

	float* feature_data = NULL;

	float* weight_out_1 = NULL;
	float* weight_out_2 = NULL;
	float* weight_out_3 = NULL;

	float* bais_out_1 = NULL;
	float* bais_out_2 = NULL;
	float* bais_out_3 = NULL;

	float* relu_out_1 = NULL;
	float* relu_out_2 = NULL;
	float* relu_out_3 = NULL;
	
	float* model_out = NULL;

	head = lw_read_head();
	feature_data = lw_read_sample(head);
	weight_out_1 = lw_weight_multiply


	liner_1_out = lw_liner(head, feature_data, layer_1);
	relu_1_out = lw_relu(head, liner_1_out, layer_1);
	free(liner_1_out);


	liner_2_out = lw_liner(head, relu_1_out, layer_2);
	free(relu_1_out);
	relu_2_out = lw_relu(head, liner_2_out, layer_2);
	free(liner_2_out);


	liner_3_out = lw_liner(head, relu_2_out, layer_3);
	free(relu_2_out);
	relu_3_out = lw_relu(head, liner_3_out, layer_3);
	free(liner_3_out);


	model_out = lw_liner(head, relu_3_out, layer_4);
	free(relu_3_out);
	free(head);


	return model_out;
}




float* lw_liner(lw_head_t* head, float* indata, lw_layer_t layer)
{
	float* weight_data = NULL;	float* weight_out = NULL;	float* bias_data = NULL;	float* liner_out = NULL;
	weight_data = lw_read_weight(head, layer);	weight_out = lw_weight_multiply(head, weight_data, indata, layer);	free(indata);
	free(weight_data);
	
	bias_data = lw_read_bias(head, layer);	liner_out = lw_bias_add(head, weight_out, bias_data, layer);	free(weight_out);
	free(bias_data);

	return liner_out;
}


void lw_read_flash(float* read_buffer, uint32_t read_len, uint32_t seek)
{
	uint32_t flash_addr = (uint32_t)FLASH_DATA_BASE_ADDR + seek;
	for(uint32_t i = 0; i < read_len; i++)
	{
		read_buffer[i] = *(float*)(flash_addr);
		flash_addr += 4;	}
}


lw_head_t* lw_read_head(void)
{
	lw_head_t* head = (lw_head_t*)malloc(sizeof(lw_head_t));	if (head != NULL)
	{
		#if (RUN_PLATFORM == 1)
			FILE* file_read;
			file_read = fopen("weight_bias_sample_label.bin", "rb");			fread(head, sizeof(lw_head_t), 1, file_read);			fclose(file_read);		#else
			lw_read_flash((float*)head, sizeof(lw_head_t)/sizeof(float), 0);		#endif
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
		#if (RUN_PLATFORM == 1)
			FILE* file_read;

			file_read = fopen("weight_bias_sample_label.bin", "rb");			fseek(file_read, seek, 0);			fread(feature_data, sizeof(float) * (head->feature_len + 1), 1, file_read);			fclose(file_read);		#else
			lw_read_flash(feature_data, (head->feature_len + 1), seek);		#endif
	}
	else
	{
		printf("feature data malloc fial!\r\n");
	}

	return feature_data;
}

float* lw_read_weight(lw_head_t* head, lw_layer_t layer)
{
	uint16_t row = 0;	uint16_t col = 0;	uint32_t seek = 0;
	switch(layer)
	{
		case 1:
			row = head->layer_1_out;
			col = head->feature_len;
			seek = sizeof(float) * (head->head_len);
		break;

		case 2:
			row = head->layer_2_out;
			col = head->layer_1_out;
			seek = sizeof(float) * (head->head_len + head->layer_1_out *(head->feature_len + 1));		break;

		case 3:
			row = head->layer_3_out;
			col = head->layer_2_out;
			seek = sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out *(head->layer_1_out + 1));
		break;

		case 4:
			row = head->layer_3_out;
			col = head->feature_out;
			seek = sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out * (head->layer_1_out + 1) + head->layer_3_out *(head->layer_2_out + 1));
		break;
	}

	float* weight_data = (float*)malloc(sizeof(float) *row *col);	if (weight_data != NULL)
	{
		#if (RUN_PLATFORM == 1)
			FILE* file_read;

			file_read = fopen("weight_bias_sample_label.bin", "rb");			fseek(file_read, seek, 0);			fread(weight_data, sizeof(float) * row * col, 1, file_read);			fclose(file_read);		#else
			lw_read_flash(weight_data, row *col, seek);		#endif
	}
	else
	{
		printf("Layer %d: weight data malloc fial!\r\n",layer);
	}

	return weight_data;
}

float* lw_weight_multiply(lw_head_t* head, float* weight_data, float* indata,  lw_layer_t layer)
{
	uint16_t row = 0;	uint16_t col = 0;
	switch (layer)
	{
		case 1:
			row = head->layer_1_out;
			col = head->feature_len;
			break;

		case 2:
			row = head->layer_2_out;
			col = head->layer_1_out;
			break;

		case 3:
			row = head->layer_3_out;
			col = head->layer_2_out;
			break;

		case 4:
			row = head->feature_out;
			col = head->layer_3_out;
			break;
	}

	float* weight_out = (float*)malloc(sizeof(float) * row);
	if (weight_out != NULL)
	{
		for (uint32_t i = 0; i < row; i++)		{
			float sum = 0;			for (uint32_t j = 0; j < col; j++)			{
				sum += weight_data[col * i + j] * indata[j];
			}
			weight_out[i] = sum;
		}
	}
	else
	{
		printf("Layer %d: weight out malloc fial!\r\n", layer);
	}

	return weight_out;
}

float* lw_read_bias(lw_head_t* head, lw_layer_t layer)
{
	uint16_t bias_len = 0;
	uint32_t seek = 0;
	switch (layer)
	{
		case 1:
			bias_len = head->layer_1_out;
			seek = sizeof(float) * (head->head_len + head->layer_1_out * head->feature_len);			break;

		case 2:
			bias_len = head->layer_2_out;
			seek = sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out * head->layer_1_out);
			break;

		case 3:
			bias_len = head->layer_3_out;
			seek = sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out * (head->layer_1_out + 1) + head->layer_3_out * head->layer_2_out);
			break;

		case 4:
			bias_len = head->feature_out;
			seek = sizeof(float) * (head->head_len + head->layer_1_out * (head->feature_len + 1) + head->layer_2_out * (head->layer_1_out + 1) + head->layer_3_out * (head->layer_2_out + 1) + head->feature_out * head->layer_3_out);
			break;
	}

	float* bias_data = (float*)malloc(sizeof(float) * bias_len);	if (bias_data != NULL)
	{
		#if (RUN_PLATFORM == 1)
			FILE* file_read;
		
			file_read = fopen("weight_bias_sample_label.bin", "rb");			fseek(file_read, seek, 0);			fread(bias_data, sizeof(float) * bias_len, 1, file_read);			fclose(file_read);		#else
					lw_read_flash(bias_data, bias_len, seek);		#endif
	}
	else
	{
		printf("Layer %d: bias data malloc fial!\r\n", layer);
	}

	return bias_data;
}

float* lw_bias_add(lw_head_t* head, float* weight_out, float* indata, lw_layer_t layer)
{
	uint16_t bias_len = 0;

	switch (layer)
	{
		case 1:
			bias_len = head->layer_1_out;
			break;

		case 2:
			bias_len = head->layer_2_out;
			break;

		case 3:
			bias_len = head->layer_3_out;
			break;

		case 4:
			bias_len = head->feature_out;
			break;
	}

	float* bias_out = (float*)malloc(sizeof(float) * bias_len);
	if (bias_out != NULL)
	{
		for (uint16_t i = 0; i < bias_len; i++)
		{
			bias_out[i] = weight_out[i] + indata[i];
		}
	}
	else
	{
		printf("Layer %d: bias out malloc fial!\r\n", layer);
	}

	return bias_out;
}

float* lw_relu(lw_head_t* head, float* indata, lw_layer_t layer)
{
	uint16_t relu_len = 0;

	switch(layer)
	{
		case 1:
			relu_len = head->layer_1_out;
			break;

		case 2:
			relu_len = head->layer_2_out;
			break;

		case 3:
			relu_len = head->layer_3_out;
			break;
		
		case 4:			relu_len = head->feature_out;
		break;
	}

	float* relu_out = (float*)malloc(sizeof(float) * relu_len);
	if (relu_out != NULL)
	{
		for (uint16_t i = 0; i < relu_len; i++)
		{
			if (indata[i] > 0)
			{
				relu_out[i] = indata[i];
			}
			else
			{
				relu_out[i] = 0;
			}
		}
	}
	else
	{
		printf("Layer %d: relu out malloc fial!\r\n", layer);
	}

	return relu_out;
}



