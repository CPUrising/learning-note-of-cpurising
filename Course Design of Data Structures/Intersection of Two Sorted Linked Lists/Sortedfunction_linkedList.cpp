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
		// 这里没有复制next指针，姑且以后处理。应该是深拷贝然后next=other.next?
	}
	return *this;
}
bool safe_read_node(std::istream& is, LinkedNode& node) {
	try {
		if (!(is >> node.num)) {
			throw std::runtime_error("数据输入错误，请输入整数");
		}
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "输入错误: " << e.what() << std::endl;

		// 清除错误状态并忽略当前行剩余内容，以便下次输入
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
void Linkedlist::node_pushback(int num)
{
	LinkedNode* cur = _dummyHead;
	while (cur->next != nullptr)
	{
		cur = cur->next;
	}
	LinkedNode* newNode = new LinkedNode(num);
	cur->next = newNode;
}
void Linkedlist::node_pushback_loop()
{
	int n;
	LinkedNode* cur = _dummyHead;
	while(1)
	{
		std::cin >> n;
		if (n == -1)
			break;
		node_pushback(n);
	}
}
void Linkedlist::sorted(Linkedlist& s1, Linkedlist& s2, Linkedlist& s3)//to be repaired
{
	s3._dummyHead->next = nullptr;
	LinkedNode* p1 = s1._dummyHead->next;
	LinkedNode* p2 = s2._dummyHead->next;
	int lastAdded = -1; 

	while (p1 != nullptr && p2 != nullptr)
	{
		if (p1->num == p2->num) {
			if (p1->num != -1) {
				s3.node_pushback(p1->num);
				lastAdded = p1->num;
			}
			p1 = p1->next;
			p2 = p2->next;
		}
		else if (p1->num < p2->num) {
			p1 = p1->next;
		}
		else {
			p2 = p2->next;
		}
	}
}
void Linkedlist::print_list()//no space after last node
{
	LinkedNode* cur = _dummyHead->next;
	if (cur == nullptr) {
		std::cout << "NULL" << std::endl;
		return;
	}
	while (cur->next!= nullptr)
	{
		std::cout << cur->num << ' ';
		cur = cur->next;
	}
	std::cout << cur->num;

	std::cout << std::endl;
}