
/*
	K. Naveen Kumar
	CS19MTECH11009
	IIT Hyderabad
*/

/*
	The class ProcessClass is responsible to creat process by using the constructor of the same class. That constructor is
	responsible to create process object and maintains two threads for each process. one thread for local clock and another
	thread for receive function.

	This class is also contains synchronize_request(),synchronize_reply(),send() and receive() function which are given in
	the assignment discription. Along with these functions this class also includes the function called socket_create() which
	will create socket for process.

*/


using namespace std;

class ProcessClass
{   /*each thread will be created to execute this class instance */
	
	/*Making members/feilds of this class available as public so that they can be accessible froma any class.*/
	public:
	int socket_file_discripter;/*Socket discriptor for process */

	localclock local_clock;  /* A process must have its own local clock. So for that we need this */

	thread localclock_t,receive_t; /* Each process should maintain two threads. one for local clock and another for receive*/
	
	int pid; /*Process identifier. Which will be initialized when the process constructor is created.*/
	 
	time_t temp_time1,temp_time2; /*time variables used to get T2 and T3 times, to compute delta*/

	bool flag = false; /*Used for wait*/


	/*
		Belwo snippet of code is to create constructor of processClass. The Constructor takes process id and creates object
		/instance of the class. 

	*/	
	ProcessClass(int process_id) /*Constructor code,One argument as process id*/
	{
		localclock_t =  thread(&localclock :: IncrementDriftFactor,local_clock); /*Creating thred for local clock*/
		receive_t =  thread(&ProcessClass :: receive,this); /*Thread for receive*/
		pid = process_id;/*Assign process id to pid variable of the process class*/

		/* Detaches the thread represented by the object from the calling thread, allowing them to execute independently from each other*/
		localclock_t.detach(); 
		receive_t.detach();
	}

	/* For closing the socket 
	~ProcessClass(){
		close(socket_file_discripter);
	}

	*/


	/*
		The synchronize_request function takes two arguments, 1.destination process id and 2. round number. 
		This function will gather T1,T2,T3,T4 times and calculate the delta and update the process clock to correct it.
	*/

	void  synchronize_Request(int destination_pid,int round_number)
	{


	  /* Reading time and converting it into our convenient format */
	  struct tm * time_information; /* Structure containing a calendar date and time broken down into its components*/
	  time_t system_time=local_clock.Timeread();/* time type variable, which stores the local_clock time from the tableValues*/
	  time ( &system_time ); /*This function is defined in time.h*/
	  time_information = localtime ( &system_time ); /*Converts to local time*/
	  char *stringTime= asctime(time_information); /*Convert tm structure to string*/
	  	
	  /*Below two lines extract hh::mm::ss format from the stringTime*/
	  stringTime[19]='\0'; 
	  stringTime=&stringTime[11];  
	  /*Just to write times for request done*/
	  printf("Server %d requests round_number %d clock synchronization to Server %d at %s\n",pid+1,round_number,destination_pid+1,stringTime);


	  time_t T1 = local_clock.Timeread(); /*This is T1. Time read of the process from local clock*/
		
	  char message[MAX_BUFFER]; /*Message character array to use as message buffer*/
	  sprintf(message,"request %d",round_number); /* Arranging the message in message buffer*/
	  send(htons(DIFF_PORTS+destination_pid),message);  /*Sending the message to other destination_pid process as request to get T2,T3*/

	  time_t T2,T3;
	  while((flag == false)); 
	  T2 = temp_time1; /*T2 from  request (T2,T3) from process*/
	  T3 =temp_time2; /*T3 as well */


	  system_time=local_clock.Timeread();/* time type variable, which stores the local_clock time from the tableValues*/
	  time ( &system_time ); /*This function is defined in time.h*/
	  time_information = localtime ( &system_time ); /*Converts to local time*/
	  stringTime= asctime(time_information); /*Convert tm structure to string*/
	  	
	  /*Below two lines extract hh::mm::ss format from the stringTime*/
	  stringTime[19]='\0'; 
	  stringTime=&stringTime[11]; 

      printf("Server %d receives round_number %d clock synchronization response from Server %d at %s\n",pid+1,round_number+1,destination_pid+1,stringTime);
	
	  flag = false;

	  time_t T4 = local_clock.Timeread(); /*Time read for T4, which will be used to calculate delta*/
		

	  printf("Computing the round_number %d delta between Server %d and Server %d..\n",round_number,pid+1,destination_pid+1);
	  

	  /*Following code is used to calculate the delta. This formula is taken from the book*/
	  double a ,b;
	  a=T1-T3;
	  b=T2-T4;
	  double delta=a+b/2.0;

	  local_clock.update(delta); /*Updte the delta by calling the update function in localclock class*/


	  /*
		Sleep before synchronizing again with the exponential average of lambda_p. This is asked in assignment discription
		Below code is calculating reqSleepTime and going to sleep.
	  */
	  
	  double reqSleepTime;
	  reqSleepTime = rand() / (double)RAND_MAX;
	  reqSleepTime=-log(1- reqSleepTime) / lamda_p;
	  sleep(reqSleepTime); /*Sleeping........*/

		
	}


