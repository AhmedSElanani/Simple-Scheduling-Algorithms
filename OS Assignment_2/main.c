#include <stdio.h>
#include <stdlib.h>


/******************************************************************************************
    Macros
 ******************************************************************************************/
#define     Size_of_Input_File          50                         // Define the length of the input file that has information about the processes
#define     FCFS_Filename            "outputFCFS.txt"              // Define the Name of the File created in case of FCFS Algorithm
#define     RR_Filename              "outputRR.txt"                // Define the Name of the File created in case of FCFS Algorithm

//  Define All possible Processes States
#define     Running_State               0
#define     Ready_State                 1
#define     Blocked_State               2
#define     Inactive_State              3

//  Define  Queue State
#define     Empty                      'E'

//  Define  I/O State
#define     IO_Finished                  1
#define     IO_NotFinished               0


/******************************************************************************************
    Global Variables
 ******************************************************************************************/
//  Define an Object to hold all Processes information
typedef struct {

    int Process_ID;
    int CPU_time;
    int IO_time;
    int Arrival_time;

} Process, *ProcessPTR;




/******************************************************************************************
    Functions Prototypes
 ******************************************************************************************/
int Init(void);
void Create_Text_File(char *FileName);
ProcessPTR ReadFile(int *Number_of_Processes);
int *FCFS_Algorithm(ProcessPTR Processes ,int Number_of_Processes ,char *FileName ,int *Finishing_Time, int *BusyCPU_Time);
int *RR_Algorithm(ProcessPTR Processes ,int Number_of_Processes ,char *FileName ,int *Finishing_Time, int *BusyCPU_Time);
int Get_QuantumTime(void);
int GetMax_T(ProcessPTR Processes ,int Number_of_Processes);
int GetMax_AT(ProcessPTR Processes ,int Number_of_Processes);
void Print_Status(int *Processes_States, int Cycle ,int Number_of_Processes);
void Update_Text_File(char *FileName, int *Processes_States, int Cycle ,int Number_of_Processes);
void Calculate_turnaround_time(int *Arrival_time, int *Termination_time, int *Turnaround_Time ,int Number_of_Processes);
void Show_statistics(char *FileName ,int Finishing_Time, int BusyCPU_Time , int *Turnaround_Time ,int Number_of_Processes);





/******************************************************************************************
    Main Entry Point
 ******************************************************************************************/
int main()
{
    //  Initializations
    int Number_of_Processes, Algorithm, Finishing_Time =0 , BusyCPU_Time =0 ;
    int *Turnaround_Time;
    ProcessPTR Processes;

    //  Call the Initialization Function and get the User's Input
    Algorithm = Init();

    //  Read the file, allocate memory and return number of processes and address of allocated processes
    Processes  = ReadFile(&Number_of_Processes);

    //  Call the Scheduling Algorithm's Simulation according to the User's Input
    if(Algorithm)
    {
        //  Simulate RR scheduling algorithm and store the turnaround time
        Turnaround_Time = RR_Algorithm(Processes,Number_of_Processes,RR_Filename,&Finishing_Time,&BusyCPU_Time);

        //  Print the statistics of the Simulation
        Show_statistics(RR_Filename ,Finishing_Time, BusyCPU_Time ,Turnaround_Time,Number_of_Processes);
    }

    else
    {
        //  Simulate FCFS scheduling algorithm and store the turnaround time
        Turnaround_Time = FCFS_Algorithm(Processes,Number_of_Processes,FCFS_Filename,&Finishing_Time,&BusyCPU_Time);

        //  Print the statistics of the Simulation
        Show_statistics(FCFS_Filename, Finishing_Time, BusyCPU_Time ,Turnaround_Time,Number_of_Processes);
    }


    return 0;
}



/******************************************************************************************
    Initialization Function
 ******************************************************************************************/
int Init(void)
{
    int choice;

    //  This Loop holds as long as the User hasn't Entered 0 or 1
    do
    {
        //  Asks the User for the Used Algorithm
        printf("Please Enter either 0 for  FCFS Algorithm, or 1 for  RR Algorithm : ");
        scanf("%d", &choice);

    } while(!((choice == 0) || (choice == 1)));

    //  Print the User's Choice
    if(choice)
    {
        printf("\nYou Chose Round Robin Algorithm \n");
    }
    else
    {
        printf("\nYou Chose First Come First Served Algorithm \n");
    }

    return choice;
}



