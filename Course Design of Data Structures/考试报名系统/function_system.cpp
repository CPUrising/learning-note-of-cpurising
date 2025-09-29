#include"system.h"
void system_init(LinkedNode &temp, Linkedlist &registerList)
{
	int total;
	std::cout << "首先请建立考生信息系统!\n";
	std::cout << "请输入考生人数：\n";
	while (!(std::cin >> total)) {
		std::cerr << "输入错误，请输入整数: ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	std::cout << "请依次输入要插入的考生的考号，姓名，性别(男/女)，年龄及报考类别：\n";//remember cin error
	while (total--)
	{
		while (!safe_read_node(std::cin, temp)) {
			std::cout << "请重新输入考生信息：\n";
		}
		registerList.node_putback(temp);
	}
	std::cout<<std::endl;
	std::cout << "请选择您要进行的操作（1为插入，2为删除，3为查找，4为修改，0为取消操作）\n\n";
}
void system_insert(LinkedNode& temp, Linkedlist& registerList) {
	int index;
	std::cout << "请输入要插入的考生的位置：\n";

	// 处理位置输入的异常
	while (1) {//danger ! N0 limits
		if (!(std::cin >> index))
		{
			std::cout << "输入错误，请输入整数: ";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else if (index<0 || index>registerList.get_size()+1)
			std::cout << "插入位置超过范围，请重新输入：";
		else
			break;
	}

	std::cout << "请依次输入要插入的考生的考号，姓名，性别(男/女)，年龄及报考类别：\n";

	// 循环直到成功读取有效数据
	while (!safe_read_node(std::cin, temp)) {
		std::cout << "请重新输入考生信息：\n";
	}
	registerList.node_insert(index, temp);
	registerList.print_list();
}
void system_find(Linkedlist& registerList)
{
	std::string num;
	std::cout << "请输入要查找的考生的考号：\n";
	while (!safe_read_num(std::cin, num)) {}
	std::cout << "查找的考生的信息：\n";
	registerList.node_find(num);
	registerList.print_list();
}
void system_delete(Linkedlist& registerList)//print problem
{
	std::string num;
	std::cout << "请输入要删除的考生的考号：\n";
	while (!safe_read_num(std::cin, num)) {}
	std::cout << "您删除的考生信息是：\n";
	registerList.node_delete(num);
	registerList.print_list();
}
void system_edit(LinkedNode& temp, Linkedlist& registerList)
{
	std::string num;
	std::cout << "请输入要修改的考生的考号：\n";
	while (!safe_read_num(std::cin, num)) {}
	if (registerList.node_search(num) == nullptr)
	{
		std::cout << "查无此人\n\n";
		return;
	}
	std::cout << "请依次输入要修改的考生的考号，姓名，性别(男/女)，年龄及报考类别：\n";
	while (!safe_read_node(std::cin, temp)) {
		std::cout << "请重新输入考生信息：\n";
	}
	registerList.node_edit(num, temp);
	registerList.print_list();
}
void system_loop(LinkedNode &temp, Linkedlist &registerList)
{
	while(2077)
	{
		char choice;
		std::cout << "请选择您要进行的操作：";
		while (!(std::cin >> choice)||choice<'0'||choice>'4') {
			std::cerr << "输入错误，请输入正确操作指令: ";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		if (choice == '0')
			break;
		switch (choice)
		{
		case '1':
		{
			system_insert(temp, registerList);
			break;
		}
		case '2'://delete
		{
			system_delete(registerList);
			break;
		}
		case '3'://find
		{
			system_find(registerList);
			break;
		}
		case '4'://edit
		{
			system_edit(temp, registerList);
			break;
		}
		default:
			std::cout << "请输入正确的操作：\n";
		}
	}
}