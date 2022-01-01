// Testone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Testone.h"



/** For Scheduling the thread execution order, Evens are enough, no synchronization objects like Mutex, Critical Section etc.. are not needed. Below is a code and which is scalable based on the number of threads entered. */

//Code for Schedule threads execution in a order T1->T2->T3-> ..->Tn->T1..
// where T1 is Thread-1, T2 is Thread-2 .. Tn is Thread-N

#include <iostream>
#include <Windows.h>

HANDLE  * EventsHandlesArray;    
HANDLE * ThreadHandlesArray;
int arr[3]={1,0,2};
int counter=0;
int iMaxWaitTime = 10000; // 10 Secs    
int iThreadWaitTimeInterval = 1000; //1 sec

int iNumberOfThread = 3; // You can change this no. of thread value within the permitted limit by your system :)

DWORD WINAPI ThreadFun( LPVOID lpParam )

{
 
	int arrVal = arr[counter];
    while(1)
    {	

       if(::WaitForSingleObject(EventsHandlesArray[arrVal],INFINITE) == WAIT_OBJECT_0)
        {
            
                    std::cout<<" I am a thread "<<arrVal<<std::endl;
                    ResetEvent ( EventsHandlesArray[arrVal]);
                    Sleep (iThreadWaitTimeInterval);
					counter=counter+1;//here ++counter would set the next priority thread from arr in signaled state
					arrVal = arr[counter];
					if(arrVal<iNumberOfThread)
						SetEvent (  EventsHandlesArray[arrVal]);
					else break;
					
        }
    }
	return 0L;
}

int main(void)

{       
__try
    {
       std::cout<<"Running Main, Creating Events"<<std::endl;
       EventsHandlesArray = new HANDLE[iNumberOfThread];
       ThreadHandlesArray = new HANDLE[iNumberOfThread];

        for(int iCount = 0; iCount<iNumberOfThread; iCount++)
        {
            EventsHandlesArray[iCount] = CreateEvent ( NULL , true , false , NULL );
        }

        for(int iCount = 0; iCount<iNumberOfThread; iCount++)
        {
            if( EventsHandlesArray[iCount] == INVALID_HANDLE_VALUE)
            {
                std::cout<<"Problem with Event Creation"<<std::endl;
                for(int iCount =0;iCount<iNumberOfThread;iCount++)
                {
                    CloseHandle(EventsHandlesArray[iCount]); 
                }
                return 0;
            }
        }

        DWORD Id=0;
        int iThreadIndex = 1;

        for(int iCount = 0; iCount<iNumberOfThread; iCount++)
        {
            iThreadIndex = iCount+1;
            ThreadHandlesArray[iCount] = CreateThread ( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFun,(LPVOID)iCount ,CREATE_SUSPENDED,&Id );
            std::cout<<"Thread Created : "<<Id<<std::endl;
        }

        bool bThreadCreatedSuccessfully = true;

        for(int iCount = 0; iCount<iNumberOfThread; iCount++)
        {
            if( ThreadHandlesArray[iCount] == INVALID_HANDLE_VALUE || ThreadHandlesArray[iCount] == NULL)
            {
                bThreadCreatedSuccessfully = false;
                break;
            }
        }

        if(bThreadCreatedSuccessfully)
        {
            std::cout<<"Resuming Threads "<<std::endl;
            for(int iCount =0;iCount<iNumberOfThread;iCount++)
            {
                //Try to close the event handles
                ResumeThread(ThreadHandlesArray[iCount]);
            }

            Sleep (iThreadWaitTimeInterval);
			
            SetEvent ( EventsHandlesArray[arr[0]]);
            WaitForMultipleObjects(iNumberOfThread,ThreadHandlesArray, TRUE, INFINITE);
        }
        else
        {
            std::cout<<"Issue with Thread Creation"<<std::endl;
        }
    }

    __finally 
    {
        //Close Threads & Events
        for(int iCount=0;iCount<iNumberOfThread;iCount++)
        {
            //Try to close the event handles
            CloseHandle(ThreadHandlesArray[iCount]);
            CloseHandle(EventsHandlesArray[iCount]);
        }
        std::cout<<"  .... Exiting the program"<<std::endl;
    }
    return 0;
}