/******************************************************************************************
       Create a new Text file
 ******************************************************************************************/
void Create_Text_File(char *FileName)
{
    //  Remove any Old files having the Same names
    remove (FCFS_Filename);
    remove (RR_Filename);

    // Opens a file or creates is if it doesn't exist, with file acccess as write mode
    FILE* file_ptr;
    file_ptr = fopen(FileName, "w");

    // closes the file pointed by file_ptr
    fclose(file_ptr);
}



/******************************************************************************************
       Read the Input text file
 ******************************************************************************************/
Process *ReadFile(int *Number_of_Processes)
{
   //   Initializations
   int Lines = 0;
   ProcessPTR P1 ;
   char ch, file_name[Size_of_Input_File];
   FILE *fp;

    // Clears the input buffer
    while ((getchar()) != '\n');

    //  Prompts the User to get the file name
   printf("Enter the name of the input file: \n");
   gets(file_name);


    //  Open the File
   fp = fopen(file_name, "r"); // read mode

   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }

   //   Count the lines in the Input file
   while((ch = fgetc(fp)) != EOF)
   {
        if(ch=='\n')
        {
            Lines++;
        }
   }

   //   Sets the file position to the beginning of the file of the given stream
   rewind(fp);

   //   Pass the number of lines to the main function
   *Number_of_Processes = Lines;            // maybe i don't need it

   //   Allocate memory for the given Processes
   P1 = (ProcessPTR)malloc(Lines * sizeof(Process));


   //   Fill the Structure with content in the Input file
   for(int i=0 ; i<Lines ; i++)
       {
           fscanf(fp, "%d %d %d %d", &(P1[i].Process_ID), &(P1[i].CPU_time), &(P1[i].IO_time), &(P1[i].Arrival_time));
       }


   //   Close the File
   fclose(fp);

   //   Return the address of Allocated memory
   return P1;
}



/******************************************************************************************
       First Come First Served Scheduling Algorithm
 ******************************************************************************************/
