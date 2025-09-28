#include"system.h"
void system_init(LinkedNode &temp, Linkedlist &registerList)
{
	int total;
	std::cout << "�����뽨��������Ϣϵͳ!\n";
	std::cout << "�����뿼��������\n";
	std::cin >> total;
	std::cout << "���������뿼���Ŀ��ţ��������Ա����估�������\n";//remember cin error
	while (total--)
	{
		std::cin >> temp;
		registerList.node_putback(temp);
	}
	std::cout<<std::endl;
	std::cout << "��ѡ����Ҫ���еĲ�����1Ϊ���룬2Ϊɾ����3Ϊ���ң�4Ϊ�޸ģ�0Ϊȡ��������\n\n";
}
void system_insert(LinkedNode& temp, Linkedlist& registerList)
{
	int index;
	std::cout << "������Ҫ����Ŀ�����λ�ã�\n";
	std::cin >> index;
	std::cout << "����������Ҫ����Ŀ����Ŀ��ţ��������Ա����估�������\n";
	std::cin >> temp;
	registerList.node_insert(index, temp);
	registerList.print_list();
}
void system_find(Linkedlist& registerList)
{
	std::string num;
	std::cout << "������Ҫ���ҵĿ����Ŀ��ţ�\n";
	std::cin >> num;
	registerList.node_find(num);
	registerList.print_list();
}
void system_delete(Linkedlist& registerList)//print problem
{
	std::string num;
	std::cout << "������Ҫɾ���Ŀ����Ŀ��ţ�\n";
	std::cin >> num;
	std::cout << "��ɾ���Ŀ�����Ϣ�ǣ�\n";
	registerList.node_find(num);
	registerList.node_delete(num);
	registerList.print_list();
}
void system_edit(LinkedNode& temp, Linkedlist& registerList)
{
	std::string num;
	std::cout << "������Ҫ�޸ĵĿ����Ŀ��ţ�\n";
	std::cin >> num;
	registerList.node_edit(num, temp);
	registerList.print_list();
}
void system_loop(LinkedNode &temp, Linkedlist &registerList)
{
	while(2077)
	{
		char choice;
		std::cout << "��ѡ����Ҫ���еĲ�����";
		std::cin >> choice;
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