#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <algorithm>

int barrel = 50;
bool sim_running = true;
std::mutex mut1;
std::mutex mut2;
std::condition_variable barrel_empty_condition_variable;
std::condition_variable pause_all_condition_variable;
std::vector<int> wait_list;
int numOfStudents;
std::vector<int>::iterator wait_iterator;


void show_barrel_level(){
	//Display Barrel level
	std::cout << "-----------------------------------" << std::endl;
	std::cout << "Drinks remaining: " << barrel << std::endl;
	std::cout << "-----------------------------------" << std::endl;
}

bool simulation_running(){
	return sim_running;
}

//Student class simulates a student, uses a thread to carry out the student function  
class Student{
public: 
	//pointer to student thread
	std::thread *student_thread;

	//process control block data
	int thread_id;
	int process_id;
	std::string state;
	int num_of_drinks;
	int drinks_consumed = 0;
	int wait_count = 0;

	Student(){
		//run();
	}
	
	//Contruct a student object with a thread id
	Student(int id){
		this->process_id = 7897548;
		this->thread_id = id;
		wait_list.push_back(0);
	}
	
	bool join(){
		if(student_thread->joinable()){
			student_thread->join();
			return true;
		}else{
			return false;
		}
	}
	
	void display_pcb(){
		std::cout << "-----------------------------------" << std::endl;
		std::cout << "Process ID: \t\t" << this->process_id << std::endl;
		std::cout << "Thread ID: \t\t" << this->thread_id << std::endl;
		std::cout << "Thread State: \t\t" << this->state << std::endl;
		std::cout << "Process Type: \t\tStudent" << std::endl;
		std::cout << "Light beer required: \t" << this->num_of_drinks << std::endl;
		std::cout << "Light beer consumption \t" << this->drinks_consumed << std::endl;
		std::cout << "Waiting for \t\t" << this->wait_count << std::endl;

		
	}
	
	// innitiate a students drinking and thinking 
	void student_actions(){
		//TODO: use condition variable to check for pause and wait 
		for(int i=0; i<=8; i++){
			//Set the thread state to waiting as we try to get drink
			this->state = "Waiting";

			std::unique_lock<std::mutex> locker2(mut2);
			pause_all_condition_variable.wait(locker2, simulation_running);

			
			locker2.unlock();
			get_serving();

			
			//Set the therad state to drink and think			
			if(this->state == "Drinking-Thinking"){
				drink_and_think();
			}


		}
		this->state = "Terminated";
	}


	// simulates students drinking and thinking. The amount of time 
	//   spent thinking and drinking is random 
	void drink_and_think(){
		int wait_time = (rand()%5);
		//display the amountof time this student will think for
		std::cout << "Student " << this->thread_id << " thinking ... . think time " << wait_time << std::endl;
		//wait for the amount of time randomly generatd for studen to think 
		std::this_thread::sleep_for(std::chrono::seconds(wait_time));
	}
	
	void get_serving(){
		//Determin howm many drinks the student wants 
		if(barrel > 0){
			//this->wait_count++;
			//std::cout << "Wait count:" << this->wait_count << std::endl;
			//check if any element hase been wating for two times the number of threads
			wait_iterator = std::max_element(wait_list.begin(), wait_list.end());
			//get index of max element
			int max_wait = std::distance(wait_list.begin(), wait_iterator);
			//If no process has been waiting for more than twice the number of threads or if this is that thread, enter the critical section 
			//if(!wait_list[max_wait] >= (numOfStudents*2) || max_wait == this->thread_id){

				std::unique_lock<std::mutex> locker(mut1);
				
				//increase wait count of processes if the process gets a turn to execute critical section
				for(int j = 0; j < numOfStudents; j++){
					wait_list[j] = wait_list[j]+1;
				}
				wait_list[this->thread_id] = 0;
				
				// generate the number of beers student will drink
				this->num_of_drinks = (rand()%5+1);
				if(barrel > this->num_of_drinks){
					barrel = barrel - this->num_of_drinks;
					std::cout << "-----------------------------------" << std::endl;
					std::cout << "Student " << this->thread_id << " took " << num_of_drinks << " light beers" << std::endl;

					//this->display_pcb();
				}else{
					this->num_of_drinks = barrel;
					barrel = 0;
				}
				this->drinks_consumed += this->num_of_drinks;

				//Display Barrel level
				show_barrel_level();


				this->state = "Drinking-Thinking";

				// reset wait counter
				this->wait_count = 0;

				locker.unlock();
			//}

		}else{
			std::cout <<"Barrel empty"<< std::endl;
			barrel_empty_condition_variable.notify_all();
			
		}
	}


};

