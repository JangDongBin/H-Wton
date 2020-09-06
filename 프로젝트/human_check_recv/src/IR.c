#include <peripheral_io.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "log.h"
#include "IR.h"

static peripheral_gpio_h g_sensor_h = NULL;
static int g_pin_num = -1;
static resource_ir_sensor_interrupted_data *g_interrupted_data = NULL;


/*센서 사용 준비 확인*/
int _resource_validate_ir_sensor(int pin_num)
{
	int ret = IR_HANDLE_ERROR_NONE;

	if(!g_sensor_h)
	{
		ret = IR_HANDLE_ERROR_NOT_OPEN;
	}
	else if(g_pin_num != pin_num)
	{
		ret = IR_HANDLE_ERROR_INVALID_PIN;
	}

	return ret;
}

/*센서 사용*/
int resource_open_ir_sensor(int pin_num)
{
	peripheral_gpio_h temp = NULL;

	int ret = peripheral_gpio_open(pin_num, &temp);

	if(ret)
	{
		peripheral_gpio_close(temp);
		_E("peripheral_gpio_open_failed.");
		return -1;
	}

	ret = peripheral_gpio_set_direction(temp, PERIPHERAL_GPIO_DIRECTION_IN);

	if(ret)
	{
		peripheral_gpio_close(temp);
		_E("peripheral_gpio_set_direction_failed.");
		return -1;
	}

	g_sensor_h = temp;
	g_pin_num = pin_num;

	return 0;
}

/*센서 데이터 읽어오기*/
int resource_read_ir_sensor(int pin_num, uint32_t *out_value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if(_resource_validate_ir_sensor(pin_num) == IR_HANDLE_ERROR_NOT_OPEN)
	{
		ret = resource_open_ir_sensor(pin_num);
		retv_if(ret<0,-1);
	}
	if(_resource_validate_ir_sensor(pin_num) == IR_HANDLE_ERROR_INVALID_PIN)
	{
		_E("Invalid pin number.");
		return -1;
	}

	ret = peripheral_gpio_read(g_sensor_h, out_value);
	retv_if(ret < 0, -1);

	return 0;
}

/*센서 사용 종료*/
void resource_close_ir_sensor(void)
{
	if(!g_sensor_h)
	{
		return;
	}
	_I("Infrared ir sensor is finishing...");

	peripheral_gpio_close(g_sensor_h);

	if(g_interrupted_data != NULL)
	{
		free(g_interrupted_data);
		g_interrupted_data = NULL;
	}
	g_sensor_h = NULL;
	g_pin_num = -1;
}

/*센서 인터럽트 콜백*/
void _resource_ir_interrupted_cb(peripheral_gpio_h gpio, peripheral_error_e error, void *user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if(!g_sensor_h)
		return;

	if(g_interrupted_data->is_called_first_time)
	{
		ret = peripheral_gpio_read(g_sensor_h, &g_interrupted_data->ir_value);
		if(ret)
			return;

		g_interrupted_data->is_called_first_time = 0;
	}
	else
	{
		g_interrupted_data->ir_value = !g_interrupted_data->ir_value;
	}

	g_interrupted_data->interrupted_cb(g_interrupted_data->ir_value, g_interrupted_data->interrupted_cb_user_data);
}

/*IR 센서 인터럽트 셋 콜백*/
int resource_set_interrupted_cb_ir_sensor(int pin_num, resource_ir_sensor_interrupted_cb interrupted_cb, void *user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;
	if(g_interrupted_data == NULL)
	{
		g_interrupted_data = calloc(1, sizeof(resource_ir_sensor_interrupted_data));
	}

	if(_resource_validate_ir_sensor(pin_num) == IR_HANDLE_ERROR_NOT_OPEN)
	{
		ret = resource_open_ir_sensor(pin_num);
		retv_if(ret < 0, -1);
	}

	if(_resource_validate_ir_sensor(pin_num) == IR_HANDLE_ERROR_INVALID_PIN)
	{
		_E("Invalid pin number.");
		return -1;
	}

	g_interrupted_data->interrupted_cb = interrupted_cb;
	g_interrupted_data->interrupted_cb_user_data = user_data;
	g_interrupted_data->ir_value = 0;
	g_interrupted_data->is_called_first_time = 1;

	ret = peripheral_gpio_set_edge_mode(g_sensor_h, PERIPHERAL_GPIO_EDGE_RISING);
	retv_if(ret < 0, -1);

	ret = peripheral_gpio_set_interrupted_cb(g_sensor_h, _resource_ir_interrupted_cb, g_interrupted_data);
	retv_if(ret < 0, -1);

	return 0;
}

/*IR 센서 인터럽트 언셋 콜백*/
int resource_unset_interrupted_cb_ir_sensor(int pin_num)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if(g_interrupted_data != NULL)
	{
		free(g_interrupted_data);
		g_interrupted_data = NULL;
	}

	if(_resource_validate_ir_sensor(pin_num) == IR_HANDLE_ERROR_NOT_OPEN)
	{
		_E("No open handle.");
		return -1;
	}

	else if(_resource_validate_ir_sensor(pin_num) == IR_HANDLE_ERROR_INVALID_PIN)
	{
		_E("Invalid pin number");
		return -1;
	}

	ret = peripheral_gpio_unset_interrupted_cb(g_sensor_h);
	retv_if(ret < 0, -1);

	return 0;
}