int *FCFS_Algorithm(ProcessPTR Processes ,int Number_of_Processes ,char *FileName ,int *Finishing_Time, int *BusyCPU_Time)
{
    //  Initializations
    int RunningProcess = Empty , CPU_TIME = 0 , LastReady = 0 , LastBlocked = 0 , Max_time = 0 , Max_Arrivaltime = 0 ;


    //  Get the Address of the Processes
    ProcessPTR P1 = Processes;

    //  Allocate memory for the Processes States
    int *Processes_States = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize their state to Inactive State
    for(int i=0; i<Number_of_Processes ;i++)
    {
        Processes_States[i] = Inactive_State;
    }


    //  Allocate memory for the I/O States
    int *IO_States = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize their state to (I/O Not finished) State
    for(int i=0; i<Number_of_Processes ;i++)
    {
        IO_States[i] = IO_NotFinished;
    }


    //  Allocate memory for the Queues
    int *Ready_Queue = (int *)malloc(Number_of_Processes * sizeof(int));
    int *Blocked_Queue = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize all Queues to Empty state at the Beginning
    Ready_Queue[0] = Empty;
    Blocked_Queue[0] = Empty;


    //  Allocate memory for arrival times
    int *Arrival_time = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize arrival times with the given values in the input file
    for(int i=0; i<Number_of_Processes ;i++)
    {
        Arrival_time[i]  =  P1[i].Arrival_time;
    }


    //  Allocate memory for termination times
    int *Termination_time = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Allocate memory for turnaround times
    int *Turnaround_Time = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize termination times and turnaround times to zero
    for(int i=0; i<Number_of_Processes ;i++)
    {
        Termination_time[i] = 0;
        Turnaround_Time[i]  = 0;
    }


    //  Get the Maximum Possible time
    Max_time = GetMax_T(Processes,Number_of_Processes);

    //  Get the Maximum Arrival time
    Max_Arrivaltime = GetMax_AT(Processes,Number_of_Processes);


    //  Initialize text file depending on mode with the given name
    Create_Text_File(FileName);


    //  Start the FCFS Scheduling Algorithm
    for(int i =0; i < Max_time ; i++)
    {
        for(int j =0; j<Number_of_Processes ; j++)
        {
            if((P1[j].Arrival_time) == i)
            {
                //  Some Process has arrived
                if((P1[j].CPU_time) == 0)
                {

                    if(Blocked_Queue[0] == Empty)//check this line
                    {
                        Blocked_Queue[0] = j;//check this line
                    }
                    else
                    {
                        LastBlocked++ ;
                        Blocked_Queue[LastBlocked] = j;
                    }

                    //  Update Process State
                    Processes_States[j] = Blocked_State;

                }
                else
                {
                    if(Ready_Queue[0] != Empty)
                    {
                        LastReady++ ;
                        Ready_Queue[LastReady] = j;

                        //  Update Process State
                        Processes_States[j] = Ready_State;
                    }
                    else
                    {
                        Ready_Queue[0] = j;

                        //  Update Process State
                        Processes_States[j] = Ready_State;
                    }
                }
            }
        }


        //  Execute some process
        if(RunningProcess == Empty)
        {
            if( Ready_Queue[0] != Empty)
            {
                RunningProcess = Ready_Queue[0];

                 //  Update Process State
                Processes_States[Ready_Queue[0]] = Running_State;

                 //   Get the CPU Time
                 CPU_TIME = P1[(Ready_Queue[0])].CPU_time ;


                 //  Update Ready Queue
                if(LastReady)
                {
                    for(int i =0; i<=(LastReady-1) ;i++)
                    {
                        //  Move all the Elements of the Queue One Step Forward
                        Ready_Queue[i] = Ready_Queue[i+1];
                    }

                    LastReady-- ;
                }
                else
                {
                    Ready_Queue[0] = Empty;//check this line
                }
            }
        }



        //  Output the Simulation of the Current Cycle to Command Window
        Print_Status(Processes_States, i , Number_of_Processes);

        //  Output the Simulation to Output file
        Update_Text_File(FCFS_Filename , Processes_States, i ,Number_of_Processes);



        //  Manage Blocked Queue
        if((Blocked_Queue[0]) != Empty)//check this line
        {
            for(int j=0; j<=LastBlocked; j++ )
            {
                //  Decrement I/O time by One Cycle
                (P1[(Blocked_Queue[j])].IO_time)-- ;

                //  Check if Some process became Ready
                if(!(P1[(Blocked_Queue[j])].IO_time))
                {
                    //  Manage CPU time
                    if((P1[(Blocked_Queue[j])].CPU_time) == 0)
                    {
                        //   Update I/O State
                        IO_States[(Blocked_Queue[j])] = IO_Finished;

                        //  Update Process State
                        Termination_time[(Blocked_Queue[j])] =i;
                        Processes_States[(Blocked_Queue[j])] = Inactive_State;
                    }
                    else
                    {
                        //   Update I/O State
                        IO_States[(Blocked_Queue[j])] = IO_Finished;

                        //  Update Process State
                        Processes_States[(Blocked_Queue[j])] = Ready_State;

                         //  Move the Blocked Process to Ready Queue
                         if(Ready_Queue[0] != Empty)
                        {
                            LastReady++ ;
                            Ready_Queue[LastReady] =  Blocked_Queue[j];
                        }
                        else
                        {
                            Ready_Queue[0] = Blocked_Queue[j];
                        }
                    }

                    //  Manage Blocked Queue
                    if(LastBlocked)
                    {
                        //  Move all the Elements of the Queue One Step Forward
                        for(int k=j; k<=(LastBlocked-1) ;k++)//check this line
                        {
                            Blocked_Queue[k] = Blocked_Queue[k+1];
                        }

                        j-- ;//check this line
                        LastBlocked-- ;
                    }
                    else
                    {
                        Blocked_Queue[0] = Empty;//check this line
                    }
                }

            }
        }


        if(RunningProcess != Empty)
        {
            //  Count this Cycle as Busy CPU Cycle
            (*BusyCPU_Time)++ ;

            //  Decrement CPU time
            CPU_TIME--;

            if(!CPU_TIME)
            {
                //  Check to see if this Process Finished I/O
                if(IO_States[RunningProcess]  ==  IO_NotFinished)
                {
                    if(Blocked_Queue[0] == Empty)//check this line
                    {
                        Blocked_Queue[LastBlocked] = RunningProcess;
                    }
                    else
                    {
                        LastBlocked++ ;
                        Blocked_Queue[LastBlocked] = RunningProcess;
                    }

                    //  Update Process State
                     Processes_States[RunningProcess] = Blocked_State;
                }
                else
                {
                     Termination_time[RunningProcess] =i;
                     Processes_States[RunningProcess] = Inactive_State;
                }

                //  The Process has Finished its Execution
                RunningProcess = Empty;
            }

        }

        //  Check that All Processes have arrived, all Queues are Empty. In this case, Terminate the Execution
        if((i>Max_Arrivaltime)&&(RunningProcess == Empty)&&( Ready_Queue[0] == Empty)&&(Blocked_Queue[0] == Empty))//check this line
        {
            //  Get the finishing time
            *Finishing_Time = i;

            //  Calculate the turnaround time for each process
            Calculate_turnaround_time(Arrival_time, Termination_time, Turnaround_Time ,Number_of_Processes);

            break;
        }
    }

    //  Free the Allocated memory
    free(Ready_Queue);
    free(Blocked_Queue);
    free(Arrival_time);
    free(Termination_time);
    free(Processes_States);
    free(IO_States);

    //  Return the turnaround time of the given processes
    return Turnaround_Time;
}



