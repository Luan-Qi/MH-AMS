#include "USB_debug.h"

__ASM(".global __use_no_semihosting");
//__ASM(".global _main_redirection");
const char __stdin_name[150];
const char __stdout_name[150];
const char __stderr_name[150];
typedef int FILEHANDLE;
typedef unsigned int clock_t;    /* cpu time type */
typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */
extern FILE __stdout;

void _sys_exit(int status){while(1);}
FILEHANDLE _sys_open(const char *name, int openmode){return 0;}
int _sys_close(FILEHANDLE fh){return 0;}
int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode){return 0;}
int _sys_read(FILEHANDLE fh, unsigned char*buf, unsigned len, int mode){return 0;}
int _sys_istty(FILEHANDLE fh){return 0;}
int _sys_seek(FILEHANDLE fh, long pos){return 0;}
int _sys_ensure(FILEHANDLE fh){return 0;}
long _sys_flen(FILEHANDLE fh){return 0;}
int _sys_tmpnam(char *name, int fileno, unsigned maxlength){return 0;}
void _ttywrch(int ch){}
time_t time(time_t *t){return 0;}
int remove(const char *filename){return 0;}
int rename(const char *oldname, const char *newname){return 0;}
int system(const char *command){return 0;}
char *_sys_command_string(char *cmd, int len){return 0;}
clock_t clock(void){return 0;}
 
int fputc(int ch, FILE *f)
{
	while(usart_flag_get(USART2, USART_TDBE_FLAG)== RESET);
	usart_data_transmit(USART2, ch);
	return ch;
}

void debug_Bambus_echo(unsigned char _RX_IRQ_data)
{
	while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET){};
	usart_data_transmit(USART2, _RX_IRQ_data);
	while(usart_flag_get(USART2, USART_TDC_FLAG) == RESET){};
}

