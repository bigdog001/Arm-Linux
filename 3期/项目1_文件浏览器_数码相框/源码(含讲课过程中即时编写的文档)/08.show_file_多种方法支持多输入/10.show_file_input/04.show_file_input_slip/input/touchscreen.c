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

	if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL ) 
	{
		g_tTSDev = ts_open(pcTSName, 0);  /* ��������ʽ�� */
	}
	else
	{
		g_tTSDev = ts_open("/dev/event0", 1);
	}

	if (!g_tTSDev) {
		DBG_PRINTF("ts_open error!\n");
		return -1;
	}

	if (ts_config(g_tTSDev)) {
		DBG_PRINTF("ts_config error!\n");
		return -1;
	}

	if (GetDispResolution(&giXres, &giYres))
	{
		return -1;
	}

	return 0;
}

static int TouchScreenDevExit(void)
{
	return 0;
}


static int isOutOf500ms(struct timeval *ptPreTime, struct timeval *ptNowTime)
{
	int iPreMs;
	int iNowMs;
	
	iPreMs = ptPreTime->tv_sec * 1000 + ptPreTime->tv_usec / 1000;
	iNowMs = ptNowTime->tv_sec * 1000 + ptNowTime->tv_usec / 1000;

	return (iNowMs > iPreMs + 500);
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
			if ((tSamp.pressure > 0) && (bStart == 0))
			{
				/* �հ��� */
				/* ��¼�տ�ʼѹ�µĵ� */
				tSampPressed = tSamp;
				bStart = 1;
			}
			
			if (tSamp.pressure <= 0)
			{
				/* �ɿ� */
				tSampReleased = tSamp;

				/* �������� */
				if (!bStart)
				{
					return -1;
				}
				else
				{
					iDelta = tSampReleased.x - tSampPressed.x;
					ptInputEvent->tTime = tSampReleased.tv;
					ptInputEvent->iType = INPUT_TYPE_TOUCHSCREEN;
					
					if (iDelta > giXres/5)
					{
						/* ������һҳ */
						ptInputEvent->iVal = INPUT_VALUE_UP;
					}
					else if (iDelta < 0 - giXres/5)
					{
						/* ������һҳ */
						ptInputEvent->iVal = INPUT_VALUE_DOWN;
					}
					else
					{
						ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
					}
					return 0;
				}
			}
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