/******************************************************************************************
       Round Robin Scheduling Algorithm
 ******************************************************************************************/
int *RR_Algorithm(ProcessPTR Processes ,int Number_of_Processes ,char *FileName ,int *Finishing_Time, int *BusyCPU_Time)
{
    //  Initializations
    int QuantumTime , Counter , RunningProcess = Empty , CPU_TIME = 0 , LastReady = 0 , LastBlocked = 0 , Max_time = 0 , Max_Arrivaltime = 0 ;


    //  Get the Address of the Processes
    ProcessPTR P1 = Processes;

    //  Allocate memory for the Processes States
    int *Processes_States = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize their state to Inactive State
    for(int i=0; i<Number_of_Processes ;i++)
    {
        Processes_States[i] = Inactive_State;
    }


    //  Allocate memory for the I/O States
    int *IO_States = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize their state to (I/O Not finished) State
    for(int i=0; i<Number_of_Processes ;i++)
    {
        IO_States[i] = IO_NotFinished;
    }


    //  Allocate memory for the Queues
    int *Ready_Queue = (int *)malloc(Number_of_Processes * sizeof(int));
    int *Blocked_Queue = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize all Queues to Empty state at the Beginning
    Ready_Queue[0] = Empty;
    Blocked_Queue[0] = Empty;


    //  Store the CPU time for each process
    int *Process_CPUTime = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize arrival times with the given values in the input file
    for(int i=0; i<Number_of_Processes ;i++)
    {
        Process_CPUTime[i]  =  P1[i].CPU_time;
    }


    //  Allocate memory for arrival times
    int *Arrival_time = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize arrival times with the given values in the input file
    for(int i=0; i<Number_of_Processes ;i++)
    {
        Arrival_time[i]  =  P1[i].Arrival_time;
    }


    //  Allocate memory for termination times
    int *Termination_time = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Allocate memory for turnaround times
    int *Turnaround_Time = (int *)malloc(Number_of_Processes * sizeof(int));

    //  Initialize termination times and turnaround times to zero
    for(int i=0; i<Number_of_Processes ;i++)
    {
        Termination_time[i] = 0;
        Turnaround_Time[i]  = 0;
    }


    //  Get the Maximum Possible time
    Max_time = GetMax_T(Processes,Number_of_Processes);

    //  Get the Maximum Arrival time
    Max_Arrivaltime = GetMax_AT(Processes,Number_of_Processes);


    //ask for quantum time and check it is more than zero
    do
    {
        //  The Function Get_QuantumTime returns Zero in case the input was less than or equal Zero
        QuantumTime = Get_QuantumTime();

    } while(!QuantumTime);


     //  Initialize text file depending on mode with the given name
    Create_Text_File(FileName);


    //  Start the RR Scheduling Algorithm
    for(int i =0; i < Max_time ; i++)
    {
        for(int j =0; j<Number_of_Processes ; j++)
        {
            if((P1[j].Arrival_time) == i)
            {
                //  Some Process has arrived
                if((P1[j].CPU_time) == 0)
                {

                    if(Blocked_Queue[0] == Empty)
                    {
                        Blocked_Queue[0] = j;
                    }
                    else
                    {
                        LastBlocked++ ;
                        Blocked_Queue[LastBlocked] = j;
                    }

                    //  Update Process State
                    Processes_States[j] = Blocked_State;

                }
                else
                {
                    if(Ready_Queue[0] != Empty)
                    {
                        LastReady++ ;
                        Ready_Queue[LastReady] = j;

                        //  Update Process State
                        Processes_States[j] = Ready_State;
                    }
                    else
                    {
                        Ready_Queue[0] = j;

                        //  Update Process State
                        Processes_States[j] = Ready_State;
                    }
                }
            }
        }


        //  Execute some process
        if(RunningProcess == Empty)
        {
            if( Ready_Queue[0] != Empty)
            {
                RunningProcess = Ready_Queue[0];

                 //  Update Process State
                Processes_States[Ready_Queue[0]] = Running_State;

                 //   Get the CPU Time
                 CPU_TIME = P1[(Ready_Queue[0])].CPU_time ;

                //  Reset the Time slice equal to the Entered Quantum for the Running process
                Counter = QuantumTime;

                 //  Update Ready Queue
                if(LastReady)
                {
                    for(int i =0; i<=(LastReady-1) ;i++)
                    {
                        //  Move all the Elements of the Queue One Step Forward
                        Ready_Queue[i] = Ready_Queue[i+1];
                    }

                    LastReady-- ;
                }
                else
                {
                    Ready_Queue[0] = Empty;//check this line
                }
            }
        }



        //  Output the Simulation of the Current Cycle to Command Window
        Print_Status(Processes_States, i , Number_of_Processes);

        //  Output the Simulation to Output file
        Update_Text_File(RR_Filename , Processes_States, i ,Number_of_Processes);


        //  Manage Blocked Queue
        if((Blocked_Queue[0]) != Empty)//check this line
        {
            for(int j=0; j<=LastBlocked; j++ )
            {
                //  Decrement I/O time by One Cycle
                (P1[(Blocked_Queue[j])].IO_time)-- ;

                //  Check if Some process became Ready
                if(!(P1[(Blocked_Queue[j])].IO_time))
                {
                    //  Manage CPU time
                    if((P1[(Blocked_Queue[j])].CPU_time) == 0)
                    {
                        //   Update I/O State
                        IO_States[(Blocked_Queue[j])] = IO_Finished;

                        //  Update Process State
                        Termination_time[(Blocked_Queue[j])] =i;
                        Processes_States[(Blocked_Queue[j])] = Inactive_State;
                    }
                    else
                    {
                        //   Update I/O State
                        IO_States[(Blocked_Queue[j])] = IO_Finished;

                        //  Update Process State
                        Processes_States[(Blocked_Queue[j])] = Ready_State;

                         //  Move the Blocked Process to Ready Queue
                         if(Ready_Queue[0] != Empty)
                        {
                            LastReady++ ;
                            Ready_Queue[LastReady] =  Blocked_Queue[j];
                        }
                        else
                        {
                            Ready_Queue[0] = Blocked_Queue[j];
                        }
                    }

                    //  Manage Blocked Queue
                    if(LastBlocked)
                    {
                        //  Move all the Elements of the Queue One Step Forward
                        for(int k=j; k<=(LastBlocked-1) ;k++)//check this line
                        {
                            Blocked_Queue[k] = Blocked_Queue[k+1];
                        }

                        j-- ;//check this line
                        LastBlocked-- ;
                    }
                    else
                    {
                        Blocked_Queue[0] = Empty;//check this line
                    }
                }

            }
        }


        if(RunningProcess != Empty)
        {
            //  Count this Cycle as Busy CPU Cycle
            (*BusyCPU_Time)++ ;

            //  Decrement CPU time
            CPU_TIME--;

            //  Update the Counter for the Time slice spent for the Running Process
            Counter--;

            if(!CPU_TIME)
            {
                //  Check to see if this Process Finished I/O
                if(IO_States[RunningProcess]  ==  IO_NotFinished)
                {
                    //  Reset the CPU Clock
                    P1[RunningProcess].CPU_time = Process_CPUTime[RunningProcess];

                    //  Update Process State
                     Processes_States[RunningProcess] = Blocked_State;


                    if(Blocked_Queue[0] == Empty)//check this line
                    {
                        Blocked_Queue[LastBlocked] = RunningProcess;
                    }
                    else
                    {
                        LastBlocked++ ;
                        Blocked_Queue[LastBlocked] = RunningProcess;
                    }
                }
                else
                {
                     Termination_time[RunningProcess] =i;
                     Processes_States[RunningProcess] = Inactive_State;
                }

                //  The Process has Finished its Execution
                RunningProcess = Empty;
            }
            else
            {
                if(!Counter)
                {
                    //  Update the CPU Time for the Running Process
                    P1[RunningProcess].CPU_time = CPU_TIME;

                    //  Move the Running Process to Ready Queue
                    if(Ready_Queue[0] != Empty)
                    {
                        LastReady++ ;
                        Ready_Queue[LastReady] = RunningProcess;


                    }
                    else
                    {
                        Ready_Queue[0] = RunningProcess;
                    }

                    //  Update Process State
                        Processes_States[RunningProcess] = Ready_State;

                    //  Indicate that the Processor became Empty
                    RunningProcess = Empty;
                }
            }
        }

        //  Check that All Processes have arrived, all Queues are Empty. In this case, Terminate the Execution
        if((i>Max_Arrivaltime)&&(RunningProcess == Empty)&&( Ready_Queue[0] == Empty)&&(Blocked_Queue[0] == Empty))//check this line
        {
            //  Get the finishing time
            *Finishing_Time = i;

            //  Calculate the turnaround time for each process
            Calculate_turnaround_time(Arrival_time, Termination_time, Turnaround_Time ,Number_of_Processes);

            break;
        }
    }

    //  Free the Allocated memory
    free(Ready_Queue);
    free(Blocked_Queue);
    free(Arrival_time);
    free(Termination_time);
    free(Processes_States);
    free(IO_States);

    //  Return the turnaround time of the given processes
    return Turnaround_Time;
}



