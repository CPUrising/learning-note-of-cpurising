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
		// 这里没有复制next指针，姑且以后处理。应该是深拷贝然后next=other.next?
	}
	return *this;
}
bool safe_read_node(std::istream& is, LinkedNode& node) {
	try {
		// 读取考号
		if (!(is >> node.number)) {
			throw std::runtime_error("考号输入错误，请输入整数");
		}

		// 读取姓名
		if (!(is >> node.name)) {
			throw std::runtime_error("姓名输入错误");
		}

		// 读取性别
		std::string sex;
		if (!(is >> sex)) {
			throw std::runtime_error("性别输入错误");
		}
		if (sex == "女") {
			node.sex = 1;
		}
		else if (sex == "男") {
			node.sex = 0;
		}
		else {
			throw std::runtime_error("性别输入无效，请输入'男'或'女'");
		}

		// 读取年龄
		if (!(is >> node.age)) {
			throw std::runtime_error("年龄输入错误，请输入整数");
		}
		if (node.age <= 0 || node.age > 150) {
			throw std::runtime_error("年龄值无效，必须在1-150之间");
		}

		// 读取报考类别
		if (!(is >> node.application)) {
			throw std::runtime_error("报考类别输入错误");
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
bool safe_read_num(std::istream& is, std::string &num)
{
	try {
		if (!(is >> num)) {
			throw std::runtime_error("考号输入失败");
		}
		// 新增：校验编号是否全为数字
		for (char c : num) {
			if (!std::isdigit(static_cast<unsigned char>(c))) {
				throw std::runtime_error("请输入正确格式考号:");
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "输入错误," << e.what() << std::endl;
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
	std::cout << std::setiosflags(std::ios::left)<<std::setw(SPACE) << "考号" << std::setw(SPACE) << "姓名" << std::setw(SPACE) << "性别" << std::setw(SPACE) << "年龄" << std::setw(SPACE) << "报考类别"<< std::endl;
}
void Linkedlist::print_person(const LinkedNode& node)//care the space
{
	std::cout << std::setiosflags(std::ios::left) << std::setw(SPACE) << node.number << std::setw(SPACE) << node.name;
	std::cout<< std::setw(SPACE);
	if (node.sex)
		std::cout << "女"; 
	else 
		std::cout << "男"; 
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
		std::cout << "查无此人\n";
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
		std::cout << "查无此人\n";
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