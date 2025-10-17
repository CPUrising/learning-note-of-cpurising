void input(int &len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good() || len < 1 || len > 10)
		{
			cin.ignore('\n', 1000);
			cout << "Please re-enter a positive integer (1-10):\n";
		}
		else
			break;
	}
}
