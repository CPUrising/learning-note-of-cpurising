#include<thread>
#include<iostream>
void threadf()
{
	std::cout << "hello tread";
}

int main()
{
	std::thread th(threadf);
	std::cout << "main";
	th.join();
}