#include <config.h>
#include <input_manager.h>
#include <stdlib.h>

#include <tslib.h>
#include <draw.h>

/* �ο�tslib���ts_print.c */

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;

/* ע��: ����Ҫ�õ�LCD�ķֱ���, �˺���Ҫ��SelectAndInitDisplay֮����� */
static int TouchScreenDevInit(void)
{
	char *pcTSName = NULL;
	int iBpp;

	if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL ) 
	{
		g_tTSDev = ts_open(pcTSName, 0);  /* ��������ʽ�� */
	}
	else
	{
		g_tTSDev = ts_open("/dev/event0", 1);
	}

	if (!g_tTSDev) {
		DBG_PRINTF(APP_ERR"ts_open error!\n");
		return -1;
	}

	if (ts_config(g_tTSDev)) {
		DBG_PRINTF("ts_config error!\n");
		return -1;
	}

	if (GetDispResolution(&giXres, &giYres, &iBpp))
	{
		return -1;
	}

	return 0;
}

static int TouchScreenDevExit(void)
{
	return 0;
}

static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
	struct ts_sample tSamp;
	struct ts_sample tSampPressed;
	struct ts_sample tSampReleased;
	int iRet;
	int bStart = 0;
	int iDelta;

	static struct timeval tPreTime;
	

	while (1)
	{
		iRet = ts_read(g_tTSDev, &tSamp, 1); /* ��������������� */
		if (iRet == 1)
		{
			ptInputEvent->tTime     = tSamp.tv;
			ptInputEvent->iType     = INPUT_TYPE_TOUCHSCREEN;
			ptInputEvent->iX        = tSamp.x;
			ptInputEvent->iY        = tSamp.y;
			ptInputEvent->iPressure = tSamp.pressure;
			return 0;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}


static T_InputOpr g_tTouchScreenOpr = {
	.name          = "touchscreen",
	.DeviceInit    = TouchScreenDevInit,
	.DeviceExit    = TouchScreenDevExit,
	.GetInputEvent = TouchScreenGetInputEvent,
};

int TouchScreenInit(void)
{
	return RegisterInputOpr(&g_tTouchScreenOpr);
}