	/*
		This function is responsible for replying for synchronization requests.It takes two arguments as inpuut one is dest process id and round number.
		It receives synchronize request from prcess destination_pid 
		
	*/
	void synchronize_Reply(int destination_pid,int round_number)
	{
		
		//Read time
		/*As mentioned in the assignment discription reading the time in the beginning of this function*/

		time_t T2 = local_clock.Timeread(); /*Read the time from local clock of the process*/

		/*
		Sleep before synchronizing again with the exponential average of lambda_p. This is asked in assignment discription
		Below code is calculating reqSleepTime and going to sleep.
	  */
	  
	    double reqSleepTime;
	    reqSleepTime = rand() / (double)RAND_MAX;
	    reqSleepTime=-log(1- reqSleepTime) / lamda_q;
	    sleep(reqSleepTime); /*Sleeping........As given in the discription of the assignment.*/




		time_t T3 = local_clock.Timeread(); /*Reading the local clock and stored as T3*/
		

		/* The reply should contain values T2, T3
			So, here making the message containing values of T2 and T3
		*/
		char message[MAX_BUFFER];
		sprintf(message,"%ld %ld",T2,T3);


	  	/* Reading time and converting it into our convenient format */
	  	struct tm * time_information; /* Structure containing a calendar date and time broken down into its components*/
	  	time_t system_time=local_clock.Timeread();/* time type variable, which stores the local_clock time from the tableValues*/
	  	time ( &system_time ); /*This function is defined in time.h*/
	  	time_information = localtime ( &system_time ); /*Converts to local time*/
	  	char *stringTime= asctime(time_information); /*Convert tm structure to string*/
	  	
	  	/*Below two lines extract hh::mm::ss format from the stringTime*/
	  	stringTime[19]='\0'; 
	  	stringTime=&stringTime[11];  

	  	/*Printing replies time in round*/
		printf("Server %d replies round_number %d synchronization response to Server %d at %s\n",pid+1,round_number,destination_pid+1,stringTime);
		send(htons(destination_pid+DIFF_PORTS),message); /*This is network send to destination*/
		
	}



	/*
		All processes use send function to send the messages to other processes.
		The send function takes two arguments one is process id to which we want to send and second is the message that we want to send.
		This function uses sendto() function from sockets to send the message to destination process

	*/

	void send(int destination_port,char *message) /*Here process id to which we want to send the message is converted to the respective port number
													in the code which will call the send(0 function */
	{
	
		
		
		/*
		Sleep before synchronizing again with the exponential average of lambda_p. This is asked in assignment discription
		Below code is calculating reqSleepTime and going to sleep.
	  */
	  
	    double reqSleepTime;
	    reqSleepTime = rand() / (double)RAND_MAX;
	    reqSleepTime=-log(1- reqSleepTime) / lamda_q;
	    sleep(reqSleepTime); /*Sleeping........As given in the discription of the assignment.*/



		struct sockaddr_in peeraddr;  /*Now socket address to the peer to whome the process want to send the message*/
		peeraddr.sin_family    = AF_INET; /*We use IPV4 protocol*/ 
		peeraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /* Local host address here*/
		peeraddr.sin_port = destination_port;  /*Assigning the destination process port to peeraddr.sin_port*/
	
		
		/*This is network send(dest_id) using sendto() function from the sockets conept
		  
		*/
		sendto(socket_file_discripter, (const char *)message, strlen(message),MSG_CONFIRM, (const struct sockaddr *) &peeraddr,sizeof(peeraddr)); 
	}


