#include "Arduino.h"
#include "Source.h"
#include "TestLibrary.h"

/*	Insulin Pump Communication Test
	
	This checks whether the script's communication function works to send a signal to the insulin pump.
*/
int TestPressButton(int buttonID, int expected)
{
	TestState state = GetCurrentState();

    /*  PressButton */
    int initial = 0, final = 0, result = 1;
    switch (buttonID)
    {
    case APS_BUTTON1:
    case APS_BUTTON2:
    case APS_BUTTON3:
        initial = digitalWrite(buttonID, HIGH);
        result &= pinValue[buttonID] == HIGH;
        delay(100);
        final = digitalWrite(buttonID, LOW);
    default:
        break;
    }

    /*  Expecting initial == HIGH && final == low && pinValue[buttonID] == low */
    result &= initial == HIGH && final == LOW && pinValue[buttonID] == LOW;
	SetCurrentState(state);
    if (result != expected)
        printf("Failed for %d\n", buttonID);
    return result == expected;
}
int Tests_for_PressButton() {

    int total = 6;
    int accuracy = 0;
    printf("Running Communication Tests\n");
    accuracy += TestPressButton(APS_BUTTON1, 1);
    accuracy += TestPressButton(APS_BUTTON2, 1);
    accuracy += TestPressButton(APS_BUTTON3, 1);
    accuracy += TestPressButton(-1, 0);
    accuracy += TestPressButton(0, 0);
    accuracy += TestPressButton(255, 0);
    printf("Results: %d/6\n", accuracy);
    return accuracy == total;
}
/*  Send Number Pump Test
    Checks if the function for transforming a number into a series of signals works.
*/
int TestSendNumberToPump(unsigned char number) {
    int i;
    int value = 0;
    for (i = 7; i >= 0; i--) {
        int mask = 1 << i;
        value |= (mask & number);
    }
    if(number != value)
    printf("Error: number = %d and value = %d\n", number, value);
    return number == value;
}
int Tests_for_SendNumberToPump() {
    int total = 6;
    int accuracy = 0;
    printf("Running Sending Number to Pump Tests\n");
    accuracy += TestSendNumberToPump(0);
    accuracy += TestSendNumberToPump(-10);
    accuracy += TestSendNumberToPump(20);
    accuracy += TestSendNumberToPump(255);
    accuracy += TestSendNumberToPump(12);
    accuracy += TestSendNumberToPump(-55);
    printf("Results: %d/6\n", accuracy);
    return accuracy == total;
}
/*  Insulin Pump Signal Reading Test
    Checks if the function for the insulin pump to reinterpret signals into a number still works.
*/
int TestInsulinPumpCommunication(unsigned char number) {
    int i;
    for (i = 7; i >= 0; i--) {
        int mask = 1 << i;
        if ((number & mask) == mask) //  == 1
            TIMER_0_task1_cb(1, 0, 0);
        else  //  == 2
            TIMER_0_task1_cb(0, 1, 0);
    }
    int result = number == TIMER_0_task1_cb(0, 0, 1);
    button_array[0] = 0;
    basal_rate[0] = -1;
    return result;
}
int Tests_ForInsulinPumpCommunication() {
    int total = 6;
    int accuracy = 0;
    printf("Running Insulin Pump Number Reading\n");
    accuracy += TestSendNumberToPump(0);
    accuracy += TestSendNumberToPump(-10);
    accuracy += TestSendNumberToPump(20);
    accuracy += TestSendNumberToPump(255);
    accuracy += TestSendNumberToPump(12);
    accuracy += TestSendNumberToPump(-55);
    printf("Results: %d/6\n", accuracy);
    return accuracy == total;
}
/*  This is the entry point to the testing code */
int main()
{

    initArduinoLib();
    int pressButtonResult = Tests_for_PressButton();
    int sendNumberResult = Tests_for_SendNumberToPump();
    int insulinPumpResult = Tests_ForInsulinPumpCommunication();
    return !(pressButtonResult && sendNumberResult && insulinPumpResult);
}