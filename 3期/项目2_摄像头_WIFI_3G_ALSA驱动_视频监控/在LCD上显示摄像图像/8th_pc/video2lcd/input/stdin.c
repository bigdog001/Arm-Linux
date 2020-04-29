
#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>


static int StdinDevInit(void)
{
    struct termios tTTYState;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn off canonical mode
    tTTYState.c_lflag &= ~ICANON;
    //minimum of number input read.
    tTTYState.c_cc[VMIN] = 1;   /* ��һ������ʱ�����̷��� */

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	return 0;
}

static int StdinDevExit(void)
{

    struct termios tTTYState;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn on canonical mode
    tTTYState.c_lflag |= ICANON;
	
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);	
	return 0;
}

static int StdinGetInputEvent(PT_InputEvent ptInputEvent)
{
	/* ��������ݾͶ�ȡ����������
	 * ���û������, ���̷���, ���ȴ�
	 */

	/* select, poll ���Բ��� UNIX�����߼���� */

    struct timeval tTV;
    fd_set tFDs;
	char c;
	
    tTV.tv_sec = 0;
    tTV.tv_usec = 0;
    FD_ZERO(&tFDs);
	
    FD_SET(STDIN_FILENO, &tFDs); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &tFDs, NULL, NULL, &tTV);
	
    if (FD_ISSET(STDIN_FILENO, &tFDs))
    {
		/* �������� */
		ptInputEvent->iType = INPUT_TYPE_STDIN;
		gettimeofday(&ptInputEvent->tTime, NULL);
		
		c = fgetc(stdin);
		if (c == 'u')
		{
			ptInputEvent->iVal = INPUT_VALUE_UP;
		}
		else if (c == 'n')
		{
			ptInputEvent->iVal = INPUT_VALUE_DOWN;
		}
		else if (c == 'q')
		{
			ptInputEvent->iVal = INPUT_VALUE_EXIT;
		}
		else
		{
			ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
		}		
		return 0;
    }
	else
	{
		return -1;
	}
}

static T_InputOpr g_tStdinOpr = {
	.name          = "stdin",
	.DeviceInit    = StdinDevInit,
	.DeviceExit    = StdinDevExit,
	.GetInputEvent = StdinGetInputEvent,
};


int StdinInit(void)
{
	return RegisterInputOpr(&g_tStdinOpr);
}

