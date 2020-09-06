#ifndef IR_H_
#define IR_H_

typedef enum {
	IR_HANDLE_ERROR_NONE = 0,
	IR_HANDLE_ERROR_NOT_OPEN,
	IR_HANDLE_ERROR_INVALID_PIN
}resource_ir_sensor_handle_error_e;

typedef void (*resource_ir_sensor_interrupted_cb) (unsigned int ir_value, void *user_data);

typedef struct resource_ir_sensor_interrupted_data_s{
	resource_ir_sensor_interrupted_cb interrupted_cb;
	void *interrupted_cb_user_data;
	unsigned int ir_value;
	int is_called_first_time;
}resource_ir_sensor_interrupted_data;

extern int resource_read_ir_sensor(int pin_num, u_int32_t *out_value);

extern void resource_close_ir_sensor(void);

extern int resource_set_interrupted_cb_ir_sensor(int pin_num, resource_ir_sensor_interrupted_cb interrupted_cb, void *user_data);

extern int resource_unset_interrupted_cb_ir_sensor(int pin_num);

#endif /* IR_H_ */
