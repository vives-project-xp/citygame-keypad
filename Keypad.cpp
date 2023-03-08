#include "Keypad.h"

void Keypad::_cbRow0Rise()
{
    _checkIndex(0, _rows[0]);
}

void Keypad::_cbRow1Rise()
{
    _checkIndex(1, _rows[1]);
}

void Keypad::_cbRow2Rise()
{
    _checkIndex(2, _rows[2]);
}

void Keypad::_cbRow3Rise()
{
    _checkIndex(3, _rows[3]);
}

void Keypad::_setupRiseTrigger()
{
    if (_rows[0])
    {
        _rows[0]->rise(queue->event(this, &Keypad::_cbRow0Rise));
    }

    if (_rows[1])
    {
        _rows[1]->rise(mbed::Callback<void()>(this, &Keypad::_cbRow1Rise));
    }

    if (_rows[2])
    {
        _rows[2]->rise(mbed::Callback<void()>(this, &Keypad::_cbRow2Rise));
    }

    if (_rows[3])
    {
        _rows[3]->rise(mbed::Callback<void()>(this, &Keypad::_cbRow3Rise));
    }
}

Keypad::Keypad(EventQueue *queue, PinName r0, PinName r1, PinName r2, PinName r3, PinName c0, PinName c1, PinName c2, PinName c3, int debounce_ms)
{
    this->queue = queue;
    PinName rPins[4] = {r0, r1, r2, r3};
    PinName cPins[4] = {c0, c1, c2, c3};

    for (int i = 0; i < 4; i++)
    {
        _rows[i] = NULL;
        _cols[i] = NULL;
    }

    _nRow = 0;
    for (int i = 0; i < 4; i++)
    {
        if (rPins[i] != NC)
        {
            _rows[i] = new InterruptIn(rPins[i]);
            _nRow++;
        }
        else
            break;
    }
    _setupRiseTrigger();

    _nCol = 0;
    for (int i = 0; i < 4; i++)
    {
        if (cPins[i] != NC)
        {
            _cols[i] = new DigitalOut(cPins[i]);
            _nCol++;
        }
        else
            break;
    }

    _debounce = debounce_ms;
}

Keypad::~Keypad()
{
    for (int i = 0; i < 4; i++)
    {
        if (_rows[i] != 0)
            delete _rows[i];
    }

    for (int i = 0; i < 4; i++)
    {
        if (_cols[i] != 0)
            delete _cols[i];
    }
}

void Keypad::start()
{
    for (int i = 0; i < _nCol; i++)
        _cols[i]->write(1);
}

void Keypad::stop()
{
    for (int i = 0; i < _nCol; i++)
        _cols[i++]->write(0);
}

void Keypad::attach(mbed::Callback<void(uint32_t)> cb)
{
    _callback = cb;
}

void Keypad::_checkIndex(int row, InterruptIn *therow)
{
    wait_us(_debounce * 1000);

    if (therow->read() == 0)
        return;

    int c;
    for (c = 0; c < _nCol; c++)
    {
        _cols[c]->write(0); // de-energize the column
        if (therow->read() == 0)
        {
            break;
        }
    }

    int index = row * _nCol + c;
    _callback(index);
    start(); // Re-energize all columns
}
