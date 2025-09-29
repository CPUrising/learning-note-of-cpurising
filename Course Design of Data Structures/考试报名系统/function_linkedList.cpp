#include"linkedList.h"
LinkedNode::LinkedNode()
{
	number = "0721";
	name = "noa";
	sex = 0;
	age = 86;
	application = "blue archive";
	next = nullptr;
}
LinkedNode::LinkedNode(std::string number, std::string name, bool sex, int age, std::string application) :
	number(number), name(name), sex(sex), age(age), application(application), next(NULL) {}
LinkedNode::LinkedNode(const LinkedNode& other) {
	// Copy basic members (value copy)
	number = other.number;
	name = other.name;
	sex = other.sex;
	age = other.age;
	application = other.application;
	// Deep copy for the next pointer: only copy the current node itself, set next to null (critical!)
	// Reason: When the new node is added to the linked list, its next pointer should be managed 
	// by the linked list itself to avoid association with the original linked list.
	next = nullptr;  //instead of "next = other.next "(shallow copy would cause pointer confusion)
}
LinkedNode& LinkedNode::operator=(const LinkedNode& other) {
	if (this != &other) {  // avoid self copy
		number = other.number;
		name = other.name;
		sex = other.sex;
		age = other.age;
		application = other.application;
		// ����û�и���nextָ�룬�����Ժ���Ӧ�������Ȼ��next=other.next?
	}
	return *this;
}
bool safe_read_node(std::istream& is, LinkedNode& node) {
	try {
		// ��ȡ����
		if (!(is >> node.number)) {
			throw std::runtime_error("���������������������");
		}

		// ��ȡ����
		if (!(is >> node.name)) {
			throw std::runtime_error("�����������");
		}

		// ��ȡ�Ա�
		std::string sex;
		if (!(is >> sex)) {
			throw std::runtime_error("�Ա��������");
		}
		if (sex == "Ů") {
			node.sex = 1;
		}
		else if (sex == "��") {
			node.sex = 0;
		}
		else {
			throw std::runtime_error("�Ա�������Ч��������'��'��'Ů'");
		}

		// ��ȡ����
		if (!(is >> node.age)) {
			throw std::runtime_error("���������������������");
		}
		if (node.age <= 0 || node.age > 150) {
			throw std::runtime_error("����ֵ��Ч��������1-150֮��");
		}

		// ��ȡ�������
		if (!(is >> node.application)) {
			throw std::runtime_error("��������������");
		}

		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "�������: " << e.what() << std::endl;

		// �������״̬�����Ե�ǰ��ʣ�����ݣ��Ա��´�����
		is.clear();
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		return false;
	}
}
bool safe_read_num(std::istream& is, std::string &num)
{
	try {
		if (!(is >> num)) {
			throw std::runtime_error("��������ʧ��");
		}
		// ������У�����Ƿ�ȫΪ����
		for (char c : num) {
			if (!std::isdigit(static_cast<unsigned char>(c))) {
				throw std::runtime_error("��������ȷ��ʽ����:");
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "�������," << e.what() << std::endl;
		is.clear();
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return false;
	}
}
Linkedlist::Linkedlist()
{
	_dummyHead = new LinkedNode;
	_size = 0;
}
Linkedlist::~Linkedlist()
{
	LinkedNode* cur = _dummyHead;
	while (cur != nullptr)
	{
		LinkedNode* temp = cur;
		cur = cur->next;
		delete temp;
	}
}
int Linkedlist::get_size()
{
	return _size;
}
void Linkedlist::print_head_row()//care the space
{
	std::cout << std::setiosflags(std::ios::left)<<std::setw(SPACE) << "����" << std::setw(SPACE) << "����" << std::setw(SPACE) << "�Ա�" << std::setw(SPACE) << "����" << std::setw(SPACE) << "�������"<< std::endl;
}
void Linkedlist::print_person(const LinkedNode& node)//care the space
{
	std::cout << std::setiosflags(std::ios::left) << std::setw(SPACE) << node.number << std::setw(SPACE) << node.name;
	std::cout<< std::setw(SPACE);
	if (node.sex)
		std::cout << "Ů"; 
	else 
		std::cout << "��"; 
	std::cout << std::setw(SPACE) << node.age << std::setw(SPACE) << node.application << std::endl;
}
LinkedNode* Linkedlist::node_search(std::string num)//index starts from 0
{
	LinkedNode* cur = _dummyHead->next;
	while (cur != nullptr&&cur->number!= num)
		cur = cur->next;
	if (cur == nullptr)
	{
		return nullptr;
	}
	return cur;
}
void Linkedlist::node_find(std::string num)
{
	LinkedNode* cur = node_search(num);
	if (cur == nullptr)
		std::cout << "���޴���\n";
	else
	{
		print_person(*cur);
	}
}
void Linkedlist::node_putback(const LinkedNode& node)
{
	LinkedNode* cur = _dummyHead;
	while (cur->next != nullptr)
	{
		cur = cur->next;
	}
	LinkedNode* newNode = new LinkedNode(node);
	cur->next = newNode;
	_size++;
}
void Linkedlist::node_insert(int index, const LinkedNode& node)
{
	LinkedNode* cur = _dummyHead;
	while(--index)
		cur= cur->next;
	LinkedNode* newNode = new LinkedNode(node);
	newNode->next = cur->next;
	cur->next = newNode;
	_size++;
}
void Linkedlist::node_delete(std::string num)
{
	LinkedNode* cur = _dummyHead;
	while (cur->next != nullptr && cur->next->number != num) {
		cur = cur->next;
	}
	if (cur->next == nullptr)
	{
		std::cout << "���޴���\n";
	}
	else
	{
		LinkedNode* temp = cur->next;
		cur->next = cur->next->next;
		delete temp;
		temp = nullptr;
		_size--;
	}
}
void Linkedlist::node_edit(std::string num, const LinkedNode& node)
{
	LinkedNode* cur = node_search(num);
	*cur = node;
}
void Linkedlist::print_list()
{
	LinkedNode* cur = _dummyHead->next;
	std::cout << std::endl;
	print_head_row();
	while (cur != nullptr)
	{
		print_person(*cur);
		cur = cur->next;
	}
	std::cout << std::endl;
}