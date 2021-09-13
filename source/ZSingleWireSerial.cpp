#include "CodalConfig.h"
#include "CodalDmesg.h"
#include "ZSingleWireSerial.h"
#include "Event.h"

#include "stdio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

using namespace codal;

#define STATUS_IDLE 0
#define STATUS_TX 0x10
#define STATUS_RX 0x20

int dmachTx = -1;
int dmachRx = -1;
extern "C"
{
    static ZSingleWireSerial *inst;

    void isr_uart1(void *p)
    {
        
    }
}

ZSingleWireSerial::ZSingleWireSerial(Pin &p) : DMASingleWireSerial(p)
{
    inst = this;
    pinTx = p.name & 0xfe;
    pinRx = (p.name & 0xfe) + 1;

    uart_init(uart1, 1000000);
    gpio_set_function(pinTx, GPIO_FUNC_SIO);
    gpio_set_function(pinRx, GPIO_FUNC_SIO);
    gpio_pull_up(pinRx);

    uart_set_hw_flow(uart1, false, false);
    uart_set_fifo_enabled(uart1, false);

    // uart_set_irq_enables(uart1, true, false);
    // irq_set_enabled(UART1_IRQ, true);

}

int ZSingleWireSerial::setMode(SingleWireMode sw)
{
    // either enable rx or tx program
    if (sw == SingleWireRx)
    {
        status = STATUS_RX;
        gpio_set_function(pinTx, GPIO_FUNC_SIO);
        gpio_set_function(pinRx, GPIO_FUNC_UART);
    }
    else if (sw == SingleWireTx)
    {
        status = STATUS_TX;
        gpio_set_function(pinTx, GPIO_FUNC_UART);
        gpio_set_function(pinRx, GPIO_FUNC_SIO);
    }
    else
    {
        status = STATUS_IDLE;
        // release gpio
        gpio_set_function(pinTx, GPIO_FUNC_SIO);
        gpio_set_function(pinRx, GPIO_FUNC_SIO);
    }

    return DEVICE_OK;
}

void ZSingleWireSerial::configureRxInterrupt(int enable) {}

int ZSingleWireSerial::configureTx(int enable)
{
    return setMode(enable ? SingleWireTx : SingleWireDisconnected);
}

int ZSingleWireSerial::configureRx(int enable)
{
    return setMode(enable ? SingleWireRx : SingleWireDisconnected);
}

int ZSingleWireSerial::putc(char c)
{
    return DEVICE_NOT_IMPLEMENTED;
}

int ZSingleWireSerial::getc()
{
    return DEVICE_NOT_IMPLEMENTED;
}

int ZSingleWireSerial::send(uint8_t *data, int len)
{
    return DEVICE_NOT_IMPLEMENTED;
}

int ZSingleWireSerial::receive(uint8_t *data, int len)
{
    return DEVICE_NOT_IMPLEMENTED;
}

int ZSingleWireSerial::setBaud(uint32_t baud)
{
    baudrate = baud;
    return DEVICE_OK;
}

uint32_t ZSingleWireSerial::getBaud()
{
    return baudrate;
}

int ZSingleWireSerial::sendDMA(uint8_t *data, int len)
{
    if (status != STATUS_TX)
        setMode(SingleWireTx);

    while(len){
        uart_putc_raw(uart1, *data);
        data++;
        len--;
    }

    return DEVICE_OK;
}

int ZSingleWireSerial::receiveDMA(uint8_t *data, int len)
{
    if (status != STATUS_RX)
        setMode(SingleWireRx);
    uart_read_blocking(uart1, data, len);
    return DEVICE_OK;
}

int ZSingleWireSerial::abortDMA()
{
    if (!(status & (STATUS_RX | STATUS_TX)))
        return DEVICE_INVALID_PARAMETER;

    setMode(SingleWireDisconnected);

    return DEVICE_OK;
}

int ZSingleWireSerial::sendBreak()
{
    return DEVICE_NOT_IMPLEMENTED;
}

int ZSingleWireSerial::getBytesReceived()
{
    return DEVICE_NOT_IMPLEMENTED;
}

int ZSingleWireSerial::getBytesTransmitted()
{
    return DEVICE_NOT_IMPLEMENTED;
}