	/*
		Each process will receive messages from other processes using this function.
		This function also uses recvfrom() function from the sockets conept to receive message from the processes.
	*/
	void receive(){

		while(true) /*This loop is to receive message continuously*/
		{

			char *message_received = new char[MAX_BUFFER];  /*Declaring message receive buffer*/
			unsigned int length, n,round_number; /*Variables declaration*/
			char garbage[8];

			struct sockaddr_in peeraddr;  /*socket addresses for  peer*/

			length = sizeof(peeraddr);  /*Storing the size of peeradd in length*/
			peeraddr.sin_family    = AF_INET; /*Using IPV4*/
			peeraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /*Local host address*/ 

			/*	Below code is used to receive message from processes. recvfrom function is used to receive the message. recvfrom
				function will return how many bytes are received.
			*/
			n = recvfrom(socket_file_discripter, (char *)message_received, MAX_BUFFER,MSG_WAITALL, ( struct sockaddr *) &peeraddr,&length); 
			message_received[n] = '\0';  /*Appending null character at the end of the message*/

			if(!strncmp(message_received,"request",8)) /*Checking the request received is synchronize_Request or not*/
			{  
				


	    		/* Reading time and converting it into our convenient format */
	  			struct tm * time_information; /* Structure containing a calendar date and time broken down into its components*/
	  			time_t system_time=local_clock.Timeread();/* time type variable, which stores the local_clock time from the tableValues*/
	  			time ( &system_time ); /*This function is defined in time.h*/
	  			time_information = localtime ( &system_time ); /*Converts to local time*/
	  			char *stringTime= asctime(time_information); /*Convert tm structure to string*/
	  	
	  			/*Below two lines extract hh::mm::ss format from the stringTime*/
	  			stringTime[19]='\0'; 
	  			stringTime=&stringTime[11];		

				sscanf(message_received,"%s %d",garbage,&round_number); /*reads formatted input from a string.*/
				printf("Server %d receives round_number %d synchronization request from Server %d at %s\n",pid+1,round_number,ntohs(peeraddr.sin_port)-DIFF_PORTS+1,stringTime);
				synchronize_Reply(ntohs(peeraddr.sin_port)-DIFF_PORTS,round_number);
			}
			
			else  /*This else condition to avoid infinite loop if the request is not synchronizes*/
			{
				while(flag);
				sscanf(message_received,"%ld %ld",&temp_time1,&temp_time2);/*reads formatted input from a string.*/
				flag = true;	
			}
		}
	}

	
	/*
		This socket_create() function creates socket for process.
		The port assigned to the socket is obtained from the process id.(i.e DIFF_PORT+processID)
		Each process will invoke this function to create socket for that process.
	*/
	int socket_create()
	{
		int  PORT = DIFF_PORTS+ pid; /* Making port number with process id. Here DIFF_PORTS=35000 as predefined */
		struct sockaddr_in myaddr; /* Socket address variable*/

		/*
			In below if condition we used socket() function to create socket and the returned discriptor is stored in the
			socket_file_discripter. If socket() function returns the value less than 0 then socket is not created. i.e error
			occured while creating the socket.
		*/	
		if ( (socket_file_discripter = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 )
		{
			printf("Socket is not created for process %d \n",pid+1);
			return -1; 
		}


	
		/*
			After creating the socket we need to bind it with address and port number to get identified.
			Below snippet of code is doing this task.
		*/
		myaddr.sin_family    = AF_INET; // IPv4 
		myaddr.sin_addr.s_addr = INADDR_ANY; /*Addressss*/
		myaddr.sin_port = htons(PORT);  /*Port number*/
	

		/*
			We use bind() function to bind the address and port number
		*/
		int status = bind(socket_file_discripter, (const struct sockaddr *)&myaddr,sizeof(myaddr))	;

		if (status < 0 ) /*If returned value from the bind() is negative then we can conclude that error occured during the binding process*/
		{
			printf("Binding error id occurred for process %d \n", myaddr.sin_port-DIFF_PORTS); 
			perror("bind failed"); 
			exit(EXIT_FAILURE); 
			return -1; //failed to associate particular port number with this process....
		}

		/*
			If we successfully bind the address then we can declare that socket creation for the process is completed successfully.
		*/
		printf("Socket Created Successfully for process %d \n", pid+1);;
		
		return 1;
	}



	/*
		The below snippet of line shows the barrier synchronization of the process. We use this synchronization because
		process after creating the socket, it has to wait untill all other processes to create their sockets. So that they
		can communicate with each other. If we don't use this type synchronization, then process will send/receive messages to
		the processes which haven't created their sockets(No means of communication then). So, we use this barrier synchronizations. 
	*/

	void process_Synchronization_Barrier()
	{    
		pthread_mutex_lock(&locker);/*all the processes should bind to socket before they start communicating each other*/  
		process_done ++; /*Each process increments it if they bind the socket*/
		
		if(process_done == n) /*If all the processes are done with binding of the sockets then we release the lock and wake up the threads*/
		{
			for(int i=1;i<=process_done;i++) /*Here waking up the threads*/
				pthread_cond_signal(&conditioned_thread);  /*This concept is used from operating systems subject for synchronization*/
		}
		
		else /*If not all processes completed the binding then wait untill they do */
		{
		pthread_cond_wait(&conditioned_thread, &locker);  /*Waiting */
		}

		pthread_mutex_unlock(&locker);/*Do this after all processes completes the binding of socekts*/
		process_done = 0;	
	}

};