/******************************************************************************************
       Get the quantum time from the user, in case of Round Robin scheduling algorithm
 ******************************************************************************************/
int Get_QuantumTime(void)
{
    int QuantumTime;
    //  Print a message to the User to Enter the Quantum Time
    printf("\nPlease Enter the Quantum Time specified for Round Robin mode: ");
    scanf("%d", &QuantumTime);

    //  Check that the Entered Quantum time is more than Zero
    if(QuantumTime > 0)
    {
        return QuantumTime;
    }
    else
    {
        printf("\nThe Entered Quantum Time must be more than Zero: ");
        return 0;
    }
}



/******************************************************************************************
       Calculate the maximum possible time for executing the given processes
 ******************************************************************************************/
int GetMax_T(ProcessPTR Processes ,int Number_of_Processes)
{
    int Total_time = 0;
    ProcessPTR P1 = Processes;

    //  It calculates the worst case scenario Total Time
    for(int i = 0; i<Number_of_Processes ; i++)
    {
        Total_time += (2*(P1[i].CPU_time));
        Total_time += (P1[i].IO_time);
    }

    return Total_time;
}



/******************************************************************************************
       Calculate the maximum arrival time for a single process of the given processes
 ******************************************************************************************/
int GetMax_AT(ProcessPTR Processes ,int Number_of_Processes)
{
    ProcessPTR P1 = Processes;

    int Max_AT = P1[0].Arrival_time;

    for(int i = 0; i<Number_of_Processes ; i++)
    {
        if((P1[i].Arrival_time) > Max_AT)
        {
            Max_AT = P1[i].Arrival_time;
        }
    }

    return Max_AT;
}



