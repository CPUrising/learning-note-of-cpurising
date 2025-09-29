#include"system.h"
void system_init(LinkedNode &temp, Linkedlist &registerList)
{
	int total;
	std::cout << "�����뽨��������Ϣϵͳ!\n";
	std::cout << "�����뿼��������\n";
	while (!(std::cin >> total)) {
		std::cerr << "�����������������: ";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	std::cout << "����������Ҫ����Ŀ����Ŀ��ţ��������Ա�(��/Ů)�����估�������\n";//remember cin error
	while (total--)
	{
		while (!safe_read_node(std::cin, temp)) {
			std::cout << "���������뿼����Ϣ��\n";
		}
		registerList.node_putback(temp);
	}
	std::cout<<std::endl;
	std::cout << "��ѡ����Ҫ���еĲ�����1Ϊ���룬2Ϊɾ����3Ϊ���ң�4Ϊ�޸ģ�0Ϊȡ��������\n\n";
}
void system_insert(LinkedNode& temp, Linkedlist& registerList) {
	int index;
	std::cout << "������Ҫ����Ŀ�����λ�ã�\n";

	// ����λ��������쳣
	while (1) {//danger ! N0 limits
		if (!(std::cin >> index))
		{
			std::cout << "�����������������: ";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else if (index<0 || index>registerList.get_size()+1)
			std::cout << "����λ�ó�����Χ�����������룺";
		else
			break;
	}

	std::cout << "����������Ҫ����Ŀ����Ŀ��ţ��������Ա�(��/Ů)�����估�������\n";

	// ѭ��ֱ���ɹ���ȡ��Ч����
	while (!safe_read_node(std::cin, temp)) {
		std::cout << "���������뿼����Ϣ��\n";
	}
	registerList.node_insert(index, temp);
	registerList.print_list();
}
void system_find(Linkedlist& registerList)
{
	std::string num;
	std::cout << "������Ҫ���ҵĿ����Ŀ��ţ�\n";
	while (!safe_read_num(std::cin, num)) {}
	std::cout << "���ҵĿ�������Ϣ��\n";
	registerList.node_find(num);
	registerList.print_list();
}
void system_delete(Linkedlist& registerList)//print problem
{
	std::string num;
	std::cout << "������Ҫɾ���Ŀ����Ŀ��ţ�\n";
	while (!safe_read_num(std::cin, num)) {}
	std::cout << "��ɾ���Ŀ�����Ϣ�ǣ�\n";
	registerList.node_delete(num);
	registerList.print_list();
}
void system_edit(LinkedNode& temp, Linkedlist& registerList)
{
	std::string num;
	std::cout << "������Ҫ�޸ĵĿ����Ŀ��ţ�\n";
	while (!safe_read_num(std::cin, num)) {}
	if (registerList.node_search(num) == nullptr)
	{
		std::cout << "���޴���\n\n";
		return;
	}
	std::cout << "����������Ҫ�޸ĵĿ����Ŀ��ţ��������Ա�(��/Ů)�����估�������\n";
	while (!safe_read_node(std::cin, temp)) {
		std::cout << "���������뿼����Ϣ��\n";
	}
	registerList.node_edit(num, temp);
	registerList.print_list();
}
void system_loop(LinkedNode &temp, Linkedlist &registerList)
{
	while(2077)
	{
		char choice;
		std::cout << "��ѡ����Ҫ���еĲ�����";
		while (!(std::cin >> choice)||choice<'0'||choice>'4') {
			std::cerr << "���������������ȷ����ָ��: ";
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
			std::cout << "��������ȷ�Ĳ�����\n";
		}
	}
}