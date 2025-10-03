#include"SortedlinkedList.h"
LinkedNode::LinkedNode()
{
	num = 86;
	next = nullptr;
}
LinkedNode::LinkedNode(int num) :
	num(num), next(NULL) {}
LinkedNode::LinkedNode(const LinkedNode& other) {
	// Copy basic members (value copy)
	num = other.num;
	next = nullptr;  //instead of "next = other.next "(shallow copy would cause pointer confusion)
}
LinkedNode& LinkedNode::operator=(const LinkedNode& other) {
	if (this != &other) {  // avoid self copy
		num = other.num;
		// ����û�и���nextָ�룬�����Ժ���Ӧ�������Ȼ��next=other.next?
	}
	return *this;
}
bool safe_read_node(std::istream& is, LinkedNode& node) {
	try {
		if (!(is >> node.num)) {
			throw std::runtime_error("���������������������");
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
LinkedNode* Linkedlist::node_search(int num)//index starts from 0
{
	LinkedNode* cur = _dummyHead->next;
	while (cur != nullptr&&cur->num!= num)
		cur = cur->next;
	if (cur == nullptr)
	{
		return nullptr;
	}
	return cur;
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
}
void Linkedlist::print_list()//no space after last node
{
	LinkedNode* cur = _dummyHead->next;
	std::cout << std::endl;
	while (cur->next != nullptr)
	{
		std::cout << cur->num << ' ';
		cur = cur->next;
	}
	std::cout << cur->num << ' ';

	std::cout << std::endl;
}