/******************************************************************************************
       Print the current status of the active processes to the command window
 ******************************************************************************************/
void  Print_Status(int *Processes_States, int Cycle ,int Number_of_Processes)
{
    //	Print the Simulation Cycle Time
    printf("%d",Cycle);

    //  Print the Process ID and State
    for(int j=0; j<Number_of_Processes ;j++)
    {
        switch(Processes_States[j])
        {
            case Inactive_State:
                break;

            case Running_State:
                printf(" %d: Running",j);
                break;

            case Ready_State:
                printf(" %d: Ready",j);
                break;

            case Blocked_State:
                printf(" %d: Blocked",j);
                break;
        }

    }

    //	Print New line
    printf("\n");

}



/******************************************************************************************
       Update the output text file with new status of the current active processes
 ******************************************************************************************/
void Update_Text_File(char *FileName, int *Processes_States, int Cycle ,int Number_of_Processes)
{
    // pointer file_ptr to FILE
    FILE* file_ptr;

    // Opens the Output file  with file access as append mode
    file_ptr = fopen(FileName, "a");


    //	Output the Simulation Cycle Time
    fprintf(file_ptr, "%d",Cycle);

    //  Output the Process ID and State
    for(int j=0; j<Number_of_Processes ;j++)
    {
        switch(Processes_States[j])
        {
            case Inactive_State:
                break;

            case Running_State:
                fprintf(file_ptr, " %d: Running",j);
                break;

            case Ready_State:
                fprintf(file_ptr, " %d: Ready",j);
                break;

            case Blocked_State:
                fprintf(file_ptr, " %d: Blocked",j);
                break;
        }

    }

    //	Output the Simulation Cycle Time
    fprintf(file_ptr, "\n");

    // closes the file pointed by file_ptr
    fclose(file_ptr);
}