class Bartender{
	std::thread *bartender_thread;
	int times_waken = 0;
	int process_id;
	int thread_id;
	std::string state;

	public:
		Bartender(){
			this->process_id = 2;
			this->thread_id = 39578;
			this->state = "terminated";
		}
		void display_pcb(){
			std::cout << "-----------------------------------" << std::endl;
			std::cout << "Process ID: \t\t" << this->process_id << std::endl;
			std::cout << "Thread ID: \t\t" << this->thread_id << std::endl;
			std::cout << "Thread State: \t\t" << this->state << std::endl;
			std::cout << "Process Type: \tBartender" << std::endl;
			std::cout << "Wake count: \t" << this->times_waken << std::endl;
		}
		
		int refill_barrel(){
			while(this->times_waken < 3){
				this->state = "Waiting";
				std::cout << "Bartender running" << std::endl;
				std::unique_lock<std::mutex> locker(mut1);
				barrel_empty_condition_variable.wait(locker, std::bind(&Bartender::barrel_empty, this));
				this->state = "Refilling";
				std::cout << "Bartender working" << std::endl;
				if(barrel <= 0){
					int num_of_drinks = (rand()%20+31);
					//TODO: Lock not working. code won't run
					
					std::cout << std::endl << "-----------------------------------" << std::endl;
					std::cout << "Refilling barrel ";
					switch(this->times_waken){
						case 0:
							std::cout << "(first time) ";
						break;
						case 1:
							std::cout << "(second time) ";
						break;
						case 2: 
							std::cout << "(third time) ";
						break;
					
					}

					
					for( int d=0; d <= num_of_drinks; d++){
						std::cout << ".";
						barrel++;
					}
					std::cout << std::endl << "-----------------------------------" << std::endl;
					std::cout << std::endl << "Barrel refilled " << std::endl;
					//Display Barrel level
					show_barrel_level();
		
				}
				locker.unlock();
				this->times_waken++;
			}
			this->state = "Terminated";
			std::cout << "Bartender off duty" << std::endl;
			
			return barrel;
		}
		
		
		void run(){
			bartender_thread = new std::thread(&Bartender::refill_barrel, this);
		}
		
		bool join(){
			if(bartender_thread->joinable()){
				bartender_thread->join();
				return true;
			}else{
				return false;
			}
		}
		
		bool barrel_empty(){
			if(barrel <= 0){
				return true; 
			}else{
				return false;
			}
		}
};


int main(){
	srand(time(0));
	

	//------------------------------------------------------------------
	numOfStudents = (rand()%5+10);
	
	std::vector<Student *> students;
	std::vector<std::thread> student_threads;
	
	std::cout << "-----------------------------------" << std::endl;
	std::cout << "Number of students present: " << numOfStudents << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	
	//Set up bartender
	Bartender *bar_keep = new Bartender();
	//------------------------------------------------------------------
	for(int i = 0; i < numOfStudents; i++){
		Student *s = new Student(i);

		students.push_back(s);
		//students[i].student_thread = new std::thread(&Student::student_actions, &students[i]); 
		student_threads.push_back(std::thread(&Student::student_actions, students[i]));
	};
	bar_keep->run();

	while (std::cin.ignore(256, '\n'))
	{
		std::cout<<"enter pressed";
		sim_running = !sim_running;
		//if(!sim_running){}
		std::unique_lock<std::mutex> locker2(mut2);
		pause_all_condition_variable.notify_all();
		
		std::cout << "PAUSED ---------------------- " << std::endl;
		//s.display_pcb();
		//s1.display_pcb();
		bar_keep->display_pcb();
		/*for(Student & stud: students){
			stud.display_pcb();
		}*/

		for(int i = 0; i < numOfStudents; i++){
			students[i]->display_pcb();
		}
		/*while (std::cin.ignore(256, '\n'))
		{
			
		}*/
	}

	//join bartender thread
	bar_keep->join();
	//join student threads
	for(std::thread & stud: student_threads){
		if(stud.joinable()){
			stud.join();
		}
	}
	
	return 0;
}