/******************************************************************************************
       Prints the required statistics of the simulation
 ******************************************************************************************/
void Calculate_turnaround_time(int *Arrival_time, int *Termination_time, int *Turnaround_Time ,int Number_of_Processes)
{
    for(int i =0 ; i<Number_of_Processes ; i++)
    {
        Turnaround_Time[i] = Termination_time[i] - Arrival_time[i] + 1 ;
    }
}



/******************************************************************************************
       Prints the required statistics of the simulation
 ******************************************************************************************/
void Show_statistics(char *FileName , int Finishing_Time, int BusyCPU_Time , int *Turnaround_Time , int Number_of_Processes)
{
    //  Calculate CPU-Utilization
    float CPU_Utilization = ((float)BusyCPU_Time)/(Finishing_Time+1);


    //  Output results to Command Window
    printf("\nFinishing time: %d",Finishing_Time);
    printf("\nCPU Utilization: %f",CPU_Utilization);

    for(int i=0 ;i<Number_of_Processes ; i++)
    {
        printf("\nTurnaround time of Process %d: %d", i ,Turnaround_Time[i]);
    }

    //  Go two lines below to make the text more clear
    printf("\n\n");



    //  Output results to output file

    // pointer file_ptr to FILE
    FILE* file_ptr;

    // Opens the Output file  with file access as append mode
    file_ptr = fopen(FileName, "a");


    fprintf(file_ptr, "\nFinishing time: %d",Finishing_Time);
    fprintf(file_ptr, "\nCPU Utilization: %f",CPU_Utilization);



     for(int i=0 ;i<Number_of_Processes ; i++)
    {
        fprintf(file_ptr, "\nTurnaround time of Process %d: %d", i ,Turnaround_Time[i]);
    }

     //  Go two lines below to make the text more clear
    fprintf(file_ptr, "\n\n");


    // closes the file pointed by file_ptr
    fclose(file_ptr);

}


